//Autor: José Izaias da Silva Júnior
//Data: 16/09/2022
//Nome do projeto: Questão 6 - Lista de Threads

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//Defines gerais.
#define LIN 3
#define COL 3
#define RMAX 20
//para ter menos elementos preenchidos na matriz e realmente ser uma matriz esparsa.
#define NUMPOS COL-1

//estrutura para representar a tupla (posicao, valor).
struct tupEsp{
    float val;
    int posi;
};

//matrizes e vetores globais para teste.
float vetort[COL];
float vetorx[LIN];
float matrizch[LIN][COL];
struct tupEsp matrizrt[LIN][COL];
struct tupEsp esparsa[LIN][COL];
struct tupEsp esparsax[LIN][COL];
struct tupEsp esparsaresult[LIN][COL];

//escopo de funcoes.
void colocaPares(struct tupEsp esparsa[LIN][COL]);
void preecheVetor();
void preencheMatriz();

//thread para multiplicar matriz esparsa por vetor.
void *multVet(void *threadid){
    int thid = *((int*) threadid);
    float mult = 0.0;

    //multiplica cada elemento da linha por cada elemento do vetor e soma os resultados.
    for(int i = 0; i < COL; i++){
        if(esparsa[thid][i].posi <= COL && esparsa[thid][i].posi == i){
            mult += esparsa[thid][i].val * vetort[i];
        }
             
    }

    //atribui o resultado a posicao correspondente do vetor resultado.
    vetorx[thid] = mult;
}

//thread para multiplicar duas matrizes esparsas.
void *multEsp(void *threadid){
    int thid = *((int*) threadid);
    float mult = 0.0;

    for(int i = 0; i < LIN; i++){
        for(int j = 0; j < COL; j++){
            //verifica se o elemento da primeira matriz eh lixo.
            if(esparsa[thid][j].posi <= COL && esparsa[thid][j].posi == j){
                //se nao for verica o da segunda.
                if(esparsax[j][i].posi <= LIN && esparsax[j][i].posi == i)
                    //se nao for multiplica o elemento e soma com o proximo.
                    mult += esparsa[thid][j].val * esparsax[j][i].val;
            }
        }
        //se ao final das iteracoes a variavel da multiplicacao e soma nao for 0.
        if(mult != 0){
            //atribui a posicao e o valor ao elemento da matriz resultante.
            esparsaresult[thid][i].posi = i;
            esparsaresult[thid][i].val = mult;
        }
        //reseta a variavel de soma das multiplicacoes.
        mult = 0.0;
    }
}

//thread para multiplicar uma matriz esparsa por uma cheia
void *multCheia(void *threadid){
    int thid = *((int*) threadid);
    float mult = 0.0;

    for(int i = 0; i < LIN; i++){
        for(int j = 0; j < COL; j++){
            //se o valor da matriz esparsa nao for lixo.
            if(esparsa[thid][j].posi <= COL && esparsa[thid][j].posi == j){
                    //soma as multiplicacoes realizadas pelo valor da linha da esparsa e o da coluna da cheia
                    mult += esparsa[thid][j].val * matrizch[j][i];
            }
        }
        if(mult != 0){
            //o motivo da matriz resultante entre uma matriz esparsa e uma matriz cheiar ser do tipo esparsa eh pq
            //se uma linha inteira da matriz esparsa for 0 isso ocasio uma linha inteira na resultante igual a 0.
            //entao se for uma esparsa ela ignorar esses espacos iguais a 0.
            matrizrt[thid][i].posi = i;
            matrizrt[thid][i].val = mult;
        }
        mult = 0.0;
    }
}

