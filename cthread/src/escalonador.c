
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/escalonador.h"
#include "../include/cdata.h"

#define NCORES 1;

#define PRIO1 0;
#define PRIO2 1;
#define PRIO3 2;

int tid = 1;
static TCB_t* executing = NULL;
static csem_t cpuSem; //Semaforo para o estado de aptos
PFILA2 joins=NULL;

ucontext_t* ending_ctx=NULL;


int initmain();
int unjoin(int tid);

typedef struct s_join{
	TCB_t* thread;	/* indica se recurso está ocupado ou não (livre > 0, ocupado = 0) */
	int tid_j; 	/* ponteiro para uma fila de threads bloqueadas no semáforo */
} joint_t;





//Inicia semaforo com 3 prioridades, 1 2 3 e suas respectivas filas.
int initEscalonador(){
    if(executing != NULL){
        return 1;
    }
    initEndingCTX();
    joins =(PFILA2) malloc(sizeof(PFILA2));
    CreateFila2(joins);

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

    initmain();

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
                        NextFila2(cpuSem.fila);
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
    return 1;

}

void endThread(){
    if(executing == NULL){
        return;
    }


    unjoin(executing->tid);

    finishThread();
    dispatch();

}

int finishThread(){
    executing->state = PROCST_TERMINO;

    free(executing);
    executing = NULL;
    cpuSem.count +=1;
    return 1;

}

TCB_t* findFirstContext(int oldTID){
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

                    if(node->state == PROCST_APTO && oldTID != node->tid){ //Processo precisa estar apto para ser executado
                        return node; //pois será o processo de menor prioridade e o primeiro a ter entrado
                    }
                    else{
                         if (NextFila2(filaprio) !=0 ){ //Se o proximo nao existe, sai da fila
                             finished2 =1;
                             if( NextFila2(cpuSem.fila) !=0)//se nao achou processo vai para a processa fila de prioridades
                                return NULL;
                         }
                    }
                }
            }
            else{
                if( NextFila2(cpuSem.fila) !=0)//se nao achou processo vai para a processa fila de prioridades
                    return NULL;
            }

        }


    }
    return NULL;

}

int dispatch(){
    int swap =0;
    TCB_t* thread = NULL;




    if(executing != NULL){
        getcontext(&(executing->context));
        thread =findFirstContext(executing->tid);   //Caso seja um yield
    }
    else{
        thread =findFirstContext(-1);
    }
    if(thread == NULL){
        return -2; //Nao existe mais thread para ser despachada
    }
    if( swap==0){
        swap =1;
        executing = NULL; // to achando arriscado colocar essa tribuicao no dispatch, mas nao vi outro lugar para colocar
        return resumeThread(thread);

    }
    return -1;


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
            dispatch();
            return 2;
        }
        else{
            return 1;
        }
    }

}


int yieldThread(){


    executing->state = PROCST_APTO;//Atualiza o estado
    insertContextAtPrio(executing,executing->prio); //insere de volta para o semaforo de aptos
    getcontext(&(executing->context));
    //executing = NULL; //Remove a thread da execucao
    cpuSem.count +=1; //Aumenta a quantidade de recursos disponiveis no semaforo

    return 1;
}



int resumeThread(TCB_t* thread){
    if(thread->state== PROCST_APTO){
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


TCB_t *findAptoTID(int tid){
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

                    if(node->tid== tid){ //Verifica se o tid ẽ igual
                        return node; //Se sim retorna o nodo
                    }
                    else{
                         if (NextFila2(filaprio) !=0 ){ //Se o proximo nao existe, sai da fila
                             finished2 =1;
                             if( NextFila2(cpuSem.fila) !=0)//se nao achou processo vai para a processa fila de prioridades
                                return NULL;
                         }
                    }
                }
            }
            else{
                if( NextFila2(cpuSem.fila) !=0)//se nao achou processo vai para a processa fila de prioridades
                    return NULL;
            }

        }


    }
    return NULL;


}

