#include "../comunicacao.mpi.h"

//#############################    MPI  SEND   ##################################

void MPI_enviar_onda(int sx, int sy, int sz, float *ondaPtr) {
  int tamanho = sx * sy * sz;
  MPI_Send((void *) ondaPtr, tamanho , MPI_FLOAT, 1, MSG_ONDA, MPI_COMM_WORLD);  
}

void  finalizar_comunicacao()
{
  MPI_Finalize();
}
//##########################    MPI  RECEIVE   ##################################

void MPI_escrita_disco(int sx, int sy, int sz, const int st,  const float dtOutput, const float dt)
{
  int tamanho = sx * sy * sz;
  float *onda = malloc(sizeof(float) * tamanho); 
  FILE *arquivo = fopen(NOME_ARQUIVO, "w+");

  float tSim=0.0;
  int nOut=1;
  float tOut=nOut*dtOutput;
 
  MPI_Recv((void *) onda, tamanho, MPI_FLOAT, 0, MSG_ONDA, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  fwrite((void *) onda, sizeof(float), tamanho, arquivo);

  for (int it=1; it<=st; it++) {

    tSim=it*dt;
    if (tSim >= tOut) {
      MPI_Recv((void *) onda, tamanho, MPI_FLOAT, 0, MSG_ONDA, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      fwrite((void *) onda, sizeof(float), tamanho, arquivo);
      tOut=(++nOut)*dtOutput;
    }
  }
  fclose(arquivo);
  MPI_Finalize();
  exit(0);
}