//thread main
int main (void){
    pthread_t th[LIN];
    int *thid[LIN];
    srand(time(NULL));

    //prints esteticos e testes.
    puts("===========================================================");
    puts("");
    printf("Operacoes com Matrizes Esparsas!!!!\n\n");
    puts("===========================================================");
    sleep(1);
    printf("Multiplicacao entre uma Matriz Esparsa e um Vetor Denso.\n");
    puts("===========================================================");
    printf("Dada a Matriz Esparsa: \n\n");
    //gera o vetor esparso.
    colocaPares(esparsa);
    
    puts("");
    printf("E o Vetor Denso: ");
    //preenche o vetor denso com valores aleatorios de um certo intervalo
    preecheVetor();

    //cria as threads para o primeiro teste.
    for(int t=0; t < LIN; t++){
        thid[t] = (int*) malloc (sizeof(int));
        *thid[t] = t;
        pthread_create(&th[t], NULL, multVet, (void*) thid[t]);
    }
    
    //espera elas acabarem os calculos.
    for(int u; u < LIN; u++){
        pthread_join(th[u], NULL);
    }

    //printa o resultado.
    puts("");
    printf("O Vetor resultante sera: ");
    for(int i=0; i < LIN; i++){
        if(i != LIN-1){
            printf("(%.2f) ", vetorx[i]);
        }else{
            printf("(%.2f)\n", vetorx[i]);
        }
    }

    puts("");
    puts("===========================================================");
    printf("Multiplicacao entre duas Matrizes Esparsas.\n");
    puts("===========================================================");
    printf("Dada uma Matriz Esparsa: \n\n");
    //re-imprime a matriz esparsa.
    for (int i=0; i < LIN;i++){
        for (int j=0; j<COL; j++){
            
            if (esparsa[i][j].posi <= COL && esparsa[i][j].posi == j){
                printf("(%d, %.2f) ", esparsa[i][j].posi, esparsa[i][j].val);
            }else{
                //print estetico para nao imprimir o lixo ou deixar em branco
                printf("(%d, %.2f) ", j, 0.0);
            }
        }
        puts("");
    }
    puts("");
    printf("E uma segunda Matriz Esparsa: \n\n");
    //gera uma segunda matriz esparsa para fazer os calculos.
    colocaPares(esparsax);
    puts("");

    //cria as threads para fazer os calculos.
    for(int t=0; t < LIN; t++){
        thid[t] = (int*) malloc (sizeof(int));
        *thid[t] = t;
        pthread_create(&th[t], NULL, multEsp, (void*) thid[t]);
    }
    
    for(int u; u < LIN; u++){
        pthread_join(th[u], NULL);
    }

    printf("A Matriz Esparsa resultante sera: \n\n");

    //printa a matriz esparsa resultante.
    for (int i=0; i < LIN;i++){
        for (int j=0; j<COL; j++){
            
            if (esparsaresult[i][j].posi <= COL && esparsaresult[i][j].posi == j){
                printf("(%d, %.2f) ", esparsaresult[i][j].posi, esparsaresult[i][j].val);
            }else{
                //print estetico para nao imprimir o lixo ou deixar em branco
                printf("(%d, %.2f) ", j, 0.0);
            }
        }
        puts("");
    }

    puts("");
    puts("===========================================================");
    printf("Multiplicacao entre uma Matriz Esparsa e uma cheia.\n");
    puts("===========================================================");
    printf("Dada uma Matriz Esparsa: \n\n");
    //re-imprime a matriz esparsa.
    for (int i=0; i < LIN;i++){
        for (int j=0; j<COL; j++){
            
            if (esparsa[i][j].posi <= COL && esparsa[i][j].posi == j){
                printf("(%d, %.2f) ", esparsa[i][j].posi, esparsa[i][j].val);
            }else{
                //print estetico para nao imprimir o lixo ou deixar em branco
                printf("(%d, %.2f) ", j, 0.0);
            }
        }
        puts("");
    }
    puts("");
    printf("E a Matriz Cheia: \n\n");
    //preenche a matriz cheia com valores aleatorios de um certo intervalo.
    preencheMatriz(matrizch);
    puts("");

    //cria as threads para fazer os calculos.
    for(int t=0; t < LIN; t++){
        thid[t] = (int*) malloc (sizeof(int));
        *thid[t] = t;
        pthread_create(&th[t], NULL, multCheia, (void*) thid[t]);
    }
    
    for(int u; u < LIN; u++){
        pthread_join(th[u], NULL);
    }

    printf("A Matriz resultante sera: \n\n");

    //imprime a matriz resultante.
    for (int i=0; i < LIN;i++){
        for (int j=0; j<COL; j++){
            
            if (matrizrt[i][j].posi <= COL && matrizrt[i][j].posi == j){
                printf("(%d, %.2f) ", matrizrt[i][j].posi, matrizrt[i][j].val);
            }else{
                //print estetico para nao imprimir o lixo ou deixar em branco
                printf("(%d, %.2f) ", j, 0.0);
            }
        }
        puts("");
    }

    //liberando espaco alocado dinamicamente.
    for(int t=0; t < LIN; t++){
        free(thid[t]);
    }

    return 0;
}

