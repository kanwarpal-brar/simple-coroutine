#ifndef CORJOB_H
#define CORJOB_H

#ifdef CORJOB_DEBUG
#include <iostream>
// TODO: create thread-safe printer class
#define PRINT(msg) (std::cout << msg << std::endl)
#else
#define PRINT(msg) {}
#endif

#include "boost/context/continuation.hpp"
#include <boost/context/fixedsize_stack.hpp>

namespace ctx=boost::context;

int BASEID = 0;

// Consturctor side CorJob acts as a "client" to the real CorJob placed on Coroutine stack
class CorJob  {
  ctx::continuation cont;
  CorJob* companion = nullptr;
  ctx::continuation sink_val = ctx::continuation{};
  bool finished = false;
  int id = BASEID++;

  protected:
  virtual void main() {
    PRINT("Coroutine Initied");
    PRINT("1");
    suspend();
    PRINT("2");
    suspend();
    PRINT("3");
  }

  void suspend() {
    PRINT("Suspending");
    sink_val = std::move(sink_val.resume());
    PRINT("Back from Suspension");
  }

  private:
    ctx::continuation&& bootstrap(ctx::continuation&& sink) {
      PRINT("Entered Bootstrap");
      sink_val = std::move(sink);
      suspend(); // initial suspend
      main();
      companion->set_finished();
      return std::move(sink_val);
    }

    void set_finished() {
      finished = true;
      companion = nullptr;
    }

    void resume() {
      PRINT("resume() enter");
      if (is_done()) throw FinishedException();
      ctx::continuation c = companion->cont.resume();
      if (!is_done()) companion->cont = std::move(c);
      PRINT("resume() exit");
    }

    // New Stack Companion CorJob
    template< typename StackAllocator >
    CorJob(void * stk_ptr, std::size_t size, ctx::stack_context sctx, StackAllocator salloc, CorJob* parent_comp): cont{}, companion{parent_comp} {
      cont=ctx::callcc(
      std::allocator_arg,
      ctx::preallocated(stk_ptr, size, sctx),
      salloc,
      std::bind(&CorJob::bootstrap, this, std::placeholders::_1)
      );
      PRINT("exited callcc");
    }

    void init_stack() {
      // create stack allocator; StackAllocator is fixedsize_stack
      ctx::fixedsize_stack salloc(4096);

      // allocate stack space
      ctx::stack_context sctx(salloc.allocate());

      // Move stack pointer to reserve space for control structure
      void * stk_ptr=static_cast<char*>(sctx.sp)-sizeof(CorJob);
      std::size_t size=sctx.size-sizeof(CorJob);

      // place new mirror CorJob on stack, pass self as companion
      companion=new(stk_ptr) CorJob(stk_ptr, size, sctx, salloc, this);

      PRINT("exited init");
    }

  public:

    bool is_done() {
      return finished || companion == nullptr;
    }

    CorJob() {
      init_stack();
    }

    class FinishedException : public std::exception {};  // Thrown when attempting to resume a terminated coroutine


    void operator()() {
      PRINT("operator()() enter");
      resume();
      PRINT("operator()() exit");
    }
};

class BaseCoroutine;  // Forward declaration

template <class T>
concept BaseCoroutineType = std::is_base_of<BaseCoroutine, T>::value;

template <BaseCoroutineType T>
class Coroutine {
  private:
    bool finished = false;  // Indicator of coroutine completion
    T* coroutine = nullptr;  // Pointer to actual coroutine object matching T
    ctx::continuation coroutine_ctx;  // Context of coroutine

    friend class BaseCoroutine;  // Required for internal set_finished() access

    /*
    Used internally by BaseCoroutine to signal done
    */
    void set_finished() {
      finished = true;
    }

    /*
    Used internally to resume self.coroutine through self.coroutine_ctx
    */
    void resume() {
      PRINT("Coroutine::resume() enter");
      if (finished) throw typename T::FinishedException();
      ctx::continuation c = coroutine_ctx.resume();
      if (!finished) coroutine_ctx = std::move(c);
      PRINT("Coroutine::resume() exit");
    }

  public:
    // Variadic constructor for coroutine arguments
    template <typename... Args>
    Coroutine(Args&&... args) {
      static_assert(std::is_constructible<T, Args...>::value, "Arguments must be valid for constructing an object of type T");

      PRINT("entered Coroutine construct");

      // create stack allocator; StackAllocator selected here is fixedsize_stack
      ctx::fixedsize_stack salloc(4096);

      // allocate stack space
      ctx::stack_context sctx(salloc.allocate());

      // Move stack pointer to reserve space for control structure
      void * stk_ptr=static_cast<char*>(sctx.sp)-sizeof(T);
      std::size_t size=sctx.size-sizeof(T);

      // place coroutine of T on new stack
      coroutine=new(stk_ptr)T(std::forward<Args>(args)...);

      /*
      TODO: below operation is expensive but I don't know how to avoid it
        maybe there's some way to pass an additional arg to the bootstrap?
      */ 
      coroutine->set_parent((Coroutine<BaseCoroutine>*)this);

      // start coroutine context:
      coroutine_ctx=ctx::callcc(
        std::allocator_arg,
        ctx::preallocated(stk_ptr, size, sctx),
        salloc,
        std::bind(&T::bootstrap, coroutine, std::placeholders::_1)
      );

      PRINT("exited Coroutine construct");
    }

    /*
    Check if coroutine has ended mainloop
    */
    bool is_finished() {
      return finished || coroutine == nullptr;
    }

    /*
    Resume coroutine
    */
    void operator()() {
      PRINT("operator()() enter");
      resume();
      PRINT("operator()() exit");
    }
};

/*
Fundamental Parent for Creating Custom Coroutines.
Inherit from this class and provide an override main() method to create a custom coroutine.
Provides a suspend() method to pause execution
*/
class BaseCoroutine {
  private:
    int id = BASEID++;
    ctx::continuation caller = ctx::continuation{};  // Context of latest caller

    Coroutine<BaseCoroutine> *parent;  // Pointer to parent Coroutine wrapper

    // TODO: add "initial_caller" to handle resumption loops

    // TODO: implement local resume() for self-resumption?
    // void resume();

    template <BaseCoroutineType T>
    friend class Coroutine;  // Required for bootstrap access

    ctx::continuation &&bootstrap(ctx::continuation &&caller_ctx) {
      PRINT("Entered Bootstrap");
      caller = std::move(caller_ctx);
      suspend(); // initial suspend
      main();
      parent->set_finished();
      return std::move(caller);
    }

    // template <BaseCoroutineType T>
    void set_parent(Coroutine<BaseCoroutine> *parent) {
      this->parent = parent;
    }
    
  protected:
    virtual void main() = 0; // Mainloop of coroutine

    /*
    Used to pause execution of coroutine; exposed to all children
    */
    void suspend() {
      PRINT("Suspending");
      caller = std::move(caller.resume());
      PRINT("Back from Suspension");
    }

  public:
    class FinishedException : public std::exception {};  // Thrown when attempting to resume a terminated coroutine
};


#endif