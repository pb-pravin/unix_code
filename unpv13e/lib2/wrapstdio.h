#ifndef	__unp_wrapstdio_h
#define	__unp_wrapstdio_h

#include "error.h"
#include <stdio.h>

void	 Fclose(FILE *);
FILE	*Fdopen(int, const char *);
char	*Fgets(char *, int, FILE *);
FILE	*Fopen(const char *, const char *);
void	 Fputs(const char *, FILE *);

#endif
