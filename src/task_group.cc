//
// Created by swagger on 2022/6/19.
//
#include <iostream>
#include "task_control.h"
#include "task_group.h"
#include "task_meta.h"

// for debug
using std::cout;
using std::endl;

__thread TaskGroup* tls_task_group = nullptr;

TaskGroup::TaskGroup(TaskControl* task_control)
: rq_()
, remote_rq_()
, task_control_(task_control)
{
}

bool TaskGroup::init() {
  main_task_meta_ = new TaskMeta;
  rq_.init(1024);
  // main stack
  return main_task_meta_->get_stack(0, nullptr);
}


void clean_task_meta(void* args) {
  delete (TaskMeta*)args;
}

struct PushRemoteRqArgs {
  TaskMeta* task_meta;

};

void TaskGroup::push_remote_rq(void* args) {
  tls_task_group->remote_rq_.push((TaskMeta*)args);
  // cout << "callback ok" << endl;
}

void TaskGroup::push_rq(void* args) {
  while ( tls_task_group->rq_.push((TaskMeta*)args) == false);
}


void TaskGroup::run_main_task() {
  TaskMeta* next_meta;
  TaskGroup* self = this;
  while (wait_task(&next_meta)) {
    sched_to(next_meta);
  }
}

bool TaskGroup::wait_task(TaskMeta **next_meta) {
  while (true) {
    if (steal_task(next_meta)) {
      return true;
    }
    // sleep(1);
  }
}

void TaskGroup::sched_to(TaskMeta *next_meta) {
  // 还没有初始化栈，那么就初始化出一个栈给它
  if ( next_meta->context_ == nullptr ) {
    bool ret = next_meta->get_stack(1, task_runner);
    if ( !ret ) {
      cout << "get_stack fail..." << endl;
      return;
    }
  }
  // *********** ending_sched

  // TaskMeta* cur_meta = (*g)->cur_meta_;
  TaskMeta* cur_meta = tls_task_group->cur_meta_;
  tls_task_group->cur_meta_ = next_meta; // 新栈
  jump_stack(cur_meta->context_, next_meta->context_);

  TaskGroup* g = tls_task_group;
  while (g->last_routine_end_callback_ != nullptr) {
    g->last_routine_end_callback_(g->last_routine_end_callback_args);
    g->last_routine_end_callback_ = nullptr;
    g->last_routine_end_callback_args = nullptr;
  }
}

bool TaskGroup::steal_task(TaskMeta **meta) {
  // 先从本地的remote_rq中获取，没有再到全局中获取
  if ( remote_rq_.pop(meta) ) {
    return true;
  }
  return task_control_->steal_task(meta);
}

void TaskGroup::task_runner(void *arg) {
  TaskGroup* g = tls_task_group;
  do {

    while (tls_task_group->last_routine_end_callback_ != nullptr) {
      tls_task_group->last_routine_end_callback_(tls_task_group->last_routine_end_callback_args);
      tls_task_group->last_routine_end_callback_ = nullptr;
      tls_task_group->last_routine_end_callback_args = nullptr;
    }

    // cout << "start running..." << endl;

    TaskMeta *const m = g->cur_meta_;

    m->callback_(m->arg);

    g = tls_task_group;

    // cout << "run ok" << endl;
    // 跳到新的meta上
    // ending_sched
    TaskMeta *next_meta = nullptr;
    const bool popped = g->rq_.pop(&next_meta);
    if ( !popped ) {
      next_meta = g->main_task_meta_;
    }
    g->set_remained(clean_task_meta, m);
    g->sched_to(next_meta);
  }while (g->cur_meta_ != g->main_task_meta_);
}

void TaskGroup::start_background(void (*fn)(void *), void *arg) {
  TaskMeta* meta = new TaskMeta;
  meta->callback_ = fn;
  meta->arg = arg;
  remote_rq_.push(meta);
}


void TaskGroup::yield() {
  TaskMeta* const cur_meta = tls_task_group->cur_meta_;
  TaskMeta* next_meta;
  const bool popped = tls_task_group->rq_.pop(&next_meta);
  if ( !popped ) {
    next_meta = main_task_meta_;
    // cout << "pick main routine" << endl;
  }
  tls_task_group->set_remained(&TaskGroup::push_rq, cur_meta);
  sched_to(next_meta);
}

void TaskGroup::set_remained(void (*fn)(void *), void *args) {
  if ( last_routine_end_callback_ != nullptr ) {
    cout << "set remained error" << endl;
  }
  last_routine_end_callback_ = fn;
  last_routine_end_callback_args = args;
}