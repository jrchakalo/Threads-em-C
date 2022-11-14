//Autor: José Izaias da Silva Júnior
//Data: 09/09/2022
//Nome do projeto: Questão 2 - Lista de Threads

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//Defines gerais.
#define NUMTREM 10
#define NUMPASS 5

//arrays para as rotas do trens, para intersecoes, para os mutexs e varaveis de condicao(um para cada intesecao).
int rota[NUMTREM][NUMPASS] = {{1, 2, 3, 2, 3}, {2, 3, 5, 2, 5}, {4, 1, 4, 2, 5}, {4, 2, 5, 2, 5}, {4, 5, 3, 1, 2}, {3, 2, 3, 4, 6}, {5, 1, 2, 2, 3},
 {3, 5, 1, 2, 3}, {1, 2, 4, 5, 2}, {1, 2, 3, 1, 2}};
int inter[5]={0,0,0,0,0};
pthread_mutex_t travat[5];
pthread_cond_t espera[5];

//thread para um trem.
void *trem(void *threadid){
    int thid = *((int*) threadid);
    //array para o percurso do trem.
    int perc[NUMPASS];
    //numero do correspondente ao trem.
    int numtrem = thid + 1;

    //for para pegar o percurso que o trem vai fazer.
    for(int i=0; i<NUMPASS; i++){
        perc[i] = rota[thid][i];
    }

    //duplo for para selecionar a intersecao que o trem deseja ir.
    for(int i=0; i<NUMPASS; i++){
        for(int j=1;j<=5;j++){
            if(perc[i] == j){
                //se o mutex tiver disponivel, trava ele.
                pthread_mutex_lock(&travat[j]);
                //se a intersecao for maior que 1 mais de dois trens estarao nela.
                while(inter[j] > 1){
                    //se isso for verdade, faz o trem que solicitou a entrada na intersecao dormir ate ela estar disponivel.
                    printf("Intersecao %d cheia, Trem %d esperando.\n", j, numtrem);
                    pthread_cond_wait(&espera[j], &travat[j]);
                }
                //se nao for, incrementa o numero de trens nessa intersecao e libera o mutex dela.
                inter[j]++;
                pthread_mutex_unlock(&travat[j]);
                //o trem passa pela intersecao.
                printf("Trem %d passando na Intersecao %d\n", numtrem, j);
                usleep(500*1000);
                //trava o mutex da intersecao, diminui o valor de trens nela e libera o mutex.
                pthread_mutex_lock(&travat[j]);
                inter[j]--;
                pthread_mutex_unlock(&travat[j]);
                //manda um sinal para alguma thread que ficou presa no wait acordar e poder passar pela intersecao.
                pthread_cond_signal(&espera[j]);
            }
            
        }
    }
    //quando o trem terminar toda sua rota, imprime essa mensagem.
    printf("Fim da rota do Trem %d\n", numtrem);
}

//thread main.
int main (void){
    pthread_t th[NUMTREM];
    int *thid[NUMTREM];

    //prints esteticos.
    puts("=========================================================");
    printf("Relatorio de intersecoes onde os trens iram trafegar:\n\n");
    for(int i=0; i<NUMTREM; i++){
        printf("Rota do Trem %d: ", i+1);
        for(int j=0; j<NUMPASS; j++){
            if(j != 4){
                printf("%d, ", rota[i][j]);
            }else{
                printf("%d\n", rota[i][j]);
            }
        }
    }
    puts("");
    puts("=========================================================");
    sleep(1);
    puts("");
    printf("Abrindo a estacao:\n\n");
    puts("=========================================================");
    sleep(1);

    //for para criacao das threads.
    for (int t=0; t<NUMTREM; t++){
        thid[t] = (int *) malloc (sizeof(int));
        *thid[t] = t;
        pthread_create(&th[t], NULL, trem, (void*) thid[t]);
    }

    //for para esperar todas as threads concluirem.
    for (int u=0; u < NUMTREM; u++){
        pthread_join(th[u], NULL);
    }

    //mensagem final.
    puts("=========================================================");
    puts("");
    printf("Todas as viagens foram concluidas!\n\n");
    puts("=========================================================");


    //libera o espaco de memoria alocado por thid.
    for(int t=0; t < NUMTREM; t++){
        free(thid[t]);
    }

    return 0;
}