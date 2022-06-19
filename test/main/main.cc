//
// Created by swagger on 2022/6/19.
//
#include <iostream>
#include <thread>
#include "src/context.h"

using namespace std;

Context* main_context = nullptr;
Context* sub_context = nullptr;
Context* new_context = nullptr;

__thread int test = 10;

void func(void*);
void new_thread();

int main() {
  main_context = get_main_stack();
  sub_context = get_stack(func);
  thread t1(new_thread);
  cout << "main_thread &test: " << &test << endl;
  jump_stack(main_context, sub_context);
  cout << "end" << endl;
  t1.join();
}

void func(void* arg) {
  cout << "func        &test: " << &test << endl;
  jump_stack(sub_context, main_context);
  cout << "func        &test: " << &test << endl;
  jump_stack(sub_context, new_context);
}

void new_thread() {
  new_context = get_main_stack();
  jump_stack(new_context, sub_context);
  cout << "new_thread  &test: "  << &test << endl;
}
