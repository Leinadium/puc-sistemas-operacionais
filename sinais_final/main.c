/*
Daniel Schreiber - 1910462
Gustavo Rodrigues - 1811619

Elabore um programa que simule um escalonador com tempo compartilhado.
Suponha que 4 processos chegam na fila de prontos no instante t0 (processos p1, p2, p3 e p4), nesta ordem.
O programa deve criar 4 processos filhos e alternar a execução dos filhos, a cada segundo, usando sinais.
Após 20 trocas de contexto de um processo filho, o processo pai envia um sinal a esse filho para que ele termine.
Isso acontece com os 4 processos.
Os filhos são formados por loops eternos. Somente devem ser exibidas mensagens de início de execução e término
de cada processo
*/

# define QUANTIDADE_PROCESSOS 4
# define TEMPO_PROCESSAMENTO 1
# define LIMITE_TROCAS 20
# define EVER ;;

# include <signal.h> 
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/wait.h>


// estruturas para a implementacao da fila
typedef struct processo {
    int pid;
    int qtdExecucoes;
} Processo;

typedef struct fila {
    Processo *processos;
    int quantidade;
    int tamanho_max;
} Fila;

void cria_fila(int tamanho, Fila *nova_fila) {
    nova_fila->processos = (Processo *)malloc(sizeof(Processo) * tamanho);
    if (nova_fila == NULL) {
        printf("Erro criando uma lista de processos. Abortando.\n");
        exit(-3);
    }
    nova_fila->tamanho_max = tamanho;
    nova_fila->quantidade = 0;
}

Processo pop_fila(Fila *fila) {
    if (fila->quantidade <= 0) {
        printf("Tentando tirar de uma fila vazia.\n");
        exit(-1);
    }
    Processo ret = fila->processos[0];
    for (int i = 0; i < fila->quantidade - 1; i ++) {
        fila->processos[i] = fila->processos[i + 1];
    }
    fila->quantidade -= 1;
    return ret;
}

void push_fila(Fila *fila, Processo novo_processo) {
    if (fila->quantidade == fila->tamanho_max) {
        printf("Tentando inserir em uma fila cheia.\n");
        exit(-2);
    }
    fila->processos[fila->quantidade] = novo_processo;
    fila->quantidade += 1;
    return;
}

int main() {

    // iniciando a fila
    Fila *fila;
    fila = (Fila *)malloc(sizeof(Fila));

    printf("Iniciando o programa.\n");
    cria_fila(QUANTIDADE_PROCESSOS, fila);

    printf("Criando os filhos.\n");
    int i, pid, status;
    Processo p;
    // fazendo o fork
    for (i = 0; i < QUANTIDADE_PROCESSOS; i++) {
        pid = fork();
        if (pid == 0) {
            // processo do filho, ele vai executar um loop infinito
            for (EVER);
        }
        else {
            // processo do pai
            printf("Processo %d foi criado\n", pid);
            Processo novo_processo;
            novo_processo.pid = pid;
            novo_processo.qtdExecucoes = 0;
            push_fila(fila, novo_processo);
            kill(pid, SIGSTOP);  // para o filho 
        }
    }

    // aqui eu tenho os processos na fila. Comecando o loop:
    while (fila->quantidade > 0) {
        p = pop_fila(fila);  // pegando o primeiro da fila
        // printf("CONTINUE processo %d, vezes executado %d\n", p.pid, p.qtdExecucoes);
        p.qtdExecucoes += 1;
        kill(p.pid, SIGCONT);  // deixa o processo processar
        sleep(TEMPO_PROCESSAMENTO);  // dorme um pouco
        // printf("STOPPING processo %d, vezes executado %d\n", p.pid, p.qtdExecucoes);
        kill(p.pid, SIGSTOP);  // para o processo novamente
        if (p.qtdExecucoes == LIMITE_TROCAS){
            kill(p.pid, SIGKILL);
            printf("O processo %d foi encerrado\n", p.pid);
        }
        else {
            push_fila(fila, p);
        }
    }

    // os processos estao tudo mortos
    for (i = 0; i < QUANTIDADE_PROCESSOS; i ++) {
        wait(&status);
    }

    printf("finalizando\n");
    return 0;
}