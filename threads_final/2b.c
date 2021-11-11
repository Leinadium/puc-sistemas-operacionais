/*
IMPLEMENTACAO USANDO TAREFAS/THREADS

Implemente um programa que, dado um vetor de 4K posicoes inicializado com valores
inteiros, crie 4 processos para paralelizar a busca do maior valor armazenado no vetor.
O processo coordenador vai criar os 4 processos trabalhadores e consolidar a resposta
indicando o maior valor armazenado no vetor. Contabilize o tempo da execução do processo
*/

# include <pthread.h>
# include <stdio.h>
# include <unistd.h>
# include <time.h>
# include <stdlib.h>

# define QUANTIDADE_THREADS 4

const int TAM_VETOR = 10000;
const int TAM_PESQUISA = TAM_VETOR / QUANTIDADE_THREADS;

int *vetor;
int *respostas;

void *pesquisa_no_vetor(void *id) {
    int ident = (int) id;
    int ini = TAM_PESQUISA * ident;
    int fim = TAM_PESQUISA * (ident + 1);
    respostas[ident] = -1;

    for (int i = ini; i < fim; i++) {
        if (vetor[i] > respostas[ident]) {
            respostas[ident] = vetor[i];
        }
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[4];
    int t, i;
    clock_t start, end;

    vetor = (int *)malloc(sizeof(int) * TAM_VETOR);
    respostas = (int *)malloc(sizeof(int) * QUANTIDADE_THREADS);

    srand(time(NULL)); // inicializando a seed para random
    // preenchendo
    for (i = 0; i < TAM_VETOR; i++) {
        vetor[i] = rand() % 5000;
    }

    start = clock();  // iniciando o clock

    // criando as threads
    for (t = 0; t < QUANTIDADE_THREADS; t ++) {
        pthread_create(&threads[t], NULL, pesquisa_no_vetor, (void *)t);
    }

    for (t = 0; t < QUANTIDADE_THREADS; t ++) {
        pthread_join(threads[t], NULL);
    }

    // calculando a melhor resposta no vetor de respostas
    int maior_valor = respostas[0];
    for (i = 1; i < QUANTIDADE_THREADS; i ++) {
        if (respostas[i] > maior_valor) {
            maior_valor = respostas[i];
        }
    }
    // imprimindo melhor valor
    printf("Maior valor: %d\n", maior_valor);

    end = clock();
    printf("Tempo decorrido: %0.5fs\n", (double)(end - start) / CLOCKS_PER_SEC);

    free(vetor);
    free(respostas);

    return 0;
}