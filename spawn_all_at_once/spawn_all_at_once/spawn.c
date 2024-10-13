#include <mpi.h>
#include <stdio.h>
#include <stdlib.h> 

#define MSG_ONDA      101
#define MSG_CONTROLE  100
#define FLAG_FINALIZAR -1

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    int rank, num_escrita, tamanho, sucesso;
    float *onda;
    MPI_File arquivo;
    MPI_Comm interCommParent;
    MPI_Offset deslocamento;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_get_parent(&interCommParent);
    MPI_Recv(&tamanho, 1, MPI_INT, 0, MSG_CONTROLE, interCommParent, MPI_STATUS_IGNORE);
    
    onda = malloc(sizeof(float) * tamanho);
    if(onda == NULL)
    {
        printf("#%d - Erro ao alocar memoria. Terminando\n", rank);
        MPI_Finalize();
        return 0;
    }
       
    sucesso = MPI_File_open(MPI_COMM_WORLD, "TTI.rsf@",
                MPI_MODE_CREATE | MPI_MODE_WRONLY,
                MPI_INFO_NULL, &arquivo);
    if(sucesso != MPI_SUCCESS)
        printf("#%d - ERRO ao abrir arquivo\n", rank);
   
    

    while(1)
    {   
        sucesso = MPI_Recv(&num_escrita, 1, MPI_INT, 0, MSG_CONTROLE, interCommParent, MPI_STATUS_IGNORE);
        if(sucesso != MPI_SUCCESS)
                printf("#%d - ERRO ao receber msg controle\n", rank);
        
        if(num_escrita == FLAG_FINALIZAR)
            break;
        deslocamento = (num_escrita) * (MPI_Offset) tamanho * sizeof(float);

        sucesso = MPI_Recv((void *) onda, tamanho, MPI_FLOAT, 0, MSG_ONDA, interCommParent, MPI_STATUS_IGNORE);
        if(sucesso != MPI_SUCCESS)
                printf("#%d - ERRO ao receber onda\n", rank);
        
        sucesso = MPI_File_write_at(arquivo, deslocamento, onda, tamanho, MPI_FLOAT, MPI_STATUS_IGNORE);
        if(sucesso != MPI_SUCCESS)
            printf("#%d - ERRO ao escrever no arquivo\n", rank);
    }
    
    free(onda);
    sucesso = MPI_File_close(&arquivo);
    if(sucesso != MPI_SUCCESS)
        printf("#%d - ERRO ao fechar arquivo\n", rank);

    MPI_Finalize();

    return 0;
}
