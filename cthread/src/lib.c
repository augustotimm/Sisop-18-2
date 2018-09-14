#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "../include/escalonador.h"
#define SUCESS 0
#define ERROR -1


int ccreate (void* (*start)(void*), void *arg, int prio) {
    if(existeFilaPrio(1) != 1){ // Fila da prioridade correta ainda nao existe
        initCPUSem(1);
    }
    TCB_t * newThread = (TCB_t *) malloc(sizeof(TCB_t));
    newThread->tid = getNewTid(); //0 tem que ser substituido por uma funçao que retorne o tid
    newThread->state =0; //0 tem que ser substituido por uma funcao do escalonador que va verificar em qual estado deve entrar
    newThread->prio= prio;
    makecontext(&(newThread->context),(void (*)(void) ) start,1,arg); //COMO FAZER????
    int existe=existeFilaPrio(prio);

    if(existe != 1){ // Fila da prioridade correta ainda nao existe
        createFilaPrioridade(prio);
    }
    //A fila já deve existir
    insertContextAtPrio(newThread,prio);
    estadoEntrada(newThread);


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
    sem->count = count;
    int success = CreateFila2(sem->fila);
	return success;
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

