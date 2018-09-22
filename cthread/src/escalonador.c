
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/escalonador.h"
#include "../include/cdata.h"

#define NCORES 1;

#define PRIO1 1;
#define PRIO2 2;
#define PRIO3 3;

int tid = 0;
static TCB_t* executing = NULL;
static csem_t cpuSem; //Semaforo para o estado de aptos
static csem_t bloqSem;

ucontext_t* ending_ctx;

//Inicia semaforo com 3 prioridades, 1 2 3 e suas respectivas filas.
int initEscalonador(){
    initEndingCTX();

    int status =0;
    PFILA2 fila1 = (PFILA2) malloc(sizeof(PFILA2));
    PFILA2 fila2 = (PFILA2) malloc(sizeof(PFILA2));
    PFILA2 fila3 = (PFILA2) malloc(sizeof(PFILA2));
    cpuSem.fila = (PFILA2) malloc(sizeof(PFILA2));
    cpuSem.count = 1;

    status += CreateFila2(cpuSem.fila);

    status += CreateFila2(fila1);

    status += CreateFila2(fila2);

    status += CreateFila2(fila3);

    int *prio1 = (int*) malloc(sizeof(int));
    int *prio2 = (int*) malloc(sizeof(int));
    int *prio3 = (int*) malloc(sizeof(int));
    *prio1 = PRIO1;
    *prio2 = PRIO2;
    *prio3 = PRIO3;
    AppendFila2(fila1, prio1);
    AppendFila2(fila2, prio2);
    AppendFila2(fila3, prio3);

    //int prio = (int) cpuSem.fila->first->node;

    status += AppendFila2(cpuSem.fila, fila1);
    status += AppendFila2(cpuSem.fila, fila2);
    status += AppendFila2(cpuSem.fila, fila3);

    return status;


}

//funcao para criar uma fila para uma prioridade especĩfica
int createFilaPrioridade(int prio){
    int finished = 0;
    PFILA2  filaAtual;
    PFILA2 filanew = (PFILA2) malloc(sizeof(PFILA2));
    int *prionew = (int*) malloc(sizeof(int));
    *prionew = prio;
    AppendFila2(filanew,prionew);


    FirstFila2(cpuSem.fila);
    while(finished == 0){

        //Pega a fila de prioridades do iterador da fila
        filaAtual = (PFILA2)GetAtIteratorFila2(cpuSem.fila);

        if(filaAtual==NULL){
            AppendFila2(cpuSem.fila,filanew);
            return 1;
            finished=1; //Criar Fila no FIM
        }
        else{
            int prioAtual =  *(int*)filaAtual->first->node; //primeiro nodo dessa fila sempre é um inteiro com o valor da prioridade.
            if(prio == prioAtual){
                return 0;
            }
            if(prio < prioAtual){
                InsertBeforeIteratorFila2(cpuSem.fila,filanew);
                return 1;

            }

            NextFila2(cpuSem.fila);
        }




    }
    return -1;

}
// Imprime todas as filas de prioridades junto com suas threads
void printCpuSem(){

    //printf("primeiro print");
    int finished = 0;
    PFILA2  filaprio;
    TCB_t* node;


    FirstFila2(cpuSem.fila);
    while(finished == 0){

        //Pega a fila de prioridades do iterador da fila
        filaprio = (PFILA2)GetAtIteratorFila2(cpuSem.fila);

        if(filaprio==NULL){
            finished=1;
        }
        else{
            int prio =  *(int*)filaprio->first->node; //primeiro nodo dessa fila sempre é um inteiro com o valor da prioridade.

            printf("\n prioridade:%d",prio);
            int finished2 =0;
            FirstFila2(filaprio);//Coloca o Iterador de novo no inicio da fila da prioridade
            NextFila2(filaprio);//ignora o primeiro valor que é a prioridade
            while(finished2 == 0){
                node = (TCB_t*)GetAtIteratorFila2(filaprio);
                if(node == NULL){
                    finished2 =1;
                }
                else{
                    printf("\n\tNode1\n\ttid:%d",node->tid);
                    printf("\n\tstate:%d",node->state);
                    printf("\n\tprio:%d",node->prio);
                    NextFila2(filaprio);
                }
            }

            NextFila2(cpuSem.fila);
        }




    }

}

