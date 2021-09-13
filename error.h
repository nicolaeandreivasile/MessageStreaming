#ifndef _ERROR_H
#define _ERROR_H 1

#include <stdio.h>
#include <stdlib.h>

#define DIE(assertion, description)				\
    do {							\
        if (assertion) {					\
            fprintf(stderr, "[%s, %d]: ", __FILE__, __LINE__);	\
            perror(description);				\
            exit(EXIT_FAILURE);					\
        }							\
    } while(0)				

#define USAGE(assertion, description)	\
    do {				\
        if (assertion) {		\
            perror(description);	\
            exit(EXIT_FAILURE);		\
        }				\
    } while(0)

#define INITFAIL(assertion, description)				\
    do {								\
        if (assertion) {						\
            fprintf(stdout, "Failed %s initialization", description);	\
            return NULL;						\
        }								\
    } while (0)

#endif
