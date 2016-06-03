#include "semaphore.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>

typedef pthread_t thread_id;


  // Create a queue to hold the threads waiting.
  SIMPLEQ_HEAD(queuehead, entry) head = SIMPLEQ_HEAD_INITIALIZER(head);
  
  // Queue entry struct
  struct semaphore_entry {
    int sem_id; // id of the semaphore
    SIMPLEQ_ENTRY(entry) next; // pointer to the next entry in queue.
  } *new_entry; 

// internalized count of semaphores.
// Used to track semaphore signaling from queue.
int semaphoreCount = 0;

// Arbitrary counter to assign ids to semaphores.
int nextId() {
 ++semaphoreCount;
  return semaphoreCount;
}

// Semaphore creation
semaphore_t * createSemaphore( int initialCount ) {
  semaphore_t * toReturn = (semaphore_t*) malloc(sizeof(semaphore_t*));
  toReturn->value = initialCount;
  toReturn->id = nextId();
  return toReturn;
}

// Semaphore "deconstructor"
void destroySemaphore(semaphore_t * sem) {
  free(sem);
}

// Mutually exclusive decrement method for semaphore_t.
void down(semaphore_t * sem) {

  // acquire mutual exclusion
  pthread_mutex_lock(&sem->mutex_lock);

  // if semaphore cannot be down(),
  // add it to the queue so that it may be
  // signaled appropriately. 
  if( sem->value <= 0 ) {
    new_entry = (struct semaphore_entry *) malloc(sizeof(struct semaphore_entry));
    new_entry->t_id = pthread_self();
    new_entry->sem_id = sem->id;
    SIMPLEQ_INSERT_TAIL(&head, new_entry, next);
    pthread_cond_wait(&sem->wake_cond, &sem->mutex_lock);
  }

  // otherwise/after signal, just decrement the semaphore.
  --(sem->value);

  // release mutex
  pthread_mutex_unlock(&sem->mutex_lock);

} 


void up(semaphore_t * sem) {

  // acquire mutual exclusion
  pthread_mutex_lock(&sem->mutex_lock);

  // iterate through waiting semaphore entries, find first (longest waiting) that matches id of given semaphore id.
  for(new_entry = SIMPLEQ_FIRST(&head); new_entry != NULL; new_entry = SIMPLEQ_NEXT(new_entry, next)) {
    if(new_entry->sem_id == sem->id) { // if ids match, signal mutex associated with it.
      pthread_cond_signal(&sem->wake_cond);
      break;
    }

  }

  // increment semaphore value
  ++(sem->value);

  // release mutex
  pthread_mutex_unlock(&sem->mutex_lock);

}
