#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int outsider = 0;

void * testFunction (void * arg) {
	static int counter = 0;

	int threadFunctionId = *((int *) arg);

	sleep(1);

	printf("Thread's id: %d. Local: %d. Outsider: %d\n", threadFunctionId, counter, outsider);

	counter++;
	outsider++;

	return NULL;
}


int main () {
	pthread_t threadId;

	for (int  i = 0; i < 10; i++) {
		pthread_create(&threadId, NULL, testFunction, (void *) &threadId);
	}

	pthread_exit(NULL);

	return 0;
}
