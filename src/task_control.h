//
// Created by swagger on 2022/6/19.
//

#ifndef ROUTINE_TASK_CONTROL_H
#define ROUTINE_TASK_CONTROL_H

#include <thread>
#include <vector>
#include <atomic>
#include <mutex>

class TaskGroup;
class TaskMeta;

class TaskControl {
public:

  TaskControl();

  void init(int num = 4);

  static void worker_thread(void* arg);


  /**
   * 创建TaskGroup
   * @return
   */
  TaskGroup* create_group();

  void add_group(TaskGroup* g);

  void pick_and_run_background(void (*fn)(void*), void* arg);

  void join();

  bool steal_task(TaskMeta** meta);

private:

  std::vector<std::thread> workers_;
  int workers_num_;

  std::vector<TaskGroup*> groups_;
  std::atomic<int> ngroups_;


  std::mutex modify_groups_mutex_;

};

#endif //ROUTINE_TASK_CONTROL_H
