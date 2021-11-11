#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define EVER ;;

void childhandler(int signo);
int delay;
int main (int argc, char *argv[])
{
	pid_t pid;
	
	signal(SIGCHLD, childhandler);
	if ((pid = fork()) < 0)
	{
		fprintf(stderr, "erro ao criar filho\n");
		exit(-1);
	}
	if (pid == 0) {sleep(1);}
	else
	{
		sscanf(argv[1], "%d", &delay);
		sleep(delay);
		printf("programa excedeu o limite de %d segundos\n", delay);
		kill(pid, SIGKILL);
	}
	return 0;
}

void childhandler(int signo)
{
	int status;
	pid_t pid = wait(&status);
	printf("Child %d terminou em %d segundos com estado %d\n", pid, delay, status);
	exit(0);
}