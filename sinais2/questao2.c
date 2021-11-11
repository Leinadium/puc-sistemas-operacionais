/*
Implemente a troca de mensagens sincrona entre 2 processos. 
Serão geradas e consumidas 64 mensagens (numeros inteiros). 
Novamente descreva no relatorio como funcionam os processos, 
suas acoes e o resultado de suas acoes.

Para troca de mensagens use <sys/msg.h>. Use as funções msgsnd() 
e msgrcv(). Estude o funcionamento do mecanismo de comunicacao 
que contempla filas, antes de implementa-lo.
*/

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <time.h>
# include <sys/wait.h>
# include <string.h>

# include <sys/ipc.h>
# include <sys/msg.h>
# include <sys/stat.h>

# include <errno.h>

const int CHAVE_FILA = 1234;
const int QUANTIDADE = 64;

typedef struct msgtext {
    long mtype;
    int mtext[1];
} Mensagem;

typedef struct msgbuf {
    long mtype;
    int mtext[1];
} Resposta;

int main() {
    int msqid;

    // criando a fila de mensagens
    msqid = msgget(CHAVE_FILA, IPC_CREAT | IPC_EXCL);
    if (msqid == -1) {
        perror("Erro criando fila de mensagens");
        exit(-1);
    } else {
        printf("Fila criada:  %d\n", msqid);
    }

    if (fork() == 0) {
        // processo filho, que recebera as mensagens
        for (int i = 0; i < QUANTIDADE; i ++) {
            Resposta r;
            // le ou espera ate chegar uma mensagem
            // daquele tipo (tipo 1 para envio, tipo 1 para receber)

            if (msgrcv(msqid, &r, 4, 1, 0) == -1) {
                perror("Erro na leitura da mensagem");
                exit(-2);
            } else {
                printf("Chegou mensagem!: %d\n", r.mtext[0]);
            }
        }
        exit(0);
    } else {
        // processo pai, que envia as mensagens
        srand(time(NULL));
        for (int i = 0; i < QUANTIDADE; i ++) {
            Mensagem m;
            m.mtype = 1;
            m.mtext[0] = rand() % 500;

            if (msgsnd(msqid, &m, 4, IPC_NOWAIT) == -1) {
                perror("Erro no envio da mensagem");
                exit(-1);
            } else {
                printf("Enviou mensagem!: %d\n", m.mtext[0]);
            }
            sleep(1);
        }
    }

    // fechando a fila e saindo
    wait(NULL);
    msgctl(msqid, IPC_RMID, NULL);
    return 0;
}