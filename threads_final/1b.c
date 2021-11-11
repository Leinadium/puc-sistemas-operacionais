/*

b) Acrescente agora uma variável global inteira que é inicializada com zero e é 
incrementada e impressa na tela por cada thread. Verifique que as tarefas manipulam
a mesma variável.

Compile com a opção -lpthread

Pergunta-se:
    i:  houve concorrencia entre as threads em a. e b? Justifique.
    ii: Os valores impressos foram os esperados? Justifique.
*/

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define N 100000
#define M 100000

int variavel = 0;

void *contCrescente() {
    for (int i = 1; i <= N; i++) {
        variavel += 1;
        printf("Cres: %d\n", variavel);
    }
    pthread_exit(NULL);
}

void *contDecrescente() {
    for (int i = M; i > 0; i--) {
        variavel -= 1;
        printf("Decr: %d\n", variavel);
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, contCrescente, NULL);
    pthread_create(&thread2, NULL, contDecrescente, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    return 0;
}