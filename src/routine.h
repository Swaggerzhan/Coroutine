//
// Created by swagger on 2022/6/16.
//

#ifndef ROUTINE_ROUTINE_H
#define ROUTINE_ROUTINE_H


bool init();

void routine_start_background(void (*fn)(void*), void* arg);

void yield();






#endif //ROUTINE_ROUTINE_H
