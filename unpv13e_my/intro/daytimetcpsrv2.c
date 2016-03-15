#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* Define bzero() as a macro if it's not in standard C library. */
#ifndef HAVE_BZERO
#define bzero(ptr,n)        memset(ptr, 0, n)
/* $$.If bzero$$ */
/* $$.If memset$$ */
#endif

/* Following shortens all the typecasts of pointer arguments: */
#define SA  struct sockaddr

#define MAXLINE     4096    /* max text line length */

/* Following could be derived from SOMAXCONN in <sys/socket.h>, but many
 *    kernels still #define it as 5, while actually supporting many more */
#define LISTENQ     1024    /* 2nd argument to listen() */

int
main(int argc, char **argv)
{
	int					listenfd, connfd, i;
	struct sockaddr_in	servaddr;
	char				buff[MAXLINE];
	time_t				ticks;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket error");
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(9999);	/* daytime server */

	if (bind(listenfd, (SA *) &servaddr, sizeof(servaddr)) < 0) {
		perror("bind error");
		exit(1);
	}

	if (listen(listenfd, LISTENQ) < 0) {
		perror("listen error");
		exit(1);
	}

	for ( ; ; ) {
		if ((connfd = accept(listenfd, (SA *) NULL, NULL)) < 0) {
			perror("accept error");
			exit(1);
		}

        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
		for (i = 0; i < strlen(buff); i++) {
        	if (write(connfd, &buff[i], 1) != 1) {
				perror("write error");
				exit(1);
			}
		}

		if (close(connfd) == -1) {
			perror("close error");
			exit(1);
		}
	}
}
