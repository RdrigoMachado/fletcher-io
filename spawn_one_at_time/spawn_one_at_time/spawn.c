#include <mpi.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#define MSG_ONDA      101
#define MSG_CONTROLE  100
#define FLAG_FINALIZAR -1

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    MPI_Comm parentcomm;
    MPI_Comm_get_parent(&parentcomm);
    int num_escrita, tamanho;
    float *onda;
    char nome_base[128], nome_arquivo[128], parte[5];
    FILE *arquivo;

    MPI_Recv(&tamanho, 1, MPI_INT, 0, MSG_CONTROLE, parentcomm, MPI_STATUS_IGNORE);
    onda = malloc(sizeof(float) * tamanho); 

    strcpy(nome_base, "TTI.rsf@");

    while(1)
    {   
        MPI_Recv(&num_escrita, 1, MPI_INT, 0, MSG_CONTROLE, parentcomm, MPI_STATUS_IGNORE);
        if(num_escrita == FLAG_FINALIZAR)
            break;

        MPI_Recv((void *) onda, tamanho, MPI_FLOAT, 0, MSG_ONDA, parentcomm, MPI_STATUS_IGNORE);
        
        strcpy(nome_arquivo, nome_base);
        sprintf(parte, "%d", num_escrita);
        strcat(nome_arquivo,".part");
        strcat(nome_arquivo, parte);
        arquivo = fopen(nome_arquivo, "w+"); 

        fwrite(onda, sizeof(float), tamanho, arquivo);
        fclose(arquivo);
    }

    free(onda);
    MPI_Finalize();
    return 0;
}
