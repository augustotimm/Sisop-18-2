#
# Makefile de EXEMPLO
#
# OBRIGATÓRIO ter uma regra "all" para geração da biblioteca e de uma
# regra "clean" para remover todos os objetos gerados.
#
# É NECESSARIO ADAPTAR ESSE ARQUIVO de makefile para suas necessidades.
#  1. Cuidado com a regra "clean" para não apagar o "support.o"
#
# OBSERVAR que as variáveis de ambiente consideram que o Makefile está no diretótio "cthread"
# 

CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src

all: cthread lcthread

lcthread: $(BIN_DIR)/lib.o $(BIN_DIR)/support.o #dependências para a regra1
	ar rcs $(LIB_DIR)/cthread.a $(BIN_DIR)/lib.o $(BIN_DIR)/support.o


cthread: $(INC_DIR)/support.h $(INC_DIR)/cthread.h $(INC_DIR)/cdata.h
	$(CC) -c $(SRC_DIR)/lib.c -o $(BIN_DIR)/lib.o




clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/lib.o  $(SRC_DIR)/*~ $(INC_DIR)/*~ *~

