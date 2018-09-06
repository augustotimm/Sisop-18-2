
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "../include/escalonador.h"
#define NCORES 1;
csem_t cpuSem;

//Inicia semaforo com 3 prioridades, 1 2 3 e suas respectivas filas.
int initCPUSem(){
    int status =0;
    PFILA2 fila1 = (PFILA2) malloc(sizeof(PFILA2));
    PFILA2 fila2 = (PFILA2) malloc(sizeof(PFILA2));
    PFILA2 fila3 = (PFILA2) malloc(sizeof(PFILA2));
    cpuSem.fila = (PFILA2) malloc(sizeof(PFILA2));
    cpuSem.count =NCORES;

    status += CreateFila2(cpuSem.fila);

    status += CreateFila2(fila1);

    status += CreateFila2(fila2);

    status += CreateFila2(fila3);
    int *prio1 = (int*) malloc(sizeof(int));
    int *prio2 = (int*) malloc(sizeof(int));
    int *prio3 = (int*) malloc(sizeof(int));
    *prio1 = 1;
    *prio2 = 2;
    *prio3 = 3;
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


}

void printCpuSem(){

    //printf("primeiro print");
    int finished = 0;
    PFILA2  filaprio;

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
            NextFila2(cpuSem.fila);
        }




    }

}
//Apenas para testes, necessário remover no futuro
int main(){

    initCPUSem();
    printCpuSem();

}
