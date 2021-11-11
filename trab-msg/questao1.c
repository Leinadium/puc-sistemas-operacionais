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

