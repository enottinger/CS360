#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <queue>
#include <iostream>
#include <semaphore.h>

#define NUM_THREADS  = 5;

// need -pthread passed to compliler

sem_t empty, full, mutex;

class myqueue{
	std::queue <int> stlqueue;
	public:
	void push(int sock){
		sem_wait(&empty);
		sem_wait(&mutex);
		stlqueue.push(sock);
		sem_post(&mutex);
		sem_post(&full);
	}
	int pop(){
		sem_wait(&full);
		sem_wait(&mutex);
		int rval = stlqueue.front();
		stlqueue.pop();
		sem_post(&mutex);
		sem_post(&empty);
		return(rval);
	}


}sockqueue;

void *howdy(void *arg)
{
	//for (;;) {
	//sock = dequeue();
	//Read Request
	//write response
	//close
	//}
	long tid;
	tid = (long) arg;
	//printf("Hi %d\n", tid);
		std::cout << "GOT " <<sockqueue.pop()<<std::endl;
	
}



main(){
#define NTHREADS 10
#define NQUEUE 20
	long threadid;
	pthread_t threads[NTHREADS];
	sem_init(&mutex, PTHREAD_PROCESS_PRIVATE, 1);
	sem_init(&full, PTHREAD_PROCESS_PRIVATE, 0);
	sem_init(&empty, PTHREAD_PROCESS_PRIVATE, NQUEUE);
	for(int i = 0; i < NQUEUE; i++) {
		sockqueue.push(i);
	}
	//for(int i = 0; i < NQUEUE; i++) {
	//	std::cout << "GOT " <<sockqueue.pop()<<std::endl;
	//}
	//exit(0);
	for(threadid = 0; threadid < NTHREADS; threadid++)
	{
		pthread_create(&threads[threadid], NULL, 
			howdy, (void *) threadid);
	}
	//Set up socket, bind, listen
	//for(;;){
	//	fd = accept
	//	enqueue(fd);
	//}
	pthread_exit(NULL);
}

//Need to create a new wrapper class that contains a queue