/**********************
Insere a variável de contexto na fila de prioridade de valor prio.
Retorna 1 se conseguiu inserir corretamente.
Retorna -1 se não consegui inserir corretamente.
***********************/
int insertContextAtPrio(TCB_t *context, int prio){
    int finished = 0;
    PFILA2  filaAtual;





    FirstFila2(cpuSem.fila);
    while(finished == 0){

        //Pega a fila de prioridades do iterador da fila
        filaAtual = (PFILA2)GetAtIteratorFila2(cpuSem.fila);

        if(filaAtual==NULL){
            //AppendFila2(cpuSem.fila,filanew);
            return -1;
        }
        else{
            int prioAtual =  *(int*)filaAtual->first->node; //primeiro nodo dessa fila sempre é um inteiro com o valor da prioridade.
            if(prio == prioAtual){
                AppendFila2(filaAtual,context);
                return 1;
            }
            if(prio < prioAtual){
                return -1;

            }

            NextFila2(cpuSem.fila);
        }


    }
    return -1;

}



/**********************
Remove a variável de contexto na fila de prioridade.
Retorna 1 se conseguiu remover corretamente.
Retorna -1 se não consegui remover corretamente.
***********************/
int deleteThreadCpuSem(TCB_t *thread){
    //printf("primeiro print");
    int finished = 0;
    PFILA2  filaprio;
    TCB_t* node;


    FirstFila2(cpuSem.fila);
    while(finished == 0){

        //Pega a fila de prioridades do iterador da fila
        filaprio = (PFILA2)GetAtIteratorFila2(cpuSem.fila);

        if(filaprio==NULL){
            finished=-1;
        }
        else{
            int prio =  *(int*)filaprio->first->node; //primeiro nodo dessa fila sempre é um inteiro com o valor da prioridade.

            if(prio == thread->prio){ //Achou a fila de prioridade correta
                int finished2 =0;
                FirstFila2(filaprio);//Coloca o Iterador de novo no inicio da fila da prioridade
                NextFila2(filaprio);//ignora o primeiro valor que é a prioridade
                while(finished2 == 0){
                    node = (TCB_t*)GetAtIteratorFila2(filaprio);
                    if(node == NULL){
                        finished2 =-1;
                    }
                    else{

                        if(thread->tid == node->tid){
                            DeleteAtIteratorFila2(filaprio);
                            return 1;
                        }
                        NextFila2(filaprio);
                    }
                }

            }
            else{ //nao achou, entao continua procurando
                NextFila2(cpuSem.fila);
            }



        }




    }
    return -1;

}

int getNewTid(){
    return tid++;
}

int existeFilaPrio(int prio){
    int finished = 0;
    PFILA2  filaAtual;





    FirstFila2(cpuSem.fila);
    while(finished == 0){

        //Pega a fila de prioridades do iterador da fila
        filaAtual = (PFILA2)GetAtIteratorFila2(cpuSem.fila);

        if(filaAtual==NULL){
            //AppendFila2(cpuSem.fila,filanew);
            return -1;
        }
        else{
            int prioAtual =  *(int*)filaAtual->first->node; //primeiro nodo dessa fila sempre é um inteiro com o valor da prioridade.
            if(prio == prioAtual){

                return 1;
            }
            if(prio < prioAtual){
                return -1;

            }

            NextFila2(cpuSem.fila);
        }


    }
    return -1;

}

ucontext_t* getEndingCtx(){
    return ending_ctx;
}
int initEndingCTX(){
    ending_ctx = malloc(sizeof(ucontext_t));

    if(getcontext(ending_ctx)!=0 || ending_ctx == NULL){
        return -1;
    }

    ending_ctx->uc_stack.ss_sp =malloc(SIGSTKSZ);
    ending_ctx->uc_stack.ss_size = SIGSTKSZ;
    ending_ctx->uc_link = NULL;

    makecontext(ending_ctx,endThread,0);

}

void endThread(){
    if(executing == NULL){
        return;
    }


    finishThread();
    dispatch();

}

int finishThread(){
    executing->state = PROCST_TERMINO;

    free(executing);
    executing = NULL;
    return 1;

}

