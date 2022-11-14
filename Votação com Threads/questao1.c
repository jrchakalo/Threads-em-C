//Autor: José Izaias da Silva Júnior
//Data: 08/09/2022
//Nome do projeto: Questão 1 - Lista de Threads

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//Defines gerais.
#define NUMARQ 4
#define NUMCAND 2
#define NUMTH 2

//array de resultado e mutex globais.
int result [NUMCAND];
pthread_mutex_t mtxresult = PTHREAD_MUTEX_INITIALIZER;

//thread para ler e contabilizar os votos dos arquivos.
void *lerArq(void *thid){
    FILE *arq;
    int tid = *((int*) thid);
    int arqnum;
    char arqnom[10];
    int comp;

    //for para que as threads nao entrem em conflito.
    //cada thread abre e contabiliza o arquivo com seu id ou seu id + o numero de threads.
    for (int t = tid; t < NUMARQ; t = t + NUMTH){
        //para transformar o numero do id em uma string
        arqnum = t+1;
        sprintf (arqnom, "%d.txt", arqnum);

        //passa a string transformada para funcao de abrir arquivo.
        arq = fopen (arqnom, "r");
        if(!arq){
            //notifica se nao conseguir abrir o arquivo e sai da thread.
            printf("Erro em abrir o arquivo\n");
            pthread_exit(NULL);
        }

        //while que roda enquanto o arquivo nao chega no final.
        while (!feof(arq)){
            fscanf (arq, "%d", &comp);
            //for para incrementar somente o candidato que corresponde a o numero no arquivo.
            for (int i=0; i<=NUMCAND; i++){
                if(comp == i){
                    //mutex lock para garantir a exclusao multua, pois result eh regiao critica.
                    pthread_mutex_lock(&mtxresult);
                    result[i]++;
                    pthread_mutex_unlock(&mtxresult);
                }
            }
        }

        //fechamento do arquivo.
        fclose(arq);
    }
    
}

//cabecalho para funcao de printar.
void printResult (int result[]);

//thread main.
int main (void){
    int *thid[NUMTH];
    pthread_t th[NUMTH];

    //for para criacao das threads.
    for(int t=0; t < NUMTH; t++){
        thid[t] = (int *) malloc (sizeof(int));
        *thid[t] = t;
        pthread_create(&th[t], NULL, lerArq, (void *) thid[t]);
    }
    
    //for para esperar (join) todas as threads acabarem de executar.
    for(int u=0; u < NUMTH; u++){
        pthread_join(th[u], NULL);
    }
    
    //chama a funcao para printar o resultado.
    printResult (result);

    //libera o espaco de memoria alocado por thid.
    for(int t=0; t < NUMTH; t++){
        free(thid[t]);
    }

    return 0;
}

//Funcao para printar o resultado.
void printResult (int result[]){
    int totvot=0;
    float porcvot;
    int venc=0, maior=0;

    //for que determina o total de votos e o vencedor.
    for (int i=0; i<=NUMCAND; i++){
        if (result[i] > maior){
            maior = result[i];
            venc = i;
        }
        totvot += result[i];
    }
    printf("O total de votos da eleicao foi: %d\n", totvot);

    //for para mostrar as porcentagens.
    printf("As porcentagens de cada candidato foram: \n");
    for (int i=0; i <= NUMCAND; i++){
        if(i == 0){
            porcvot = ((float) result[i]/totvot) * 100;
            printf("Votos em branco %d: %.1f%%\n", i, porcvot);
        }else{
            porcvot = ((float) result[i]/totvot) * 100;
            printf("Candidado %d: %.1f%%\n", i, porcvot);
        }
        
    }
    
    //print do vencedor.
    printf("O vencedor foi o: Candidato %d!!\n", venc);
}
