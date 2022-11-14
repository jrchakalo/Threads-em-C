//Autor: José Izaias da Silva Júnior
//Data: 20/09/2022
//Nome do projeto: Questão 4 - Lista de Threads

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

//Defines gerais.
#define NUMVERT 6
#define NUMARE 8
#define RMAX 15

//struct da aresta, que cosiste em vertice originario, vertice de destino e custo do caminho.
struct aresta{
    int ori, dest, cust;
};

//escopo de funcoes.
void preencheGrafo();
int raiz(int vert);
bool junta(int vert1, int vert2);

//variaveis globais.
int menorArest[NUMARE];
int vertices[NUMVERT];
struct aresta arestas[NUMARE];

//thread para calcular a menor aresta.
void *calcMen(void *threadid){
    int thid = *((int*) threadid);
    int raizori, raizdest;

    //se o valor do vertice originario for o mesmo do destino sai da thread.
    if(raiz(arestas[thid].ori) == raiz(arestas[thid].dest)){
        pthread_exit(NULL);
    }

    //raizori eh a raiz do vertice originario.
    raizori = raiz(arestas[thid].ori);
    //se a menor aresta ainda for -1 ou o custo da aresta analisada pela thread for menor que o custo da menor aresta.
    if(menorArest[raizori] == -1 || arestas[thid].cust < arestas[menorArest[raizori]].cust){
        //ela se torna a menor aresta originaria.
        menorArest[raizori] = thid;
    }
    //analogo para destinataria.
    raizdest = raiz(arestas[thid].dest);
    if(menorArest[raizdest] == -1 || arestas[thid].cust < arestas[menorArest[raizdest]].cust){
        menorArest[raizdest] = thid;
    }
}

//thread main.
int main (void){
    pthread_t th[NUMARE];
    int *thid[NUMARE];
    int t;
    //numero de vertices "ativos".
    int vertatv = NUMVERT;
    //onde armazena o resultado para obter o menor caminho.
    int menorCaminho=0;
    //funcao para randomizar as sequencias.
    srand(time(NULL));
    
    //utilizei um do-while pois as vezes a funcao preencheGrafo entrava em um loop infinito e para sair dele
    //tive que colocar um contador, quando ele saia do loop deixava um ou mais destinos igual a 0, entao
    //tratei desse erro com este do-while, forcando um grafo sem 0.
    do{
        t = 0;
        preencheGrafo();
        for(int i=1; i <= NUMARE; i++){
            if (arestas[i].dest == 0){
                t++;
            }
        }
        if(t == 0){
            break;
        }
    }while(1);
    
    //prints esteticos.
    puts("=================================================");
    puts("");
    printf("Temos %d Vertices e %d Arestas nesse Grafo.\n\n", NUMVERT, NUMARE);
    puts("=================================================");

    puts("");
    printf("As Arestas que conectao os Vertices neste Grafo sao:\n\n");
    for (int i=1; i <= NUMARE; i++){
        printf("Aresta %d, conectantdo o Vertice %d e o Vertice %d com custo %d\n", i, arestas[i].ori, arestas[i].dest, arestas[i].cust);
    }
    puts("");
    puts("=================================================");

    //enquanto tiver mais de um grupo de vertices ativo.
    while(vertatv > 1){
        //colocar todas as menores arestas de todos os vertices como -1 para iniciar.
        for (int i=1; i<=NUMVERT; i++){
            menorArest[i] = -1;
        }
        
        //utiliza uma thread por aresta para fazer os calculos das menores arestas.
        for (int i=1; i <= NUMARE; i++){
            thid[i] = (int *) malloc (sizeof(int));
            *thid[i] = i;
            pthread_create(&th[i], NULL, calcMen, (void*) thid[i]);
        }

        //espera todas as threads acabarem.
        for (int i=1; i <= NUMARE; i++){
            pthread_join(th[i], NULL);
        }

        //analisa as menores arestas.
        for(int i=1 ; i<=NUMVERT; i++){
            //se a menor aresta desse vertice nao for -1.
            if(menorArest[i] != -1){
                //e se eu conseguir juntar a menor aresta da origem com a menor aresta do destino.
                if(junta (arestas[menorArest[i]].ori, arestas[menorArest[i]].dest)){
                    //eu incremento o custo da menor aresta obtida na minha variavel de resultado.
                    menorCaminho += arestas[menorArest[i]].cust;
                    //e decremento os grupos vertices ativos, pois juntei dois.
                    vertatv--;
                }
            }
        }
    }

    //print do resultado.
    puts("");
    printf("A Arvore Geradora Minima desde Grafo eh: %d\n\n", menorCaminho);
    puts("=================================================");

    //libera os espacos alocados dinamicamente.
    for(int t=1; t <= NUMARE; t++){
        free(thid[t]);
    }

    return 0;
}

