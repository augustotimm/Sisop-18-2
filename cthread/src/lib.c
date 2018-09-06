
#include <stdio.h>
#include <string.h>
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#define SUCESS 0
#define ERROR -1


int ccreate (void* (*start)(void*), void *arg, int prio) {
	return -1;
}

int csetprio(int tid, int prio) {
	return -1;
}

int cyield(void) {
	return -1;
}

int cjoin(int tid) {
	return -1;
}

int csem_init(csem_t *sem, int count) {
	return -1;
}

int cwait(csem_t *sem) {
	return -1;
}

int csignal(csem_t *sem) {
	return -1;
}

int cidentify (char *name, int size) {
    char *names =
        "Augusto Timm do Espirito Santo  - 00113887 "
        "Vinicius Roratto Carvalho  - 00160094";
	if (strncpy (name, names, size))
            return SUCESS;
    else
            return ERROR;
}


