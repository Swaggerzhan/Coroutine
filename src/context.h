//
// Created by swagger on 2022/6/17.
//

#ifndef ROUTINE_CONTEXT_H
#define ROUTINE_CONTEXT_H


#include <unistd.h>



typedef void* stack_ptr_t;

extern "C" {

stack_ptr_t make_context(void *stack_bottom, size_t size, void (*fn)(void* arg));

void jump_context(stack_ptr_t* from, void* to, intptr_t vp=0, bool fpu=false);

};



struct Context {
  void* stack_mem; // raw mem
  void* stack_bottom; // stack_mem + stack_size
  size_t stack_size;
  stack_ptr_t stack;
};


/**
 * 主协程栈
 * @return
 */
Context* get_main_stack();

Context* get_stack(void (*fn)(void* arg));

void jump_stack(Context* from, Context* to);

bool is_main_stack(Context* c);

#endif //ROUTINE_CONTEXT_H
