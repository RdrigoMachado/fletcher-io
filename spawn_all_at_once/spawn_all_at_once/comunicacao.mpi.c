#include "../comunicacao.mpi.h"

MPI_Comm childcomm;
int rank_destino = 0;
int num_escrita = 0;
int tamanho;
int qtdProcessos;

void MPI_inicializacao(int sx, int sy, int sz, int qtdChilds)
{
  tamanho = sx * sy * sz;
  qtdProcessos = qtdChilds;
  MPI_Comm_spawn("./spawn_all_at_once.x", MPI_ARGV_NULL, qtdChilds, MPI_INFO_NULL, 0, MPI_COMM_SELF, &childcomm, MPI_ERRCODES_IGNORE);
  for(int i = 0; i < qtdProcessos; i++)
  {
    MPI_Send(&tamanho, 1, MPI_INT, i, MSG_CONTROLE, childcomm);
  }
}

void MPI_enviar_onda(float *ondaPtr)
{
  MPI_Send(&num_escrita, 1, MPI_INT, rank_destino, MSG_CONTROLE, childcomm);
  MPI_Send(ondaPtr, tamanho, MPI_FLOAT, rank_destino, MSG_ONDA, childcomm);

  rank_destino++;
  if(rank_destino == qtdProcessos)
    rank_destino = 0;

  num_escrita++;
}

void MPI_terminar()
{
  int msg = FLAG_FINALIZAR;
  for(int i = 0; i < qtdProcessos; i++)
  {
    MPI_Send(&msg, 1, MPI_INT, i, MSG_CONTROLE, childcomm);
  }
}
