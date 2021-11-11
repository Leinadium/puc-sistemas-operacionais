/* Exemplo de uso de semáforo*/

#include<sys/sem.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
    };
    
// inicializa o valor do semáforointsetSemValue(intsemId);// remove o semáforovoiddelSemValue(intsemId);// operação PintsemaforoP(intsemId);//operação VintsemaforoV(intsemId);
intmain(intargc, char* argv[]){inti;charletra = 'o';intsemId;if(argc> 1){semId= semget(8752, 1, 0666 | IPC_CREAT);setSemValue(semId);letra = 'x';sleep(2);}else{while((semId= semget(8752, 1, 0666)) < 0){putchar('.'); fflush(stdout);sleep(1);}}
for(i=0; i<10; i++){semaforoP(semId);putchar(toupper(letra)); fflush(stdout);sleep(rand() %3);putchar(letra); fflush(stdout);semaforoV(semId);sleep(rand() %2);}printf("\nProcesso%d terminou\n", getpid());if(argc> 1){sleep(10);delSemValue(semId);}return0;}
intsetSemValue(intsemId){unionsemunsemUnion;semUnion.val= 1;returnsemctl(semId, 0, SETVAL, semUnion);}voiddelSemValue(intsemId){unionsemunsemUnion;semctl(semId, 0, IPC_RMID, semUnion);}intsemaforoP(intsemId){structsembufsemB;semB.sem_num= 0;semB.sem_op= -1;semB.sem_flg= SEM_UNDO;semop(semId, &semB, 1);return0;}intsemaforoV(intsemId){structsembufsemB;semB.sem_num= 0;semB.sem_op= 1;semB.sem_flg= SEM_UNDO;semop(semId, &semB, 1);return0;}