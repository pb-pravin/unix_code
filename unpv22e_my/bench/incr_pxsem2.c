/* include main */
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define	MAXNTHREADS	100
#define	NAME	"/incr_pxsem2"
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

inline int min(int a, int b)
{
	return (a < b) ? a : b;
}

int		nloop;

struct {
  sem_t	*mutex;		/* the memory-based semaphore */
  long	counter;
} shared;

void	*incr(void *);

int
start_time(void);

double
stop_time(void);

int
main(int argc, char **argv)
{
	int		i, nthreads;
	pthread_t	tid[MAXNTHREADS];
	int err;
	double st;	// stop time

	if (argc != 3) {
		fprintf(stderr, "usage: incr_pxsem2 <#loops> <#threads>\n");
		exit(1);
	}
	nloop = atoi(argv[1]);
	nthreads = min(atoi(argv[2]), MAXNTHREADS);

		/* 4initialize named semaphore to 0 */
	sem_unlink(NAME);		/* error OK */
	if ((shared.mutex = sem_open(NAME, O_CREAT | O_EXCL, FILE_MODE, 0)) == SEM_FAILED) {
		fprintf(stderr, "sem_open error for %s: %s\n", NAME, strerror(errno));
		exit(1);
	}

		/* 4create all the threads */
	if ((err = pthread_setconcurrency(nthreads)) != 0) {
		fprintf(stderr, "pthread_setconcurrency error: %s\n", strerror(err));
		exit(1);
	}
	for (i = 0; i < nthreads; i++) {
		if ((err = pthread_create(&tid[i], NULL, incr, NULL)) != 0) {
			fprintf(stderr, "pthread_create error: %s\n", strerror(err));
			exit(1);
		}
	}
		/* 4start the timer and release the semaphore */
	if (start_time() == -1) {
		perror("start_time error");
		exit(1);
	}
	if (sem_post(shared.mutex) == -1) {
		perror("sem_post error");
		exit(1);
	}

		/* 4wait for all the threads */
	for (i = 0; i < nthreads; i++) {
		if ((err = pthread_join(tid[i], NULL)) != 0) {
			fprintf(stderr, "pthread_join error: %s\n", strerror(err));
			exit(1);
		}
	}
	if ((st = stop_time()) == 0.0) {
		perror("stop_time error");
		exit(1);
	}
	printf("microseconds: %.0f usec\n", st);
	if (shared.counter != nloop * nthreads)
		printf("error: counter = %ld\n", shared.counter);

	if (sem_unlink(NAME) == -1) {
		perror("sem_unlink error");
		exit(1);
	}

	exit(0);
}
/* end main */

/* include incr */
void *
incr(void *arg)
{
	int		i;

	for (i = 0; i < nloop; i++) {
		if (sem_wait(shared.mutex) == -1) {
			perror("sem_wait error");
			exit(1);
		}
		shared.counter++;
		if (sem_post(shared.mutex) == -1) {
			perror("sem_post error");
			exit(1);
		}
	}
	return(NULL);
}
/* end incr */
