/*
Implemente um programa que, dado um vetor de 4K posicoes inicializado com valores
inteiros, crie 4 processos para paralelizar a busca do maior valor armazenado no vetor.
O processo coordenador vai criar os 4 processos trabalhadores e consolidar a resposta
indicando o maior valor armazenado no vetor. Contabilize o tempo da execução do processo
*/

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/wait.h>
# include <time.h>

#include <sys/shm.h>
#include <sys/stat.h>

const int TAM_VETOR = 40000;
const int TAM_PESQUISA = TAM_VETOR / 4;

int main() {
    int segmento1, segmento2, i, status;
    int *vetor, *respostas;
    clock_t start, end;

    start = clock();  // iniciando o clock

    // alocando a memoria compartilhada
    printf("Alocando memoria compartilhada.\n");
    segmento1 = shmget(9999, sizeof(int) * TAM_VETOR, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    segmento2 = shmget(9998, sizeof(int) * 4, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    // verificando erro
    if (((int) segmento1 == -1) || ((int) segmento2 == -1)) {
        printf("Erro na alocacao de memoria\n");
        exit(-1);
    }

    // anexando
    printf("Anexando memoria compartilhada.\n");
    vetor = (int *)shmat(segmento1, 0, 0);
    respostas = (int *)shmat(segmento2, 0, 0);
    // verificando erro
    if (((int) vetor == -1) || ((int) respostas == -1)) {
        printf("Erro na anexacao de memoria\n");
        exit(-2);
    }

    printf("Preenchendo vetor\n");
    srand(time(NULL)); // inicializando a seed para random
    // preenchendo
    for (i = 0; i < TAM_VETOR; i++) {
        vetor[i] = rand() % 5000;
    }

    // iniciando cada processo
    for (i = 0; i < 4; i++)
    {
        if (fork() == 0)
        {
            // dentro de um processo filho.
            // area para pesquisa: TAM_PESQUISA*i ate TAM_PESQUISA*(i+1)
            int maior_valor = -1; // qualquer valor menor que 0
            for (int j = TAM_PESQUISA * i; j <= TAM_PESQUISA * (i + 1); j++) {
                if (vetor[j] > maior_valor) {maior_valor = vetor[j];}
            }
            respostas[i] = maior_valor;
            // shmdt(vetor);
            // shmdt(respostas);
            exit(0);
        }
    }
    // pai espera o filho
    for (i = 0; i < 4; i ++) {
        wait(&status);
    }

    // restante do processo do pai
    int maior_valor = respostas[0];
    for (i = 1; i < 4; i ++) {
        if (respostas[i] > maior_valor) {
            maior_valor = respostas[i];
        }
    }
    // imprimindo melhor valor
    printf("Maior valor: %d\n", maior_valor);

    end = clock();
    printf("Tempo decorrido: %0.5fs\n", (double)(end - start) / CLOCKS_PER_SEC);

    // liberando memoria
    shmdt(vetor);
    shmdt(respostas);
    shmctl(segmento1, IPC_RMID, 0);
    shmctl(segmento2, IPC_RMID, 0);
    exit(0);
}