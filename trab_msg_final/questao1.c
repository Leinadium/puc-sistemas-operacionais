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

/*
Resolução:
processo gerador: 
    * envia uma mensagem na fila com tipo 1, com o inteiro
    * adiciona 1 em uma variavel compartilhada
    * dorme
    * repete enquanto nao tiver enviado 32 elementos

processos consumidores:
    * tenta ler da fila algo do tipo 1
    * dorme algum tempo
    * repete enquanto uma variavel compartilhada nao for 128
*/

# include <stdio.h>
# include <stdlib.h>
# include <errno.h>

# include <unistd.h>
# include <sys/shm.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <time.h>

# include <sys/ipc.h>
# include <sys/msg.h>

const int MAX_FILA = 32;
const int MAX_EXEC = 128;
const int TIPO_MENSAGEM = 1;
const int CHAVE_FILA = 1234;
const int CHAVE_MEMORIA = 9998;
const int TEMPO_PRODUTOR = 1;
const int TEMPO_CONSUMIDOR1 = 1;
const int TEMPO_CONSUMIDOR2 = 3;

// estrutura da mensagem enviada
typedef struct msgtext {
    long mtype;
    int mtext[1];
} Mensagem;

// estrutura da mensagem recebida
typedef struct msgbuf {
    long mtype;
    int mtext[1];
} Resposta;


int main() {
    int segmento, i, status, pid, msgid;
    int *variavelCompartilhada;

    // alocando memoria compartilhada para a variavel
    segmento = shmget(CHAVE_MEMORIA, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if (segmento == -1) {
        perror("Nao foi possivel alocar memoria.");
        exit(-1);
    } else {
        printf("Memoria compartilhada alocada.\n");
    }

    // anexando a memoria compartilhada
    variavelCompartilhada = (int *)shmat(segmento, 0, 0);
    if ((int) variavelCompartilhada == -1) {
        perror("Nao foi possivel anexar a memoria compartilhada.");
        shmctl(segmento, IPC_RMID, 0);
        exit(-2);
    } else {
        printf("Memoria compartilhada anexada.\n");
    }
    *variavelCompartilhada = 0;

    // criando fila de mensagens
    msgid = msgget(CHAVE_FILA, IPC_CREAT | IPC_EXCL);
    if (msgid == -1) {
        perror("Nao foi possivel criar a fila");
        exit(-3);
    } else {
        printf("Fila de mensagens criada.\n");
    }

    // criando os processos filhos (consumidores)
    pid = fork();
    if (pid == 0) {
        // processo filho 1, consumidor
        int quantidade = 0;
        Resposta r;
        while (*variavelCompartilhada < MAX_EXEC) {
            if (msgrcv(msgid, &r, 4, 1, IPC_NOWAIT) == -1) {
                printf("[C1] lido: -- | total: %d\n", quantidade);
            } else {
                quantidade += 1;
                *variavelCompartilhada += 1;
                printf("[C1] lido: %2d | total: %d\n", r.mtext[0], quantidade);
            }
            sleep(TEMPO_CONSUMIDOR1);
        }
        printf("[C1] Total lido: %d. Fechando.\n", quantidade);
        exit(0);
    } else {
        printf("C1 criado! (pid: %d)\n", pid);
    }

    pid = fork();
    if (pid == 0) {
        // processo filho 2, consumidor
        int quantidade = 0;
        Resposta r;
        while (*variavelCompartilhada < MAX_EXEC) {
            if (msgrcv(msgid, &r, 4, 1, IPC_NOWAIT) == -1) {
                printf("[C2] lid: -- | total: %d\n", quantidade);
            } else {
                quantidade += 1;
                *variavelCompartilhada += 1;
                printf("[C2] lido: %2d | total: %d\n", r.mtext[0], quantidade);
            }
            sleep(TEMPO_CONSUMIDOR2);
        }
        printf("[C2] Total lido: %d. Fechando.\n", quantidade);
        exit(0);
    } else {
        printf("C2 criado! (pid: %d)\n", pid);
    }

    // processo pai, gerador
    srand(time(NULL));
    int quantidade = 0;
    while (quantidade < MAX_EXEC) {
        Mensagem m;
        struct msqid_ds buf;
        if (msgctl(msgid, IPC_STAT, &buf) == -1) {
            perror("Nao foi possivel obter informacoes da fila");
            exit(-4);
        }
        if (buf.msg_qnum < MAX_FILA) {
            m.mtype = 1;
            m.mtext[0] = rand() % 100;
            if (msgsnd(msgid, &m, 4, IPC_NOWAIT) == -1) {
                perror("Erro no envio de mensagem");
                exit(-5);
            } else {
                quantidade += 1;
                printf("[P ] send: %2d | total: %d\n", m.mtext[0], quantidade);
            }
        }
        sleep(TEMPO_PRODUTOR);
    }

    // fechando tudo
    wait(NULL);
    msgctl(msgid, IPC_RMID, NULL);
    shmdt(variavelCompartilhada);
    shmctl(segmento, IPC_RMID, 0);

    return 0;
}