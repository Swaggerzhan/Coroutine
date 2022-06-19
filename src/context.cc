//
// Created by swagger on 2022/6/17.
//
#include "context.h"

__asm (".text\n"
       ".globl make_context\n"
       ".type make_context,@function\n"
       ".align 16\n"
       "make_context:\n"
       "  movq %rdi, %rax\n" // void* sp
       "  andq $-16, %rax\n" // 16对齐
       "  leaq -0x48(%rax), %rax\n"
       "  movq %rdx, 0x38(%rax)\n" // fn
       "  stmxcsr (%rax)\n"
       "  fnstcw 0x4(%rax)\n"
       "  leaq finish(%rip), %rcx\n"
       "  movq %rcx, 0x40(%rax)\n"
       "  ret\n"
       "finish: \n"
       "  xorq %rdi, %rdi\n"
       "  call _exit@PLT\n"
       "  hlt\n"
       ".size make_context,.-make_context\n"
       ".section .note.GNU-stack,\"\",%progbits\n"
       ".previous\n"
);


__asm (
".text\n"
".globl jump_context\n"
".type jump_context,@function\n"
".align 16\n"
"jump_context:\n"
"    pushq  %rbp  \n"
"    pushq  %rbx  \n"
"    pushq  %r15  \n"
"    pushq  %r14  \n"
"    pushq  %r13  \n"
"    pushq  %r12  \n"
"    leaq  -0x8(%rsp), %rsp\n"
"    cmp  $0, %rcx\n"
"    je  1f\n"
"    stmxcsr  (%rsp)\n"
"    fnstcw   0x4(%rsp)\n"
"1:\n"
"    movq  %rsp, (%rdi)\n"
"    movq  %rsi, %rsp\n"
"    cmp  $0, %rcx\n"
"    je  2f\n"
"    ldmxcsr  (%rsp)\n"
"    fldcw  0x4(%rsp)\n"
"2:\n"
"    leaq  0x8(%rsp), %rsp\n"
"    popq  %r12  \n"
"    popq  %r13  \n"
"    popq  %r14  \n"
"    popq  %r15  \n"
"    popq  %rbx  \n"
"    popq  %rbp  \n"
"    popq  %r8\n"
"    movq  %rdx, %rax\n"
"    movq  %rdx, %rdi\n"
"    jmp  *%r8\n"
".size jump_context,.-jump_context\n"
".section .note.GNU-stack,\"\",%progbits\n"
".previous\n"
);

// 128KB
#define STACK_SIZE 1024 * 128

Context* get_main_stack() {
  Context* c = new Context;
  if ( nullptr == c ) {
    return nullptr;
  }
  c->stack_mem = nullptr;
  c->stack_bottom = nullptr;
  c->stack_size = -1;
  c->stack = nullptr;
  return c;
}

Context* get_stack(void (*fn)(void* arg)) {
  Context* c = new Context;
  if ( nullptr == c ) {
    return nullptr;
  }
  c->stack_mem = new char[STACK_SIZE];
  c->stack_bottom = (char*)c->stack_mem + STACK_SIZE;
  c->stack_size = STACK_SIZE;
  c->stack = make_context(c->stack_bottom, c->stack_size, fn);
  return c;
}

void jump_stack(Context* from, Context* to) {
  jump_context(&from->stack, to->stack);
}

bool is_main_stack(Context* c) {
  return c->stack_size == -1;
}