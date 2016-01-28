#include <pthread.h>
#include <stdio.h>
#define NUM_THREADS  = 5;

// need -pthread passed to compliler

void *howdy(void *arg)
{
	long tid;
	tid = (long) arg;
	printf("Hi %d\n", tid);
}

main(){
#define NTHREADS 20
	int threadid;
	pthread_t threads[NTHREADS];
	for(threadid = 0; threadid < NTHREADS; threadid++)
	{
		pthread_create(&threads[threadid], NULL, 
			howdy, (void *) threadid);
	}
	pthread_exit(NULL);
}
