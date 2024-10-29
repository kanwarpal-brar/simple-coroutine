#include "boost/context/continuation.hpp"
#include <boost/context/fixedsize_stack.hpp>
#include <iostream>
namespace ctx=boost::context;

int BASEID = 0;

// Consturctor side CorJob acts as a "client" to the real CorJob placed on Coroutine stack
class CorJob  {
    ctx::continuation cont;
    CorJob* companion = nullptr;
    bool finished = false;
    int id = BASEID++;

  protected:
    ctx::continuation&& main(ctx::continuation&& sink) {
      // TODO: needs wrapper
      // override this method
      std::cout << "1" << std::endl;
      sink=sink.resume();
      std::cout << "2" << std::endl;

      // invalidate parent's ptr to self on end
      companion->companion=nullptr; // TODO: put in wrapper
      return std::move(sink);
    }

  private:

    // New Stack Companion CorJob
    template< typename StackAllocator >
    CorJob(void * stk_ptr, std::size_t size, ctx::stack_context sctx, StackAllocator salloc, CorJob* parent_comp): cont{}, companion{parent_comp} {
      cont=ctx::callcc(
        std::allocator_arg,
        ctx::preallocated(stk_ptr, size, sctx),
        salloc,
        std::bind(&CorJob::main, this, std::placeholders::_1)
        // std::bind(&CorJob::bootstrap, std::placeholders::_1, this)
      ); // bootstrap needs access to the callcc stack-top object ptr
      std::cout << "exited const" << std::endl;
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
    }

  public:

    bool is_done() {
      // TODO: handle setting self to finished
      if (!finished && companion != nullptr) {
          return companion->is_done();
      }
      return finished;
      // return companion->is_done() ? companion != nullptr : finished;
    }

    CorJob() {
      init_stack();
    }

    void operator()() {
      // TODO: need a way to handle when companion is finished, and in that case do not re-resume
      std::cout << "retry" << std::endl;
      ctx::continuation && c = companion->cont.resume();
      std::cout << "resumed: " << companion << std::endl;
      if (companion != nullptr && c) {  // Check if continuation is valid
            companion->cont = std::move(c);
      }
      std::cout << "exited" << std::endl;
    }
};



ctx::continuation&& foo(ctx::continuation&& sink) {
    std::cout << "2" << std::endl;
    return sink.resume();
}