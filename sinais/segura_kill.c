#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#define EVER ;;

void killHandler(int sinal);

int main()
{
	void (*p)(int);
	p = signal(SIGKILL, killHandler);
	printf("Endereco do manipulador anterior %p\n", p);
	printf("Tente matar o processo!\n");
	for(EVER);
}

void killHandler(int sinal) {printf("Impossivel de matar!");}

