//
// Created by swagger on 2022/6/16.
//
#include "routine.h"
#include "task_control.h"
#include "task_group.h"
#include <iostream>
#include <atomic>

using namespace std;

static std::mutex task_control_modify_mutex_ = {};
TaskControl* task_control = { nullptr };
extern __thread TaskGroup* tls_task_group;

bool init() {
  std::unique_lock<std::mutex> guard(task_control_modify_mutex_);
  if ( task_control != nullptr ) {
    return false;
  }
  task_control = new TaskControl;
  task_control->init();
  return true;
}



void routine_start_background(void (*fn)(void*), void* arg) {
  if ( tls_task_group == nullptr ) {
    if ( task_control == nullptr ) {
      return;
    }
    return task_control->pick_and_run_background(fn, arg);
  }
  tls_task_group->start_background(fn, arg);
}

void yield() {
  if ( tls_task_group == nullptr ) {
    cout << "error!" << endl;
    return;
  }
  tls_task_group->yield();
}