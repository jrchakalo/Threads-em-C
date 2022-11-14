//Autor: José Izaias da Silva Júnior
//Data: 13/09/2022
//Nome do projeto: Questão 3.1 (VERSAO INFINITA) - Lista de Threads

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//Defines gerais.
#define THREAD 5
#define THWRITE 10
#define TAMBD 10
#define RMAX 50

//array para o banco de dados, mutexs e variaveis de condicao.
int bd[TAMBD];
pthread_mutex_t mtx_esc = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx_ler = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t espler = PTHREAD_COND_INITIALIZER;
pthread_cond_t espesc = PTHREAD_COND_INITIALIZER;

//thread escritora.
void *escritora(void *threadid){
    int thid = *((int*) threadid);

    while(1){
        //da lock no mutex assim que entra no while.
        pthread_mutex_lock(&mtx_esc);
        //for de escricao.
        for(int i=0; i < TAMBD; i++){
            //acha um espaco em branco dentro do banco de dados
            if(bd[i] == 0){
                //escreve um numero randomico entre 1 e RMAX.
                bd[i] = 1 + rand() % RMAX;
                printf("Escritora %d escrevendo o valor %d na posicao %d do banco de dados\n", thid, bd[i], i);
                //manda um sinal para alguma possivel leitora que esta dormindo e destrava o mutex.
                pthread_cond_signal(&espler);
                pthread_mutex_unlock(&mtx_esc);
                //sai do for e volta para o loop.
                break;
            //se o i for o ultimo numero e nao tiver nenhum 0, o banco de dados esta cheio.
            }else if( i == (TAMBD-1)){
                //espera algum leitor retirar esse numero e libera o mutex.
                pthread_cond_wait(&espesc, &mtx_esc);
                pthread_mutex_unlock(&mtx_esc);
            }
        }
    }
}

//thread leitora.
void *leitora(void *threadid){
    int thid = *((int*) threadid);

    while(1){
        //da um lock no mutex leitor no comeco.
        pthread_mutex_lock(&mtx_ler);
        for(int i=0; i < TAMBD; i++){
            //ve se no banco de dados tem algum numero diferente de 0.
            if(bd[i] != 0){
                //printa se existir e transforma ele em 0.
                printf("Leitora %d lendo o valor %d na posicao %d do banco de dados\n", thid, bd[i], i);
                bd[i] = 0;
                //avisa alguma possivel escritora que esta dormindo e destrava o mutex.
                pthread_cond_signal(&espesc);
                pthread_mutex_unlock(&mtx_ler);
                //sai do for e volta para o loop.
                break;
            //se nao tiver nenhum numero diferente de 0, dorme e destrava o mutex.
            }else if(i == (TAMBD-1)){
                pthread_cond_wait(&espler, &mtx_ler);
                pthread_mutex_unlock(&mtx_ler);
            }
        }
    }
    
}

//thread main.
int main (void){
    pthread_t thw[THWRITE];
    pthread_t thr[THREAD];
    int *thid[THWRITE];
    int *thid2[THREAD];

    //funcao para randomizar a sequencia se numeros.
    srand(time(NULL));

    //prints esteticos.
    puts("=========================================================");
    printf("Quantidade de Threads Escritoras: %d\n\n", THWRITE);
    printf("Quantidade de Threads Leitoras: %d\n", THREAD);
    puts("=========================================================");
    sleep(1);
    puts("");
    printf("Comecando a Escrita e a Leitura:\n\n");
    puts("=========================================================");
    sleep(1);
    
    //criacao das threads escritoras
    for(int i=0; i < THWRITE; i++){
        thid[i] = (int *) malloc (sizeof(int));
        *thid[i] = i;
        pthread_create(&thw[i], NULL, escritora, (void*) thid[i]);
    }

    //criacao das threads leitoras
    for(int i=0; i < THREAD; i++){
        thid2[i] = (int *) malloc (sizeof(int));
        *thid2[i] = i;
        pthread_create(&thr[i], NULL, leitora, (void*) thid2[i]);
    }
    
    //nunca chega aqui pois as threads executam infinitamente.
    for (int u=0; u < THREAD; u++){
        pthread_join(thr[u], NULL);
    }
    
    for(int t=0; t < THWRITE; t++){
        free(thid[t]);
    }

    for(int t=0; t < THREAD; t++){
        free(thid2[t]);
    }

    return 0;
}