//funcao para encontrar a raiz de um vertice.
int raiz(int vert){
    //se o vertice for ele mesmo, retorna o numero dele.
    if(vertices[vert] == vert){
        return vert;
    //se nao for, procura ele de modo recursivo.
    }else{
        return vertices[vert] = raiz(vertices[vert]);
    }
}

//funcao boleana para juntar dois grupos ativos de vertices.
bool junta(int vert1, int vert2){
    //acho as raizes dos grupos.
    int v1 = raiz(vert1);
    int v2 = raiz(vert2);
    //se elas forem iguais nao tem o que juntar. retorna 0.
    if(v1 == v2){
        return 0;
    //se forem diferentes junta e retorna 1.
    }else{
        vertices[v1] = v2;
        return 1;
    }
}

//funcao para criar um grafo aleatorio.
void preencheGrafo(){
    //auxiliar iniciada com numero alto para garantir a primeira iteracao.
    int auxo=999;
    int k=0;
    int pos;
    int pes;
    int verif=0;

    //preenche o numero dos vertices com seus indices, de 1 ate o numero de vertices.
    for (int i=1; i <= NUMVERT; i++){
        vertices[i] = i;
    }

    //for para preencher as arestas.
    for (int i=1; i <= NUMARE; i++){
        //escolhe uma posicao originaria aleatoria entre 1 e NUMVERT
        pos = 1 + (rand() % NUMVERT);
        //if para caso o numero da posicao passe do NUMVERT.
        if(pos > NUMVERT){
            pos--;
        }
        //while infinito para tratar erros.
        while(1){
            //se a posicao randomica escolhida for diferente da auxiliar.
            if(pos != auxo){
                //salva a posicao na auxiliar para comparacoes futuras.
                auxo = pos;
                //escolhe um peso aleatorio entre 1 e RMAX.
                pes = 1 + (rand() % RMAX);
                
                //coloca a posicao de origem e o peso na primeira aresta.
                arestas[i].ori = pos;
                arestas[i].cust = pes;

                //randomiza a posicao para escolher o destino.
                pos = 1 + (rand() % NUMVERT);
                if(pos > NUMVERT){
                    pos--;
                }
                //segundo while infinito para tratar erros.
                while(1){
                    //condicao para caso especial, quando todas as opcoes de destino ja foram escolhidas
                    //forca a saida do while e retorna 0 no destino.
                    if(k>15){
                        return;
                    }
                    //for para saber se nao tem nenhuma outra aresta com o mesma origem e o mesmo destino.
                    for(int i=1; i<=NUMARE; i++){
                        if(arestas[i].ori == auxo){
                            if(arestas[i].dest == pos){
                                //se tiver, incrementa a variavel de verificacao, para repetir o processo de escolha.
                                verif++;
                            }
                        }
                    }
                    //for para saber se existe alguma outra aresta que tem o mesmo conjunto, so que invertido.
                    //ex : ori: 3 ; dest: 5 || ori: 5 ; dest: 3
                    //essencialmente esses dois caminhos sao iguais.
                    for(int i=1; i<=NUMARE; i++){
                        if(arestas[i].ori == pos){
                            if(arestas[i].dest == auxo){
                                //se tiver incrementa a variavel de verificacao.
                                verif++;
                            }
                        }
                    }
                    //se o possivel destino for diferente da origem e as verificacoes passarem.
                    if(pos != auxo && verif == 0){
                        //achamos o nosso destino.
                        arestas[i].dest = pos;
                        //e vamos para proxima aresta.
                        break;
                    //se nao:
                    }else{
                        //repetimos a randomizacao de uma posivel posicao.
                        pos = 1 + (rand() % NUMVERT);
                        if(pos == NUMVERT+1){
                            pos--;
                        }
                        //colocamos a variavel de verificacao em 0.
                        verif=0;
                        //e incrementamos o contador para analisar uma possivel condicao especial.
                        k++;
                    }
                }
                //quando acabar sai do primeiro while infinito.
                break;
            }else{
                //se a posicao randomica for igual, randomiza outra posicao.
                pos = 1 + (rand() % NUMVERT);
                if(pos > NUMVERT){
                    pos--;
                }
            }
        }
    }

}