TCB_t* findFirstContext(){
   //printf("primeiro print");
    int finished = 0;
    PFILA2  filaprio;
    TCB_t* node;


    FirstFila2(cpuSem.fila);
    while(finished == 0){

        //Pega a fila de prioridades do iterador da fila
        filaprio = (PFILA2)GetAtIteratorFila2(cpuSem.fila);

        if(filaprio==NULL){
            return NULL;    //Se nao existem mais filas de prioridades, retorna null pois nao existem mais processos
        }
        else{

            int finished2 =0;
            FirstFila2(filaprio);//Coloca o Iterador de novo no inicio da fila da prioridade
            if (NextFila2(filaprio) ==0){//ignora o primeiro valor que é a prioridade
                while(finished2 == 0){
                    node = (TCB_t*)GetAtIteratorFila2(filaprio); //Retorna o primeiro processo que achar

                    if(node->state == PROCST_APTO){ //Processo precisa estar apto para ser executado
                        return node; //pois será o processo de menor prioridade e o primeiro a ter entrado
                    }
                    else{
                         if (NextFila2(filaprio) !=0 ){ //Se o proximo nao existe, sai da fila
                             finished2 =1;
                         }
                    }

                }



            }
            else{
                NextFila2(cpuSem.fila);//se nao achou processo vai para a processa fila de prioridades
            }

        }


    }
    return NULL;

}

int dispatch(){
    int swap =0;
    TCB_t* thread;
    thread =findFirstContext();
    if(thread == NULL){
        return -1; //Nao existe mais thread para ser despachada
    }


    if(executing != NULL){
        getcontext(&(executing->context));

    }
    if( swap==0){
        swap =1;

    }
    return resumeThread(thread);

}

//Verifica se haverã preempcão por prioridade
//Retorna o numero do estado que entrou
int estadoEntrada(TCB_t* nthread){
    //Nao há ninguem executando
    nthread->state =1;
    if( cpuSem.count  == 1){
        resumeThread(nthread);
        return 2;
    }
    else{ //existe alguém executando
        //Se a prioridade da thread nova é menor, passa a ser executada;
        if(executing->prio > nthread->prio){
            yieldThread();
            resumeThread(nthread);
            return 2;
        }
        else{
            return 1;
        }
    }

}


int yieldThread(){


    executing->state = PROCST_APTO_SUS;//Atualiza o estado
    insertContextAtPrio(executing,executing->prio); //insere de volta para o semaforo de aptos
    getcontext(&(executing->context));
    executing = NULL; //Remove a thread da execucao
    cpuSem.count +=1; //Aumenta a quantidade de recursos disponiveis no semaforo

    return 1;
}



int resumeThread(TCB_t* thread){
    if(thread->state==1){
        //Caso seja computador com mais de um cor necessário alterar a comparacao com executing
        if(executing == NULL && cpuSem.count>0){
            executing = thread; //passa a thread para a executando
            thread->state=2;//atualiza o estado
            cpuSem.count-=1;//atualiza os recursos disponiveis
            deleteThreadCpuSem(thread);//remove do estado de aptos
            setcontext( &(thread->context) );
            return 0;
        }
        else{
            return -1;
        }
    }
    else{
        return -1;
    }
    return -1;
}
//Apenas funcoes de exemplo para testes, remover antes da entrega

TCB_t* getExecuting(){
    return executing;
}
void threadTeste2(){
    printf("\n**********ThreadTeste 2 executando*****************");
    printCpuSem();
}
void threadTeste(){
    printf("\n**********ThreadTeste executando*****************");
    ccreate((void*) (*threadTeste2),NULL,1);

    cyield();
}


//Apenas para testes, necessário remover no futuro
int main(){

    initEscalonador();
    printf("\n---------------\n");
    printCpuSem();
    ccreate((void*) (*threadTeste),NULL,2);
    ccreate((void*) (*threadTeste2),NULL,1);
    printf("\n---------------\n");
    printCpuSem();
    yieldThread();
    printf("\n---------------\n");
    printCpuSem();



    //createFilaPrioridade(5);
    /*
    TCB_t * newThread = (TCB_t *) malloc(sizeof(TCB_t));
    newThread->tid =0; //0 tem que ser substituido por uma funçao que retorne o tid
    newThread->state =0; //0 tem que ser substituido por uma funcao do escalonador que va verificar em qual estado deve entrar
    newThread->prio= 2;
    getcontext(&(newThread->context));
    insertContextAtPrio(newThread,newThread->prio);
    printf("\n\n");
    printCpuSem();
    deleteThreadCpuSem(newThread);
    printf("\n\n");
    printCpuSem();
    */

}