// Busca por um ID em uma fila e retorna o ponteiro para o nodo.
/*
TCB_t* searchID (PFILA2 pfila, int tid){
    PFILA2  filaprio;
    TCB_t* node;
    FirstFila2(pfila); // posiciona interador no início da fila.
    node = GetAtIteratorFila2(pfila); // recebe o nodo.
    if(node->tid==tid)
        return node;

    while(NextFila2(PFILA2 pfila)){ //próxima fila.
        node = GetAtIteratorFila2(pfila);
        if(node->tid==tid)
            return node;
    }
    return NULL;
}
*/




TCB_t* getExecuting(){
    return executing;
}

int initmain(){
    TCB_t * newThread = (TCB_t *) malloc(sizeof(TCB_t));
    newThread->tid = 0; //0 tem que ser substituido por uma funçao que retorne o tid
    newThread->state = PROCST_EXEC; //0 tem que ser substituido por uma funcao do escalonador que va verificar em qual estado deve entrar
    newThread->prio= PRIO3; //prioridade mais baixa
     if( ( newThread->context.uc_stack.ss_sp = malloc(SIGSTKSZ) )  == NULL){
        return -1;
    }
    newThread->context.uc_stack.ss_size = SIGSTKSZ;
    newThread->context.uc_link = NULL;
    cpuSem.count -= 1;
    executing = newThread;

    return 1;

}


int joinThread(int tid){
    joint_t* njoin = (joint_t*) malloc(sizeof(joint_t) );
    njoin->thread =executing;
    njoin->tid_j = tid;
    AppendFila2(joins, njoin);
    executing->state = PROCST_BLOQ;
    deleteThreadCpuSem(executing);
    cpuSem.count +=1;
    return dispatch();


}

/***
Retorna -1 Se nao estã na fila de joins
***/
int isJoined(int tid){
    if(FirstFila2(joins) == 0){
        joint_t* node;
        do{
            node = GetAtIteratorFila2(joins);
            if(node == NULL){
                return 0;//Iterador invalido
            }
            if(node->tid_j == tid){
                return 1;
            }
        }while(NextFila2(joins) ==0);
    }
    return -1;
}

TCB_t* findJoinThread(int tid){
    int isj = isJoined(tid);
    if(isj != 1){
        return NULL;
    }
    if(FirstFila2(joins) == 0){
        joint_t* node;
        do{
            node = GetAtAntIteratorFila2(joins);
            if(node == NULL){
                return 0;//Iterador invalido
            }
            if(node->thread->tid == tid){
                return node->thread;
            }
        }while(NextFila2(joins) ==0);
    }
    return NULL;

}


TCB_t* getThreadJoin(int tid){
    int isj = isJoined(tid);
    if(isj != 1){
        return NULL;
    }
    if(FirstFila2(joins) == 0){
        joint_t* node;
        do{
            node = GetAtIteratorFila2(joins);
            if(node == NULL){
                return 0;//Iterador invalido
            }
            if(node->tid_j == tid){
                return node->thread;
            }
        }while(NextFila2(joins) ==0);
    }
    return NULL;

}

int unjoin(int tid){
    int isj = isJoined(tid);
    TCB_t *thread;
    if(isj != 1){
        return -1;
    }
    thread = getThreadJoin(tid);
    if(thread == NULL){
        return -1;
    }
    thread->state = PROCST_APTO;
    return insertContextAtPrio(thread,thread->prio);

}



/******
Apenas funcoes de exemplo para testes, remover antes da entrega
******/
/*
void threadTeste2(){
    printf("\n**********ThreadTeste 2 executando*****************");
    printCpuSem();
}

void threadTeste3(){
    printf("\n**********ThreadTeste 3 executando*****************");
    printCpuSem();
    cyield();
    printf("\n**********ThreadTeste 3 executando*****************");
    printf("\n---------------\n");
    printCpuSem();
}
void threadTeste(){
    printf("\n**********ThreadTeste executando*****************");

    ccreate((void*) (*threadTeste2),NULL,2);


    ccreate((void*) (*threadTeste3),NULL,2);
    printf("\n---------------ThreadTeste executando---------------\n");
    printCpuSem();
    cjoin(2);
    printf("\n---------------ThreadTeste executando---------------\n");
    printCpuSem();

    cyield();
    printf("\n**********ThreadTeste executando*****************");

    printf("\n---------------\n");
    printCpuSem();
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
    cyield();
    printf("\n--------------- MAIN ---------------\n");
    printCpuSem();



    //createFilaPrioridade(5);



}
*/

