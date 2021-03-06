/* semwait.c */
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(int argc, char **argv)
{
	sem_t *sem;
	int val;

	if (argc != 2) {
		fprintf(stderr, "usage: semwait <name>\n");
		exit(1);
	}

	sem = sem_open(argv[1], 0);
	if (sem == SEM_FAILED) {
		fprintf(stderr, "sem_open error for %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	if (sem_wait(sem) == -1) {
		perror("sem_wait error");
		exit(1);
	}

	if (sem_getvalue(sem, &val) == -1) {
		perror("sem_getvalue error");
		exit(1);
	}
	printf("pid %ld has semaphore, value = %d\n", (long) getpid(), val);

	pause();	/* blocks until killed */
	exit(0);
}
