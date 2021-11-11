/* minha calculadora */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void zeroHandler(int signal);

int main() {
	float valor1, valor2, resposta;
	int v1, v2;
	char operacao;
	
	signal (SIGFPE, zeroHandler);
	
	printf("Minha calculadora!\n");
	printf("Digite a operação a ser feita, separada por espacos: ");
	// scanf("%f %c %f", &valor1, &operacao, &valor2);
	scanf("%d %c %d", &v1, &operacao, &v2);
	
	switch (operacao)
	{
		case '+':
			// resposta = valor1 + valor2;
			// printf("\n%0.2f + %0.2f = %0.2f\n", valor1, valor2, resposta);
			resposta = v1 + v2;
			printf("\n%d + %d = %0.2f\n", v1, v2, resposta);
			break;
			
		case '-':
			resposta = valor1 - valor2;
			printf("\n%0.2f - %0.2f = %0.2f\n", valor1, valor2, resposta);
			// resposta = v1 - v2;
			// printf("\n%d - %d = %0.2f\n", v1, v2, resposta);
			break;
			
		case '*':
			// resposta = valor1 * valor2;
			// printf("\n%0.2f * %0.2f = %0.2f\n", valor1, valor2, resposta);
			resposta = v1 * v2;
			printf("\n%d * %d = %0.2f\n", v1, v2, resposta);
			break;
		case '/':
			// resposta = valor1 / valor2;
			// printf("\n%0.2f / %0.2f = %0.2f\n", valor1, valor2, resposta);
			resposta = v1 / v2;
			printf("\n%d / %d = %0.2f\n", v1, v2, resposta);
			break;
	}
	return 0;
}

void zeroHandler(int signal) {
	printf("Sempre tem um espertinho. Tente novamente\n");
	main();
	exit(0);
}