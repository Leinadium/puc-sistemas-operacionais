#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#define EVER ;;

void intHandler(int sinal);
void quitHandler(int sinal);

int main()
{
	void (*p)(int);
	// p = signal(SIGINT, intHandler);
	printf("Endereco do manipulador anterior %p\n", p);
	// p = signal(SIGQUIT, quitHandler);
	printf("Endereco do manipulador anterior %p\n", p);
	printf("CTRL-C desabilitado. Use CTRL-\\ para terminar\n");
	for(EVER);
}

void intHandler(int sinal) {printf("Pressionou CTRL-C (%d)\n", sinal);}
void quitHandler(int sinal) {printf("Terminando o processo!\n");exit(0);}
