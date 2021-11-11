/*
Usando processos, escreva um programa C que implemente a troca
de mensagens entre 3 processos, um gerando mensagens 
(produtor) e outros dois processos recebendo as mensagens 
(consumidores). Os processos compartilham recursos (filas e 
demais dados). As mensagens consistem de números inteiros que são
gerados pelo produtor e armazenados em um buffer a cada 1 segundo.
Os processos consumidores devem retirar os dados da fila, um a 
cada 1 segundo e outro a cada 3 segundos. O tamnho máximo da fila
deve ser de 32 elementos e tanto o produtor como os dois consumidores
(juntos) devem produzir/consumir 128 elementos. Gere um relatório
que permite entender os processos, suas ações e o resultado de
suas açoes.
*/


# include <stdlib.h>
# include <stdio.h>

# include <unistd.h>
# include <sys/wait.h>
# include <time.h>

// para memoria compartilhada
# include <sys/shm.h>
#include <sys/stat.h>

const int MAX_FILA = 32;
const int MAX_NUMEROS = 128;
const int TEMPO_PRODUTOR = 1;
const int TEMPO_CONSUMIDOR1 = 1;
const int TEMPO_CONSUMIDOR2 = 3;


typedef struct fila {
    int *lista;
    int quantidade;
    int tamanhoMax;
} Fila;

// nao ha funcao de criacao de fila, pois ela é gerada com memoria
// compartilhada, para os processos poderem usar juntas.

int filaVazia(Fila *f) {
    if (f->quantidade == 0) {
        return 1;
    } else {
        return 0;
    }
}

int filaCheia(Fila *f) {
    if (f->quantidade == f->tamanhoMax) {
        return 1;
    } else {
        return 0;
    }
}

int filaPop(Fila *f) {
    int res;

    if (f->quantidade <= 0) {
        printf("ERRO! Tentando tirar de uma fila vazia.\n");
        exit(-1);
    }
    // pegando o primeiro da fila
    res = f->lista[0];
    // andando a fila inteira
    for (int i = 0; i < f->quantidade - 1; i ++) {
        f->lista[i] = f->lista[i + 1];
    }
    f->quantidade -= 1;
    return res;
}

void filaPush(Fila *f, int x) {
    if (f->quantidade == f->tamanhoMax) {
        printf("Erro! Tentando colocar em uma fila cheia.\n");
        exit(-2);
    }
    f->lista[f->quantidade] = x;
    f->quantidade += 1;
}


int main() {
    int segmento1, segmento2, i, status, pid;
    Fila *fila;

    // alocando a memoria compartilhada da fila
    printf("Alocando memoria.\n");
    segmento1 = shmget(9997, sizeof(Fila),
                       IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

    segmento2 = shmget(9996, sizeof(int) * MAX_FILA,
                       IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

    if (((int) segmento1 == -1) || ((int) segmento2 == -1)) {
        printf("Erro na alocacao de memoria.\n");
        exit(-3);
    }

    // anexando e criando fila
    printf("Anexando memoria.\n");
    fila = (Fila *)shmat(segmento1, 0, 0);
    fila->lista = (int *)shmat(segmento2, 0, 0);
    if (((int) fila == -1) || ((int) fila->lista == -1)) {
        printf("Erro na anexacao de memoria\n");
        exit(-4);
    }

    fila->quantidade = 0;
    fila->tamanhoMax = MAX_FILA;

    // criando os processos coletores.
    // o processo gerador sera o processo pai de ambos

    printf("Iniciando os processos.\n");
    pid = fork();
    if (pid == 0) {
        // processo filho 1
        int quantidadeColetados = 0;
        int x;
        while (quantidadeColetados < MAX_NUMEROS) {
            // pegando um numero do buffer se houver
            if (!filaVazia(fila)) {
                x = filaPop(fila);
                printf("C1: (%d) [%d]\n", x, quantidadeColetados);
                quantidadeColetados += 1;
            } else {
                printf("C1: (000) [%d]\n", quantidadeColetados);
            }
            sleep(TEMPO_CONSUMIDOR1);
        }
        exit(0);
    }
    printf("Processo consumidor 1 criado! pid: [%d]\n", pid);

    pid = fork();
    if (pid == 0) {
        // processo filho 2
        int quantidadeColetados = 0;
        int x;
        while (quantidadeColetados < MAX_NUMEROS) {
            // pegando um numero do buffer se houver
            if (!filaVazia(fila)) {
                x = filaPop(fila);
                printf("C2: (%d) [%d]\n", x, quantidadeColetados);
                quantidadeColetados += 1;
            } else {
                printf("C2: (000) [%d]\n", quantidadeColetados);
            }
            sleep(TEMPO_CONSUMIDOR2);
        }
        exit(0);
    }
    printf("Processo consumidor 2 criado! pid: [%d]\n", pid);

    // processo gerador (PAI)
    int quantidadeExecucoes = 0;
    int x;
    srand(time(NULL));
    while (quantidadeExecucoes < MAX_NUMEROS) {
        if (!filaCheia(fila)) {
            x = rand() % 100;
            filaPush(fila, x);
            quantidadeExecucoes += 1;
            printf("P: (%d) [%d]\n", x, fila->quantidade);
        }
        sleep(TEMPO_PRODUTOR);
    }

    // esperando os filhos acabarem
    wait(&status);
    wait(&status);

    // fechando a memoria compartilhada
    shmdt(fila->lista);
    shmdt(fila);
    shmctl(segmento1, IPC_RMID, 0);
    shmctl(segmento2, IPC_RMID, 0);

    return 0;
}