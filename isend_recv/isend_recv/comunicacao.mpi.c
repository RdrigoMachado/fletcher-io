#include "../comunicacao.mpi.h"

MPI_Request request = MPI_REQUEST_NULL;
MPI_Status  status;
float *onda = NULL;
//#############################    MPI  SEND   ##################################
void MPI_enviar_onda(int sx, int sy, int sz, float *ondaPtr) 
{
  MPI_Wait(&request, MPI_STATUS_IGNORE);

  int tamanho = sx * sy * sz;
  if(onda == NULL){
    onda = malloc(sizeof(float) * tamanho);
  }
  memcpy(onda, ondaPtr, tamanho * sizeof(float));
  MPI_Isend((void *) onda, tamanho , MPI_FLOAT, 1, MSG_ONDA, MPI_COMM_WORLD, &request);  
}

void  finalizar_comunicacao()
{
  MPI_Wait(&request, MPI_STATUS_IGNORE);
  MPI_Finalize();
  free(onda);
}

//##########################    MPI  RECEIVE   ##################################

void MPI_escrita_disco(int sx, int sy, int sz, const int st, const float dtOutput, const float dt)
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
  free(onda);
  exit(0);
}
