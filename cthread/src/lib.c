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
    if(existeFilaPrio(1) != 1){ // Fila da prioridade correta ainda nao existe
        initEscalonador();
    }
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
        createFilaPrioridade(prio);
    }
    //A fila já deve existir
    insertContextAtPrio(newThread,prio);
    estadoEntrada(newThread);


	return -1; // retormar -1 não indica erro?
}
// Não testada.
int csetprio(int tid, int prio) { // tid deve ficar sempre nulo.
    TCB_t * thread;
    if(existeFilaPrio(prio) != 1){ // Fila da prioridade correta ainda nao existe
        createFilaPrioridade(prio);
    }


    if(prio > 3 || prio < 0) // verifica se é prioridade válida.
        return ERROR;

    // busca o ID na fila de aptos
    thread = searchID(tid, cpuSem.fila);
    if(thread == NULL)
        return ERROR;

    DeleteAtIteratorFila2(cpuSem.fila);

    if(insertContextAtPrio(thread, thread->prio))
        return SUCESS;
    else
        return ERROR;

}

int cyield(void) {
//        init();
//    DEBUG(("cyield>\n Thread %d cedendo a execução voluntariamente.\n", running_thread->tid));
//
//    // Muda-se o estado da thread em execução para "apto", e ela é colocada de volta pra fila de aptos.
//    running_thread->state = PROCST_APTO;
//    ready_push(running_thread);
//
//    // O escalonador é acionado.
//return dispatch();
//
    TCB_t* curThread = getExecuting();
    curThread->state = PROCST_APTO_SUS;
    //yieldThread();
    dispatch();
    curThread->state = PROCST_APTO;
	return -1; //retornar negativo não é sinalizar erro?
}

int cjoin(int tid) {

//    int cjoin(int tid) {
//    init();
//    DEBUG(("Cjoin> join na thread %d pela thread %d\n", tid, running_thread->tid));
//
//    if (blocked_join_get_thread_waiting_for(tid) != NULL) {
//        DEBUG(("A thread ja esta sendo esperada.\n"));
//        return ERROR_CODE;
//    }
//
//    TCB_t *thread = NULL;
//    if ((thread = blocked_join_get_thread(tid)) == NULL) {
//        if ((thread = get_thread_from_blocked_semaphor(tid)) == NULL) {
//            if ((thread = ready_get_thread(tid)) == NULL) {
//                DEBUG(("Thread não existe.\n"));
//                // Thread não existe, retorna erro.
//                return ERROR_CODE;
//            }
//        }
//    }
//
//    DEBUG(("Thread existe e não é esperada.\n"));
//    // Thread que se deseja esperar o término existe, não é esperada e está
//    // apontada pelo ponteiro "thread".
//    DUPLA_t *new_cjoin = (DUPLA_t *) malloc(sizeof(DUPLA_t));
//    new_cjoin->waitedTid = tid;
//    new_cjoin->blockedThread = running_thread;
//    running_thread->state = PROCST_BLOQ;
//    blocked_join_insert(new_cjoin);
//
//#if SHOULD_DEBUG
//    debug_print_blocked_list();
//#endif
//
//    return dispatch();



	return -1;
}

int csem_init(csem_t *sem, int count) {
    sem->count = count;
    int success = CreateFila2(sem->fila);
	return success;
}

// não testada.
int cwait(csem_t *sem) {

        if(existeFilaPrio(1) != 1) // Filas não existem/CPU não inicializada
            initCPUSem(1);

        if ((sem == NULL) || (sem->fila == NULL))
            return ERROR;

        if(sem->count > 0){
            sem->count--;
            return SUCESS;
        } else {
            sem->count--;
            executing->state = PROCST_BLOQ;
            AppendFila2(sem, executing);
            dispatch();
        }


	return SUCESS;
}

// não testada.
int csignal(csem_t *sem) {

    if(existeFilaPrio(1) != 1) // Filas não existem/CPU não inicializada
        initCPUSem(1);
    if ((sem == NULL) || (sem->fila == NULL))
        return ERROR;

    sem->count++;

    TCB_t *thread = NULL;
    if (FirstFila2(sem->fila) == 0) {
        *thread  = (TCB_t *)GetAtIteratorFila2(sem->fila);
        DeleteAtIteratorFila2(sem->fila);
    if (thread != NULL) {
        thread->state = PROCST_APTO;
        return insertContextAtPrio(thread,prio);
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
	if (strncpy (name, names, size))
            return SUCESS;
    else
            return ERROR;
}