//funcao para preencher uma matriz esparsa.
void colocaPares(struct tupEsp esparsa[LIN][COL]){
    //define um valor aleatorio entre 1 e NUMPOS para quantos valores da linha serao preenchidos.
    int numpos = 1 + rand() % (NUMPOS);
    int pos=0;
    int aux=999;
    int cont=0;
    int verif=0;

    for (int i=0; i < LIN; i++){
        //for para preencher na linha apenas a quantidade de colunas sorteadas.
        for (int j=0; j < numpos;j++){
            //sorteia a coluna que vai ser preenchida.
            pos = rand() % COL;
            //while infinito para evitar sobrescrita.
            while(1){
                //variavel auxiliar inicia com numero alto para garantir entrada no if.
                if(aux != pos){
                    //coloca a posicao sorteada na matriz esparsa.
                    esparsa[i][pos].posi = pos;
                    //preenche o valor da posicao sorteada com um numero aleatorio de um certo intervalo.
                    esparsa[i][pos].val = -9.0 + (((float) rand()) / ((float) RAND_MAX) * RMAX);
                    //while infinito para tratar erros.
                    while(1){    
                        //se o valor da coluna 0 em qualquer linha for muito baixo eh lixo.
                        if(esparsa[i][0].val < 0.001){
                            //entao coloca uma um valor alto na posicao para ele ser identificado como lixo.
                            esparsa[i][0].posi = 100 + rand();
                        }
                        //se o valor sorteado for muito pequeno foge da zona do printf.
                        if (esparsa[i][pos].val < 0.001){
                            //entao sorteia outro numero.
                            esparsa[i][pos].val = -9.0 + (((float) rand()) / ((float) RAND_MAX) * RMAX);
                            //e soma um na variavel verificadora.
                            verif++;
                        }else{
                            //que serve somente para garantir que possa haver numeros negativos na matriz.
                            if(verif != 0){
                                esparsa[i][pos].val = -esparsa[i][pos].val;
                                verif = 0;
                            }
                            //sai do loop infinito da correcao.
                            break;
                        }
                    }
                    //salva a ultima posicao.
                    aux = pos;
                    //sai do loop infinito que evita sobrescricao.
                    break;
                }else{
                    //se cair uma posicao igual a anterior, sorteia outra.
                    pos = rand() % COL;
                }
            }        
        }
        //sorteia outra quantidade de colunas que seram preenchidas na proxima linha.
        numpos = 1 + rand() % (NUMPOS);
    }

    //for para printar a matriz produzida.
    for (int i=0; i < LIN;i++){
        for (int j=0; j<COL; j++){
            
            //verificacao para evitar os espacos que sao lixo.
            if (esparsa[i][j].posi <= COL && esparsa[i][j].posi == j){
                printf("(%d, %.2f) ", esparsa[i][j].posi, esparsa[i][j].val);
            }else{
                //print estetico para nao imprimir o lixo ou deixar em branco.
                printf("(%d, %.2f) ", j, 0.0);
            }
        }
        puts("");
    }
}

//funcao para preencher um vetor com numeros aleatorios de um certo intervalo e printar.
void preecheVetor(){
    for (int i=0; i<COL;i++){
        vetort[i] = -9.0 + (((float) rand()) / ((float) RAND_MAX) * RMAX);
    }

    for(int i=0; i < COL; i++){
        if(i != COL-1){
            printf("(%.2f) ", vetort[i]);
        }else{
            printf("(%.2f)\n", vetort[i]);
        }
    }
}

//funcao para preencher uma matriz com numeros aleatorios de um certo intervalo e printar.
void preencheMatriz(){
    for (int i=0; i<LIN; i++){
        for (int j=0; j<COL;j++){
            matrizch[i][j] = -9.0 + (((float) rand()) / ((float) RAND_MAX) * RMAX);
        }
    }

    for (int i=0; i < LIN;i++){
        for (int j=0; j<COL; j++){
            printf("(%d, %.2f) ", j, matrizch[i][j]);
        }
        puts("");
    }
}