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


	return -1;
}

int csetprio(int tid, int prio) { // tid deve ficar sempre nulo.
    if(existeFilaPrio(prio) != 1){ // Fila da prioridade correta ainda nao existe
        createFilaPrioridade(prio);
    }

    if(prio > 3 || prio < 0) // verifica se é prioridade válida.
        return ERROR;

    // verifica se o contexto


//
//    TCB_t *thread = NULL;
//    if ((thread = blocked_join_get_thread(tid)) == NULL) { //verificar se está bloqueada por JOIN
//        if ((thread = get_thread_from_blocked_semaphor(tid)) == NULL) {  //verificar se está bloqueada por semáforo
//            if ((thread = ready_get_thread(tid)) == NULL) { // verifica se está na fila de aptos.
//                if (running_thread->tid == tid) { // MUDAR, vai ser sempre null na versão 2018-2
//                    running_thread->ticket = prio;
//                    return SUCCESS_CODE;
//                }
//                else {
//                    DEBUG(("Thread a ser modificada não existe.\n"));
//                }
//                return ERROR_CODE;
//            }
//            else {
//                thread_in_ready = true;
//            }
//        }
//    }
//    // A thread existe e está apontada pelo ponteiro "thread".
//
//    if (thread_in_ready) {
//        // Caso a thread esteja na fila de aptos, temos que removê-la da fila
//        // com prioridade atual, e inseri-la na sua nova fila com a prioridade
//        // certa.
//        thread = ready_remove(thread->tid);
//        if (thread != NULL) {
//            thread->ticket = prio;  // Altera a prioridade da thread.
//            ready_push(thread);     // Coloca na fila certa.
//        }
//    }
//    else {
//        // Neste caso, a thread não está nos aptos, e podemos simplesmente
//        // alterar sua prioridade.
//        thread->ticket = prio;
//    }
//
//    return SUCCESS_CODE;
//}





	return -1;
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

// Dúvidas aqui.
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
            // dispatch(); // como chamar o escalonador?
        }


	return SUCESS;
}


int csignal(csem_t *sem) {

    if(existeFilaPrio(1) != 1) // Filas não existem/CPU não inicializada
        initCPUSem(1);
    if ((sem == NULL) || (sem->fila == NULL))
        return ERROR;

    sem->count++;

//    TCB_t *thread = (TCB_t *)get_first_of_semaphore_queue(sem);
//    if (thread != NULL) {
//        // Existia uma thread bloqueada pelo semáforo.
//        // Estado da thread e modificado para APTO
//        thread->state = PROCST_APTO;
//        return ready_push(thread);
//    }
//    else {
//        //O semáforo esta livre. Segue execucao.
//        return SUCCESS_CODE;
}

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

