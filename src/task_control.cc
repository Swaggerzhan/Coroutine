//
// Created by swagger on 2022/6/19.
//

#include <iostream>
#include "task_control.h"
#include "task_group.h"

// for debug
using std::endl;
using std::cout;


extern __thread TaskGroup* tls_task_group;

TaskControl::TaskControl()
: ngroups_(0)
{

}

void TaskControl::init(int num) {
  workers_.reserve(num);
  groups_.reserve(num);
  for (int i=0; i<num; ++i) {
    workers_.emplace_back(&TaskControl::worker_thread, this);
  }
  while ( ngroups_.load() != num );
}

void TaskControl::worker_thread(void *arg) {
  TaskControl* self = (TaskControl*)arg;
  if ( tls_task_group != nullptr ) {
    // cout << "Error at tls_task_group not nullptr" << endl;
  }
  TaskGroup* g = self->create_group();
  if ( g == nullptr ) {
    // cout << "create_group fail..." << endl;
    return;
  }
  g->cur_meta_ = g->main_task_meta_;
  tls_task_group = g;
  // 添加group
  self->add_group(g);

  g->run_main_task();
  // cout << "task group exit..." << endl;
}

TaskGroup* TaskControl::create_group() {
  TaskGroup* g = new TaskGroup(this);
  if (!g->init() ) {
    return nullptr;
  }
  return g;
}

void TaskControl::add_group(TaskGroup *g) {
  std::unique_lock<std::mutex> guard(modify_groups_mutex_);
  groups_.push_back(g);
  ngroups_.fetch_add(1);
}

void TaskControl::join() {
  for (int i=0; i<workers_.size(); ++i) {
    workers_[i].join();
  }
}


void TaskControl::pick_and_run_background(void (*fn)(void *), void *arg) {
  int offset = rand() % workers_.size();
  // cout << "pick: " << offset << endl;
  groups_[offset]->start_background(fn, arg);
}


bool TaskControl::steal_task(TaskMeta **meta) {
  // 随机选一个偷
  int offset = rand() % ngroups_;

  for (int i=offset; i<ngroups_; ++i) {
    int index = i % ngroups_;
    if (groups_[index]->rq_.steal(meta) ) {
      return true;
    }
    if ( groups_[index]->remote_rq_.pop(meta)) {
      return true;
    }
  }
  return false;
}