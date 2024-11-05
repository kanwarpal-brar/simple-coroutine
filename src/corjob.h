#ifndef CORJOB_H
#define CORJOB_H

#include "boost/context/continuation.hpp"
#include <boost/context/fixedsize_stack.hpp>
#include <iostream>
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
      std::cout << "Coroutine Initied" << std::endl;
      std::cout << "1" << std::endl;
      suspend();
      std::cout << "2" << std::endl;
      suspend();
      std::cout << "3" << std::endl;
    }

    ctx::continuation&& bootstrap(ctx::continuation&& sink) {
      std::cout << "Entered Bootstrap" << std::endl;
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
      std::cout << "resume() enter" << std::endl;
      if (is_done()) throw FinishedException();
      ctx::continuation c = companion->cont.resume();
      if (!is_done()) companion->cont = std::move(c);
      std::cout << "resume() exit" << std::endl;
    }

    void suspend() {
      std::cout << "Suspending" << std::endl;
      sink_val = std::move(sink_val.resume());
      std::cout << "Back from Suspension" << std::endl;
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
      std::cout << "exited callcc" << std::endl;
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

      std::cout << "exited init" << std::endl;
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
      std::cout << "operator()() enter" << std::endl;
      resume();
      std::cout << "operator()() exit" << std::endl;
    }
};

#endif