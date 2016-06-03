#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <pthread.h>

typedef struct {
  int value; // quantitative value of semaphore
  int id; // identifier of semaphore
  pthread_mutex_t mutex_lock; // unique mutex lock for this semaphore
  pthread_cond_t wake_cond; // condition for waking up
} semaphore_t;

 semaphore_t * createSemaphore(int initialCount);
 void destroySemaphore(semaphore_t * sem);
 void down( semaphore_t * sem);
 void up( semaphore_t * sem );

#endif // _SEMAPHORE_H_
