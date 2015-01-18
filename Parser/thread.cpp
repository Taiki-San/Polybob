/*
 *		Code écrit dans le cadre du projet Rakshata
 *		L'équipe de développement du projet met ce code sous licence BSD 3-clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <utime.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/file.h>
#include <sys/wait.h>

#define THREAD_TYPE pthread_t

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct thread_info
{
	pthread_t thread_id;
	int       thread_num;
	char     *argv_string;
};


void createNewThread(void *function, void *arg)
{
	pthread_t thread;
	
	if (pthread_create(&thread, NULL, (void *(*)(void *)) function, arg))
	{
		printf("Failed at create thread\n");
		exit(EXIT_FAILURE);
	}
}
