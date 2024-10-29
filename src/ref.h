#include "boost/context/continuation.hpp"
#include <boost/context/fixedsize_stack.hpp>
#include <iostream>

using namespace boost::context;

continuation&& foo(continuation&& sink) {
    std::cout << "1" << std::endl;
    sink=sink.resume();
    std::cout << "2" << std::endl;
    return std::move(sink);
}

struct my_control_structure  {
    // captured continuation
    continuation  c;
    template< typename StackAllocator >
    my_control_structure(void * sp, std::size_t size, stack_context sctx, StackAllocator salloc) :
        // create captured continuation
        c{
        ctx::callcc(std::allocator_arg, preallocated(sp,size,sctx), salloc, foo)} {}
};

void init() {
    // stack-allocator used for (de-)allocating stack
    fixedsize_stack salloc(4048);
    // allocate stack space
    stack_context sctx(salloc.allocate());
    // reserve space for control structure on top of the stack
    void * sp=static_cast<char*>(sctx.sp)-sizeof(my_control_structure);
    std::size_t size=sctx.size-sizeof(my_control_structure);
    // placement new creates control structure on reserved space
    my_control_structure * cs=new(sp)my_control_structure(sp,size,sctx,salloc);
    cs->c = cs->c.resume();
    // destructing the control structure
    cs->~my_control_structure();
}