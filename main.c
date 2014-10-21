/* main.c */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "buffer.h"
#include <iostream>
#include <time.h>


#define RAND_DIVISOR 5
#define TRUE 1

/* The mutex lock */
pthread_mutex_t mutex;

/* the semaphores */
sem_t full, empty;

/* the buffer */
buffer_item buffer[BUFFER_SIZE];

/* buffer counter */
int counter;

pthread_t tid;       //Thread ID
//pthread_attr_t attr; //Set of thread attributes


void initializeData() {

   /* Create the mutex lock */
   pthread_mutex_init(&mutex, NULL);

   /* Create the full semaphore and initialize to 0 */
   sem_init(&full, 0, 1);

   /* Create the empty semaphore and initialize to BUFFER_SIZE */
   sem_init(&empty, 0, BUFFER_SIZE);

   /* Get the default attributes */
   //pthread_attr_init(&attr);

   /* init buffer */
   counter = 0;
}


/* Add an item to the buffer */
int insert_item(buffer_item item) {
   /* When the buffer is not full add the item
      and increment the counter*/
   if(counter < BUFFER_SIZE) {
      buffer[counter] = item;
      counter++;
      return 0;
   }
   else { /* Error the buffer is full */
      return -1;
   }
}

/* Remove an item from the buffer */
int remove_item(buffer_item *item) {
   /* When the buffer is not empty remove the item
      and decrement the counter */
   if(counter > 0) {
      *item = buffer[(counter-1)];
      counter--;
      return 0;
   }
   else { /* Error buffer empty */
      return -1;
   }
}








/* Producer Thread */
void *producer(void *param) {
   buffer_item item;

   while(TRUE) {
      /* sleep for a random period of time */
      int rNum = rand()%RAND_DIVISOR +1;
      sleep(rNum);

      /* generate a random number */
      item = rand();

      /* acquire the empty lock */
      sem_wait(&empty);
      /* acquire the mutex lock */
      pthread_mutex_lock(&mutex);

      if(insert_item(item)) {
         fprintf(stderr, " Producer report error condition\n");
      }
      else {
         printf("producer produced %d\n", item);
      }
      /* release the mutex lock */
      pthread_mutex_unlock(&mutex);
      /* signal full */
      sem_post(&full);
   }
}

/* Consumer Thread */
void *consumer(void *param) {
   buffer_item item;

   while(TRUE) {
      /* sleep for a random period of time */
      int rNum = rand() % RAND_DIVISOR + 1;
      sleep(rNum);

      /* aquire the full lock */
      sem_wait(&full);
      /* aquire the mutex lock */
      pthread_mutex_lock(&mutex);
      if(remove_item(&item)) {
         fprintf(stderr, "Consumer report error condition\n");
      }
      else {
         printf("consumer consumed %d\n", item);
      }
      /* release the mutex lock */
      pthread_mutex_unlock(&mutex);
      /* signal empty */
      sem_post(&empty);
   }
}



int main(int argc, char *argv[]) {
   /* Loop counter */
   int i;

   /* Verifying the number of arguments passed */
   if(argc != 4) {
      fprintf(stderr, "Incorrect No. of Arguments \n Format:- ./main.out <INT> <INT> <INT>\n");
   }

   int sleepTimeMain = atoi(argv[1]); /* Time in seconds for main to sleep. atoi() is a typecast function FOR int*/
   int numProTs = atoi(argv[2]); /* Number of producer threads */
   int numConsTs = atoi(argv[3]); /* Number of consumer threads */

   /* Initialize the app */
   initializeData();

   /* Create the producer threads */
   for(i = 0; i < numProTs; i++) {
      /* Create the thread */
      pthread_create(&tid,NULL,producer,NULL);
    }

   /* Create the consumer threads */
   for(i = 0; i < numConsTs; i++) {
      /* Create the thread */
      pthread_create(&tid,NULL,consumer,NULL);
   }

   /* sleep for the specified amount of time in seconds */
   sleep(sleepTimeMain);
   /* 
   for(i = 0; i < numProTs; i++)
    {
      /* Create the thread 
     pthread_join(tid,NULL);
    }
    */

   /* Exit the program */
   printf("Exit the program\n");
   exit(0);
}