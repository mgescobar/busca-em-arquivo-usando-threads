#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>

#define NUM_THREADS 4

char *files[] = {"File1_test3.txt", "File2_test3.txt", "File3_test3.txt", "File4_test3.txt"};
char *threads_nome[] = {"Thread_0", "Thread_1", "Thread_2", "Thread_3"};

// Definicao da struct de argumentos
typedef struct args {
  char *palavra;
  char *nome_arq;
  char *nome_thread;
} args;

// Funcao que recebe um nome de arquivo
// e realiza a abertura do mesmo
FILE *le_arq(char *nome_arq)
{
  FILE *arq;
  arq = fopen(nome_arq, "r");
  if (arq == NULL) {
    printf("\n[!!!] Problema em ler o arquivo %s\n", nome_arq);
    exit(0);
  }
  return arq;
}

// Funcao que recebe a estrutura de argumentos 
// chama as funcoes para abertura do arquivo
// e posteriormente realiza a busca retornando
// o nome da thread que achou a string.
void *busca_palavra(void* argumentos)
{
  args* recebidos = (args*) argumentos;

  char letra;
  clock_t inicio, fim;
  int cont = 0;
  bool flag = false;
  
  inicio = clock(); //inicia a contagem
  FILE *arquivo = le_arq(recebidos->nome_arq);
  
  while(!feof(arquivo) || cont == strlen(recebidos->palavra)) {
    letra=fgetc(arquivo);
    if(recebidos->palavra[cont] == letra) {
      cont++;
      if(cont == strlen(recebidos->palavra)) {
        fim = clock(); // termina contagem quando acha a palavra
        flag = true;
        break;
      }
    } else {
      cont = 0;
    }
  }
  
  // nao achou termina a contagem
  if(cont < strlen(recebidos->palavra)) {
    fim = clock();
  }
  
  if(flag == true) {
    printf("[OK] Palavra Encontrada - Thread: %s - ", recebidos->nome_thread); 
    printf("Arquivo: %s - ", recebidos->nome_arq);
    printf("Tempo de busca: %f \n",(double)(fim-inicio)/CLOCKS_PER_SEC);
  } else {
    printf("[!!!] Palavra Nao Encontrada - Thread: %s - ", recebidos->nome_thread); 
    printf("Arquivo: %s - ", recebidos->nome_arq);
    printf("Tempo de busca: %f \n",(double)(fim-inicio)/CLOCKS_PER_SEC);
  }
  
  fclose(arquivo);
  
  //retorna o nome da thread caso tenha encontrado a palavra
  if(flag == true) return recebidos->nome_thread;
}

int main(int argc, char *argv[])
{
  char *encontraram[NUM_THREADS];
  pthread_t threads[NUM_THREADS];

  if(argc != 2) {
    printf("\n[!!!] Argumentos inconsistentes.\n");
    exit(0); 
  }
  
  printf("[] Palavra a ser buscada: %s\n\n", argv[1]);
  for(int t = 0; t < NUM_THREADS; t++) {
    args* argumentos = (args*) malloc(sizeof(args));
    argumentos->palavra = argv[1];
    argumentos->nome_arq = files[t];
    argumentos->nome_thread = threads_nome[t];
    pthread_create(&threads[t], NULL, busca_palavra, (void*)argumentos);
  }

  for(int t=0;t<NUM_THREADS;t++) {
    pthread_join(threads[t],(void **) &encontraram[t]);
  }
  for(int t=0;t<NUM_THREADS;t++) {
    if(encontraram[t] != NULL) {
      printf("[FIM] %s: encontrou a palavra.\n", encontraram[t]);
    }
  }
}
