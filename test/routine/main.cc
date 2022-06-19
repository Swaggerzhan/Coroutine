//
// Created by swagger on 2022/6/19.
//
#include "src/routine.h"
#include <unistd.h>
#include <iostream>
#include <sys/time.h>
#include <thread>

using namespace std;

void func(void* arg) {
  thread::id id = this_thread::get_id();
  yield();
  thread::id after = this_thread::get_id();
  if ( id != after ) {
    cout << id << " to " << after << endl;
  }
}

int main() {
  init();
  for (int i=0; i<100000; ++i) {
    routine_start_background(func, nullptr);
  }
  sleep(3);
}
