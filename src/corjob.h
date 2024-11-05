#ifndef CORJOB_H
#define CORJOB_H

#ifdef CORJOB_DEBUG
#include <iostream>
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
  void main() {
    PRINT("Coroutine Initied");
    PRINT("1");
    suspend();
    PRINT("2");
    suspend();
    PRINT("3");
  }

  ctx::continuation&& bootstrap(ctx::continuation&& sink) {
    PRINT("Entered Bootstrap");
    sink_val = std::move(sink);
    suspend(); // initial suspend
    main();
    companion->set_finished();
    return std::move(sink_val);
  }

  private:

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

  void suspend() {
    PRINT("Suspending");
    sink_val = std::move(sink_val.resume());
    PRINT("Back from Suspension");
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
    companion=new(stk_ptr)CorJob(stk_ptr, size, sctx, salloc, this);

    PRINT("exited init");
  }

  public:

  bool is_done() {
    return finished || companion == nullptr;
  }

  CorJob() {
    init_stack();
  }

  class FinishedException : public std::exception {};

  void operator()() {
    PRINT("operator()() enter");
    resume();
    PRINT("operator()() exit");
  }
};

#endif