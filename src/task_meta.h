//
// Created by swagger on 2022/6/19.
//

#ifndef ROUTINE_TASK_META_H
#define ROUTINE_TASK_META_H

#include "context.h"

typedef int routine_id;
typedef void(*fn)(void*);

class TaskMeta {
public:

  TaskMeta();

  /**
   * @param 0 为main stack，1为普通stack
   * @return
   */
  bool get_stack(int, void (*fn)(void* arg));


private:
  friend class TaskGroup;

  fn callback_; // 用户回调
  void* arg;    // 用户参数


  routine_id tid_;
  Context* context_;

};




#endif //ROUTINE_TASK_META_H
