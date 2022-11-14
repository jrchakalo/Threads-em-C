//Autor: José Izaias da Silva Júnior
//Data: 15/09/2022
//Nome do projeto: Questão 7 - Lista de Threads

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//Define da ordem da matriz quadrada.
#define ORD 4


//matrizes para teste.
int quad1[ORD][ORD] = {{1, 2, 3, 4}, {2, 1, 4, 3}, {3, 4, 1, 2}, {4, 3, 2, 1}};
int quad2[ORD][ORD] = {{1, 1, 1, 1}, {3, 4, 1, 2}, {2, 1, 4, 3}, {1, 1, 1, 1}};
int quad3[ORD-1][ORD-1] = {{1, 1, 1}, {3, 4, 1}, {2, 1, 4}};
int quad4[ORD+1][ORD+1] = {{1, 1, 1, 1, 2}, {3, 4, 1, 2, 2}, {2, 1, 4, 3, 1}, {1, 1, 1, 1, 1}, {1, 2, 3, 2, 1}};
int quad5[ORD][ORD] = {{1, 2, 3, 4}, {2, 2, 2, 2}, {3, 3, 3, 3}, {4, 3, 2, 1}};
int quad6[ORD][ORD] = {{1, 2, 3, 4}, {3, 4, 1, 2}, {2, 1, 4, 3}, {4, 3, 2, 1}};

//matrizes usada pelas threads, contador e mutex para o contador que eh regiao critica.
int quad[ORD][ORD];
int quadt2[ORD-1][ORD-1];
int quadt3[ORD+1][ORD+1];
int cont=0;
//variavel para manipular o tamanho da matriz, util apenas para as exemplificacoes.
int ord = 0;
pthread_mutex_t inc = PTHREAD_MUTEX_INITIALIZER;

//escopo das funcoes.
int fatorial(int n);
void copiaMatriz(int tam, int matrizd[tam][tam], int matrizo[tam][tam]);
void verificaLatina(int tam);

//thread para linha.
void *linha(void *threadid){
    int thid = *((int*) threadid);
    //auxiliar para calculo.
    int aux=0;
    int aux2=0;

    //for para comparar os valores da linha e ver se eles sao diferentes.
    for(int i = thid; i < ord; i = i + (ord/2)){
        for(int j = 0; j < ord; j++){
            for (int k=j+1; k < ord; k++){
                //se os elementos forem diferentes vai incrementando em aux.
                if(quad[i][j] != quad[i][k]){
                    aux++;
                }
            }
        }
        //segundo auxiliar para conta.
        aux2++;
    }

    //se a matriz for de ordem par.
    if (ord%2 == 0){
        //e se o auxiliar for equivalente a ((ordem vezes 2) vezes o numero de thread) menos a ordem.
        if(aux == (((ord*2)*(ord/2)) - ord)){
            //incrementa o contador.
            pthread_mutex_lock(&inc);
            cont++;
            pthread_mutex_unlock(&inc);
        }
    //se for de ordem impar.
    }else{
        //e se o auxiliar for equivalente a (ordem vezes a quantidade de linhas ou colunas que a thread fica (aux2)) vezes o numero de thread.
        if(aux == ((ord*aux2) * ord/2)){
            //incrementa o contador.
            pthread_mutex_lock(&inc);
            cont++;
            pthread_mutex_unlock(&inc);    
        }
    }
}

//analogo para thread coluna
void *coluna(void *threadid){
    int thid = *((int*) threadid);
    int aux=0;
    int aux2=0;

    for(int i = thid; i < ord; i = i + (ord/2)){
        for(int j = 0; j < ord; j++){
            //apenas troca a posicao do i
            for (int k=j+1; k < ord; k++){
                if(quad[j][i] != quad[k][i]){
                    aux++;
                }
            }
        }
        aux2++;
    }

    if (ord%2 == 0){
        if(aux == (((ord*2)*(ord/2)) - ord)){
            pthread_mutex_lock(&inc);
            cont++;
            pthread_mutex_unlock(&inc);
        }
    }else{
        if(aux == ((ord*aux2) * ord/2)){
            pthread_mutex_lock(&inc);
            cont++;
            pthread_mutex_unlock(&inc);    
        }
    }
}

