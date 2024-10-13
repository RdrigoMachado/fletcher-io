#ifndef _COMMUNICACAO_MPI_
#define _COMMUNICACAO_MPI_

#include <mpi.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"

#define MSG_ONDA      101
#define MSG_CONTROLE  100
#define FLAG_FINALIZAR -1
#define FLAG_ONDA 1
#define NOME_ARQUIVO "TTI.rsf@"

void MPI_escrita_disco(int sx, int sy, int sz, const int st,  const float dtOutput, const float dt);
void MPI_enviar_onda(int sx, int sy, int sz, float *ondaPtr);
void finalizar_comunicacao();

#endif