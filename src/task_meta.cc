//
// Created by swagger on 2022/6/19.
//

#include "task_meta.h"

TaskMeta::TaskMeta()
: callback_(nullptr)
, arg(nullptr)
, context_(nullptr)
{}

bool TaskMeta::get_stack(int s, void (*fn)(void*)) {
  if ( s == 0 ) {
    context_ = get_main_stack();
    return context_ != nullptr;
  }
  context_ = ::get_stack(fn);
  return context_ != nullptr;
}