//thread main.
int main (void){
    //prints esteticos para testes.
    puts("=================================================");
    puts("");
    printf("Vamos ver se as matrizes sao latinas!!\n\n");
    puts("=================================================");
    sleep(1);

    puts("=================================================");
    puts("");
    printf("PRIMEIRA MATRIZ\n\n");

    ord = ORD;
    copiaMatriz(ord, quad, quad1);
    puts("");
    verificaLatina(ord);
    puts("");
    sleep(1);
    puts("=================================================");
    puts("=================================================");
    puts("");
    printf("SEGUNDA MATRIZ\n\n");

    ord = ORD;
    copiaMatriz(ord, quad, quad2);
    puts("");
    verificaLatina(ord);
    puts("");
    sleep(1);
    puts("=================================================");
    puts("=================================================");
    puts("");
    printf("TERCEIRA MATRIZ\n\n");

    ord = ORD-1;
    copiaMatriz(ord, quadt2, quad3);
    puts("");
    verificaLatina(ord);
    puts("");
    sleep(1);
    puts("=================================================");
    puts("=================================================");
    puts("");
    printf("QUARTA MATRIZ\n\n");

    ord = ORD+1;
    copiaMatriz(ord, quadt3, quad4);
    puts("");
    verificaLatina(ord);
    puts("");
    sleep(1);
    puts("=================================================");
    puts("=================================================");
    puts("");
    printf("QUINTA MATRIZ\n\n");

    ord = ORD;
    copiaMatriz(ord, quad, quad5);
    puts("");
    verificaLatina(ord);
    puts("");
    sleep(1);
    puts("=================================================");
    puts("=================================================");
    puts("");
    printf("SEXTA MATRIZ\n\n");

    copiaMatriz(ord, quad, quad6);
    puts("");
    verificaLatina(ord);
    puts("");
    sleep(1);
    puts("=================================================");

    return 0;
}

//funcao fatorial.
int fatorial(int n){
    if (n <= 1){
        return 1;
    }

    return n*fatorial(n-1);
}

//funcao para copiar a matriz e imprimir ela.
void copiaMatriz(int tam, int matrizd[tam][tam], int matrizo[tam][tam]){
    for(int i=0; i < tam; i++){
        for(int j=0; j < tam; j++){
            matrizd[i][j] = matrizo[i][j];
            if(j != (tam-1)){
                printf(" %d |", quad[i][j]);
            }else{
                printf(" %d\n", quad[i][j]);
            }
        }
        if(ord == 4){
            puts("---------------");
        }else if (ord == 3){
            puts("-----------");
        }else if (ord == 5){
            puts("-------------------");
        }
        
    }
}

//funcao para criar as threads e verificar se a matriz eh latina atraves delas.
void verificaLatina(int tam){
    pthread_t thl[tam/2];
    pthread_t thc[tam/2];
    int *thidl[tam/2];
    int *thidc[tam/2];

    //criando as threads das linhas.
    for(int i=0; i < (tam/2); i++){
        thidl[i] = (int*) malloc (sizeof(int));
        *thidl[i] = i;
        pthread_create(&thl[i], NULL, linha, (void*) thidl[i]);
    }

    //criando as threads das colunas.
    for(int i=0; i < (tam/2); i++){
        thidc[i] = (int*) malloc (sizeof(int));
        *thidc[i] = i;
        pthread_create(&thc[i], NULL, coluna, (void*) thidc[i]);
    }
    
    //join para esperar as threads acabarem.
    for (int u=0; u < (tam/2) ; u++){
        pthread_join(thl[u], NULL);
        pthread_join(thc[u], NULL);
    }

    //dando free nos vetores alocadas dinamicamente.
    for (int u=0; u < (tam/2) ; u++){
        free(thidl[u]);
        free(thidc[u]);
    }

    //verificacao se a matriz eh latina.
    if(cont == tam){
        printf("A MATRIZ EH LATINA\n");
    }else{
        printf("A MATRIZ NAO EH LATINA\n");
    }
    
    //reseta contador para outra verificacao.
    cont = 0;
}