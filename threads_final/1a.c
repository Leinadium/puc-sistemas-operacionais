/*
1) Implemente um programa que cria 2 threads:

a) Uma delas (contCrescente) contara de 1 a N=60 (com pausas
de ate T1=2 segundos) imprimindo o valor de contCrescente. A outra (contDecrescente)
de M=60 a 1 (com pausas de T2=1 segundo) imprimindo o valor de contDescrescente.

*/

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define N 60
#define T1 2
#define M 60
#define T2 1

void *contCrescente() {
    for (int i = 1; i <= N; i++) {
        printf("Cres: %d\n", i);
        sleep(T2);
    }
    pthread_exit(NULL);
}

void *contDecrescente() {
    for (int i = M; i > 0; i--) {
        printf("Decr: %d\n", i);
        sleep(T1);
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