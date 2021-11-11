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
# include <time.h>
# include <sys/wait.h>
# include <string.h>

# include <sys/ipc.h>
# include <sys/msg.h>

const int CHAVE_FILA = 1234;
const int QUANTIDADE = 64;

typedef struct msgtext {
    long mtype;
    int mtext[0];
} Mensagem;

int main() {
    int msqid;

    // criando a fila de mensagens
    msqid = msgget(CHAVE_FILA, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if (msqid == -1) {
        printf("Erro criando fila de mensagens\n");
        exit(-1);
    } else {
        printf("Fila criada:  %d", msqid);
    }

    if (fork() == 0) {
        // processo filho, que recebera as mensagens

    } else {
        // processo pai, que envia as mensagens
        srand(time(NULL));
        for (int i = 0; i < QUANTIDADE; i ++) {
            Mensagem m;
            m.mtype = 1;
            m.mtext[0] = 1;

            if (msgsnd(msqid, &m, 4, IPC_NOWAIT) == -1) {
                printf("Erro no envio da mensagem\n");
                exit(-1);
            }
        }
    }

    // fechando a fila e saindo
    wait(NULL);
    msgctl(msqid, IPC_RMID, NULL);
    return 0;
}