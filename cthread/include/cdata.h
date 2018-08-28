/*
 * cdata.h: arquivo de inclusão de uso apenas na geração da libpithread
 *
 * Esse arquivo NÃO pode ser modificado.
 *
 * VERSÃO: 27/08/2018
 *
 */
#ifndef __cdata__
#define __cdata__
#include "ucontext.h"

#define	PROCST_CRIACAO	0
#define	PROCST_APTO	1
#define	PROCST_EXEC	2
#define	PROCST_BLOQ	3
#define	PROCST_TERMINO	4
#define	PROCST_APTO_SUS	5
#define	PROCST_BLOQ_SUS	6

/* Os campos "tid", "state" e "context" dessa estrutura devem ser mantidos e usados convenientemente
   Pode-se acrescentar outros campos APÓS os campos obrigatórios dessa estrutura
*/
typedef struct s_TCB { 
	int		tid; 		// identificador da thread
	int		state;		// estado em que a thread se encontra
					// 0: Criação; 1: Apto; 2: Execução; 3: Bloqueado; 4: Término; 5: Apto-Suspenso; 6: Bloqueado-Suspenso
	int 		prio;		// Prioridade associada a thread NÃO USADO NESSA IMPLEMENTAÇÃO
	ucontext_t 	context;	// contexto de execução da thread (SP, PC, GPRs e recursos) 
	void		*data;	// aponta estruturas de dados, associadas à thread, necessárias à implementação 
	
} TCB_t; 


#endif
