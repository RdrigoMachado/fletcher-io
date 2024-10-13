#include "../comunicacao.mpi.h"

MPI_Comm *childcomm;
int rank_destino = 0;
int processo_criados = 0;
int num_escrita = 0;
int qtdProcessos;
int tamanho;

void MPI_inicializacao(int sx, int sy, int sz, int qtdChilds)
{
  tamanho = sx * sy * sz;
  qtdProcessos = qtdChilds;
  childcomm = malloc(sizeof(MPI_Comm ) * qtdChilds);
}

void MPI_enviar_onda(float *ondaPtr) {
  if(processo_criados < qtdProcessos)
  {
    MPI_Comm_spawn("./spawn_one_at_time.x", MPI_ARGV_NULL, 1, MPI_INFO_NULL, 0, MPI_COMM_SELF, &(childcomm[processo_criados]), MPI_ERRCODES_IGNORE);
    MPI_Send(&tamanho, 1, MPI_INT, 0, MSG_CONTROLE, childcomm[processo_criados]);
    processo_criados++;
  }
  MPI_Send(&num_escrita, 1, MPI_INT, 0, MSG_CONTROLE, childcomm[rank_destino]);
  MPI_Send(ondaPtr, tamanho, MPI_FLOAT, 0, MSG_ONDA, childcomm[rank_destino]);
  
  rank_destino++;
  if(rank_destino == qtdProcessos)
    rank_destino = 0;
  num_escrita++;
}

void MPI_terminar()
{
  int msg = FLAG_FINALIZAR;
  for(int rank = 0; rank < processo_criados; rank++)
  {
    MPI_Send(&msg, 1, MPI_INT, 0, MSG_CONTROLE, childcomm[rank]);
  }
}
