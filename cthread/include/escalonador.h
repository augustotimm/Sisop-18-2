
#include"support.h"
#include"cthread.h"


int initCPUSem(int count);

int createFilaPrioridade(int prio);
void printCpuSem();

int insertContextAtPrio(TCB_t *context, int prio);
