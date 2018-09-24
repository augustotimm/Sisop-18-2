
#include "support.h"
#include "cthread.h"
#include "cdata.h"

int initEscalonador();

void printCpuSem();

int insertContextAtPrio(TCB_t *context, int prio);

int getNewTid();

int existeFilaPrio(int prio);
int estadoEntrada(TCB_t* nthread);


int yieldThread();
int deleteThreadCpuSem(TCB_t *context);
int resumeThread(TCB_t* thread);

void endThread();
int initEndingCTX();
int finishThread();
int dispatch();
ucontext_t* getEndingCtx();
TCB_t* getExecuting();
TCB_t* findFirstContext();


TCB_t *findAptoTID(int tid);
int joinThread(int tid);
TCB_t* findJoinThread(int tid);
