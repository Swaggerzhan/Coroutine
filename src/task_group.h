//
// Created by swagger on 2022/6/19.
//

#ifndef ROUTINE_TASK_GROUP_H
#define ROUTINE_TASK_GROUP_H

#include "block_queue.hpp"
#include "task_meta.h"

class TaskControl;

class TaskGroup {
public:


  TaskGroup(TaskControl*);

  bool init();

  // pthread启动时会跑在当前的函数上，当这个函数退出时
  // worker也就退出了
  void run_main_task();


  bool wait_task(TaskMeta** next_meta);

  bool steal_task(TaskMeta** meta);

  void sched_to(TaskMeta* next_meta);

  // arg将被传入TaskGroup*
  static void task_runner(void* arg);


  void start_background(void (*fn)(void*), void* args);

  void yield();

  void set_remained(void (*fn)(void*), void* args);

  static void push_remote_rq(void* args);
  static void push_rq(void* args);

private:
  friend class TaskControl;

  TaskControl* task_control_;

  TaskMeta* main_task_meta_; // 线程中的主协程
  TaskMeta* cur_meta_;        // 当前正在跑的协程

  WorkStealingQueue rq_;
  BlockQueue remote_rq_;

  // 切换或者结束时调用，由下一个协程启动前来调用
  fn last_routine_end_callback_;
  void* last_routine_end_callback_args;


};






#endif //ROUTINE_TASK_GROUP_H
