//Autor: José Izaias da Silva Júnior
//Data: 15/09/2022
//Nome do projeto: Questão 5 - Lista de Threads

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

//Defines gerais.
#define NUMFIL 5
#define NUMREF 5

//arrays globais para contar as refeicoes e quantos garfos/filosofos tem.
int contal[NUMREF];
int garfos[NUMFIL];
//mutexs e variavel de condicao para espera.
pthread_mutex_t garfo[NUMFIL] = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t espgarf = PTHREAD_COND_INITIALIZER;

//escopo das funcoes.
void pensando(int numfil);
void pegarGarfos(int numfil);
void comer(int numfil);
void devolverGarfos(int numfil);
bool estaCheio(int numfil);

//thread filosofos.
void *filosofos(void *threadid){
    int thid = *((int*) threadid);
    bool acab;

    while(1){
        //contador para a refeicao acabar.
        acab = estaCheio(thid);
        if (acab == 1){
            break;
        }
        pensando(thid);
        pegarGarfos(thid);
        comer(thid);
        devolverGarfos(thid);
    }
    printf("Filosofo %d acabou a refeicao.\n", thid+1);

}

//thread main.
int main (void){
    pthread_t th[NUMFIL];
    int *thid[NUMFIL];

    //prints esteticos.
    puts("==============================================================");
    puts("");
    printf("Bem Vindo ao Jantar dos Filosofos!!\n\n");
    puts("==============================================================");
    sleep(1);
    puts("");
    printf("Essa noite temos %d Filosofos e iremos observar seu jantar!\n\n", NUMFIL);
    puts("==============================================================");
    sleep(1);

    //for para criacao dos filosofos.
    for(int t=0; t<NUMFIL; t++){
        thid[t] = (int *) malloc (sizeof(int));
        *thid[t] = t;
        pthread_create(&th[t], NULL, filosofos, (void*) thid[t]);
    }

    //for para esperar todas as threads concluirem.
    for (int u=0; u < NUMFIL ; u++){
        pthread_join(th[u], NULL);
    }

    //mensagem final.
    puts("==============================================================");
    puts("");
    printf("O jantar acabou, todos os Filosofos estao cheios!\n\n");
    puts("==============================================================");

    //liberando espaco alocado dinamicamente.
    for(int t=0; t < NUMFIL; t++){
        free(thid[t]);
    }

    return 0;
}

//funcao para o filosofo pensar.
void pensando(int numfil){
    printf("Filosofo %d esta pensando...\n", numfil+1);
    sleep(2);
}

//funcao para o filosofo pegar os garfos.
void pegarGarfos(int numfil){
    printf("O Filosofo %d, esta com fome e ira tentar comer.\n", numfil+1);
    //trava o mutex referente ao garfo.
    pthread_mutex_lock (&garfo[numfil%NUMFIL]);
    //se o garfo estiver com 0 ninguem esta utilizando.
    if(garfos[numfil%NUMFIL] == 0){
        //incrementa o garfo para ninguem usar este garfo.
        garfos[numfil%NUMFIL]++;
    }else{
        //se o garfo estiver ocupado espera e libera o mutex dele.
        pthread_cond_wait(&espgarf, &garfo[numfil%NUMFIL]);
        pthread_mutex_unlock (&garfo[numfil%NUMFIL]);
    }

    //mesma coisa para o segundo garfo.
    pthread_mutex_lock (&garfo[(numfil+1)%NUMFIL]);
    if(garfos[(numfil+1)%NUMFIL] == 0){
        garfos[(numfil+1)%NUMFIL]++;
    }else{
        pthread_cond_wait(&espgarf, &garfo[(numfil+1)%NUMFIL]);
        pthread_mutex_unlock (&garfo[(numfil+1)%NUMFIL]);
    }    
}

//funcao para o filosofo comer.
void comer(int numfil){
    printf("O Filosofo %d, esta comendo.\n", numfil+1);
    sleep(3);
}

//funcao para o filosofo devolver os garfos.
void devolverGarfos(int numfil){
    printf("O Filosofo %d terminou seu prato.\n", numfil+1);
    //poe 0 nos garfos novamente e incrementa o contador de refeicoes.
    garfos[numfil] = 0;
    garfos[numfil+1] = 0;
    contal[numfil]++;
    //libera os mutexs dos garfos.
    pthread_mutex_unlock (&garfo[numfil%NUMFIL]);
    pthread_mutex_unlock (&garfo[(numfil+1)%NUMFIL]);
    //manda um sinal para algum filosofo que esta esperando um garfo.
    pthread_cond_signal(&espgarf);
}

//funcao para saber se o filosofo esta cheio.
bool estaCheio(int numfil){
    if(contal[numfil] == NUMREF){
        return 1;
    }else
        return 0;
}