#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "../include/escalonador.h"
#define SUCESS 1
#define ERROR -1


int ccreate (void* (*start)(void*), void *arg, int prio) {

    initEscalonador();

    TCB_t * newThread = (TCB_t *) malloc(sizeof(TCB_t));
    newThread->tid = getNewTid(); //0 tem que ser substituido por uma funçao que retorne o tid
    newThread->state =0; //0 tem que ser substituido por uma funcao do escalonador que va verificar em qual estado deve entrar
    newThread->prio= prio;

    getcontext(&(newThread->context) );
    if( ( newThread->context.uc_stack.ss_sp = malloc(SIGSTKSZ) )  == NULL){
        return -1;
    }
    newThread->context.uc_stack.ss_size = SIGSTKSZ;
    newThread->context.uc_link = getEndingCtx();

    makecontext(&(newThread->context),(void (*)(void)) start,1,arg); //COMO FAZER????
    int existe=existeFilaPrio(prio);

    if(existe != 1){ // Fila da prioridade correta ainda nao existe
        return ERROR;
    }
    //A fila já deve existir
    insertContextAtPrio(newThread,prio);
    estadoEntrada(newThread);


	return newThread->tid;
}

int csetprio(int tid, int prio) { // tid deve ficar sempre nulo.
    TCB_t * thread;

    if(prio < 1 ){
        return ERROR;
    }
    initEscalonador();


    // busca o ID na fila de aptos
    thread = findAptoTID(tid);
    if(thread == NULL)
        return ERROR;

    deleteThreadCpuSem(thread);
    thread->prio = prio;

    if(insertContextAtPrio(thread, thread->prio)){
        estadoEntrada(thread);
        return SUCESS;

    }
    else
        return ERROR;

}

int cyield(void) {

    TCB_t* curThread = getExecuting();
    yieldThread();
    //yieldThread();
    if( dispatch(curThread->tid) == -2){
        deleteThreadCpuSem(curThread);
        return 1; //Nao existia outra thread
    }
    return 0;//Existia outra thread
}

int cjoin(int tid) {

    TCB_t* thread = findAptoTID(tid);   //Verifica se a thread existe na fila de aptos
    if(thread ==NULL){
        thread = findJoinThread(tid);    //Verifica se a thread existe na fila de bloqueados por join
        if(thread == NULL){
            return -1;
        }

    }
    int isj = isJoined(tid);
    if(isj != -1){
        return -2;//Thread ja esta em join
    }
    return joinThread(tid);

}

int csem_init(csem_t *sem, int count) {
    sem->count = count;
    int success = CreateFila2(sem->fila);
	return success;
}

int cwait(csem_t *sem) {
    TCB_t *exec;
    initEscalonador();

        if ((sem == NULL) || (sem->fila == NULL))
            return ERROR;

        if(sem->count > 0){
            sem->count--;
            return SUCESS;
        } else {
            exec = getExecuting();
            exec->state = PROCST_BLOQ;
            AppendFila2(sem->fila, exec);
            blockThread();
        }


	return SUCESS;
}

int csignal(csem_t *sem) {


    initEscalonador();
    if ((sem == NULL) || (sem->fila == NULL))
        return ERROR;


    TCB_t *thread = NULL;
    if (FirstFila2(sem->fila) == 0) {
        thread  = (TCB_t *)GetAtIteratorFila2(sem->fila);
        DeleteAtIteratorFila2(sem->fila);
    }
    else{
        sem->count++;
    }
    if (thread != NULL) {
        thread->state = PROCST_APTO;
        insertContextAtPrio(thread,thread->prio);
        return estadoEntrada(thread);


    }
    else
    {
        return SUCESS;
    }

}




int cidentify (char *name, int size) {
    char *names =
        "Augusto Timm do Espirito Santo  - 00113887 "
        "Vinicius Roratto Carvalho  - 00160094";
	if (strncpy (name, names, size)){
        return SUCESS;
	}

    return ERROR;
}

