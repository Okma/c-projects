#include "semaphore.c"

// number of waiting chairs available
#define CHAIRS 5

// number of barbers in the shop
#define NUM_BARBERS 2

// declare sempahores
semaphore_t * active_barbers;
semaphore_t * wait_customers;
semaphore_t * mutex;

// counter of how many chairs are currently occupied by customers
int chairsFull = 0;

// 2 barber threads
pthread_t barbers[2];

// arbitrary number of customer threads
pthread_t customers[25];

// dummy iteration integer
int i;

// simulate 'cutting hair' by sleeping for 10,000 uSeconds.
void cut_hair() {
  usleep(10000);
}

// simulate 'receiving a haircut' by sleeping for 10,000 uSeconds.
void receive_haircut() {
  usleep(10000);
}

// barber method of exeuction for pthread
void * barber(void * arg) {
  while( 1 ) {
    printf("Barber %d is going to sleep.\n", (long)arg);
    down(wait_customers);
    down(mutex);
    --chairsFull;
    up(active_barbers);
    printf("Barber %d has woken up.\n", (long)arg);
    printf("Barber %d calls a customer over.\n", (long)arg);
    up(mutex);
    printf("Barber %d is giving a haircut.\n", (long)arg);
    cut_hair();
  }
  pthread_exit(NULL);

}

// customer method of exeuction for pthread
void * customer(void * arg) {
  down(mutex);
  if(chairsFull < CHAIRS) { // if space is available.
    ++chairsFull; // increment chairs that are full
    printf("Customer %d has sat down in a waiting chair. [%d out of %d chairs full]\n", (long)arg, chairsFull, CHAIRS );
    up(wait_customers); // increment waiting customer count
    up(mutex);
    down(active_barbers);
    printf("Customer %d is receiving a haircut.\n", (long)arg);
    receive_haircut(); 
    printf("Customer %d has finished receiving a haircut.\n", (long) arg);
  } else { // no space avilable, release mutex. Customer leaves (thread terminates).
    printf("No chairs available! A customer has left.\n");
    up(mutex);
  }
  pthread_exit(NULL);
}

int main() {

// define the previously declared semaphores
mutex = createSemaphore(1);
active_barbers = createSemaphore(0);
wait_customers = createSemaphore(0);

// create barbers
 for(i = 0; i < NUM_BARBERS; ++i) {
   printf("Created barber %d.\n", i);
   pthread_create(&barbers[i], NULL, barber, (void*) i + 1);
 }

 /*
  * Extensive test-driven simulation
  */


 // First test: Two customers enter at same time.
 // Goal: To see both barbers cutting hair.
 sleep(2);
 printf("Customer 1 walks into the shop.\n");
 pthread_create(&customers[0], NULL, customer, (void*) 1);
 printf("Customer 2 walks into the shop.\n");
 pthread_create(&customers[1], NULL, customer, (void*) 2);

 // Second test: 6 customers enter at same time.
 // Goal: To see over-saturation of chairs (customer leaving).
 sleep(2);
 printf("Customer 3 walks into the shop.\n");
 pthread_create(&customers[0], NULL, customer,(void*)3);
 printf("Customer 4 walks into the shop.\n");
 pthread_create(&customers[1], NULL, customer,(void*) 4);
 printf("Customer 5 walks into the shop.\n");
 pthread_create(&customers[2], NULL, customer, (void*) 5);
 printf("Customer 6 walks into the shop.\n");
 pthread_create(&customers[3], NULL, customer, (void*) 6);
 printf("Customer 7 walks into the shop.\n");
 pthread_create(&customers[4], NULL, customer, (void*) 7);
 printf("Customer 8 walks into the shop.\n");
 pthread_create(&customers[5], NULL, customer, (void*) 8);

 // Third test: 2 customers enter, followed by 1, then 2 more.
 // Goal : To see prioritization of earlier arriving customers.
 sleep(2);
 printf("Customer 9 walks into the shop.\n");
 pthread_create(&customers[0], NULL, customer,(void*) 9);
 printf("Customer 10 walks into the shop.\n");
 pthread_create(&customers[1], NULL, customer,(void*) 10);
 usleep(500);
 printf("Customer 11 walks into the shop.\n");
 pthread_create(&customers[2], NULL, customer, (void*) 11);
 usleep(500);
 printf("Customer 12 walks into the shop.\n");
 pthread_create(&customers[3], NULL, customer, (void*) 12);
 printf("Customer 13 walks into the shop.\n");
 pthread_create(&customers[4], NULL, customer, (void*) 13);

 // join straggler customers
 for(i = 0; i < 25; ++i) {
   pthread_join(customers[i], NULL);
 }

// clean up semaphores
destroySemaphore(active_barbers);
destroySemaphore(wait_customers);
destroySemaphore(mutex);

 return 0;
}
