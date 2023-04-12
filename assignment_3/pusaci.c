/*Riješiti problem pušača koristeći procese i sinkronizirati ih semaforima*/
/* pušač1 - papir, pušač2 - duhan, pušač3-šibice */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MATCHES "šibice"
#define PAPER "papir"
#define TOBACCO "duhan"

sem_t *KO, *p1, *p2, *p3, *stol_prazan;

int *sastojci_na_stolu, ID;

void print_sastojak(int i)
{
	switch(i) {
		case 0:
				printf("%s", PAPER);
				break;
		case 1:
				printf("%s", TOBACCO);
				break;
		case 2:
				printf("%s", MATCHES);
				break;
		default:
				printf("Greška u algoritmu!");
				break;
	}
}

void trgovac()
{
	while (1) {
		/* rand() % ((upper - lower + 1) + lower) */
		srand((unsigned int)time(NULL));
		
		int sastojak1 = rand() % ((2 - 0 + 1) + 0);
		int sastojak2 = rand() % ((2 - 0 + 1) + 0);
		
		while (sastojak1 == sastojak2)
			sastojak2 = rand() % ((2 - 0 + 1) + 0);
		
		sem_wait(KO);
		printf("Trgovac stavlja: ");
		print_sastojak(sastojak1);
		printf(" i ");
		print_sastojak(sastojak2);
		printf("\n");
		sastojci_na_stolu[sastojak1] = 1;
		sastojci_na_stolu[sastojak2] = 1;
		sleep(2);
		sem_post(KO);
		sleep(2);
		sem_post(p1);
		sleep(2);
		sem_post(p2);
		sleep(2);
		sem_post(p3);
		sem_wait(stol_prazan);
	}
}

void pusac(int p)
{
	/* indeksi sastojaka */
	int sastojak1, sastojak2;
	sem_t *sem;
	char ime_sastojka;
	printf("Pusac %d: ima ", p);
	print_sastojak(p);
	printf("\n");
	
	switch(p) {
		case 0:
				sastojak1 = 1;
				sastojak2 = 2;
				sem = p1;
				break;
		case 1:
				sastojak1 = 0;
				sastojak2 = 2;
				sem = p2;
				break;
		case 2:
				sastojak1 = 0;
				sastojak2 = 1;
				sem = p3;
				break;
		default:
				printf("Greška u algoritmu!\n");
				break;
	}

	while(1) {
		sem_wait(sem);
		sem_wait(KO);
		if (sastojci_na_stolu[sastojak1] == 1 && sastojci_na_stolu[sastojak2] == 1) {
			printf("Pusac %d: uzima sastojke sa stola i..", p);
			sastojci_na_stolu[sastojak1] = 0;
			sastojci_na_stolu[sastojak2] = 0;
			sleep(2);
			sem_post(KO);
			sem_post(stol_prazan);
			printf("Pusac %d pusi cigaru!\n", p);
			sleep(2);
		} else {
			sem_post(KO);
		}
	}
}



int main(void)
{	
	/* alokacija memorije */
	ID = shmget (IPC_PRIVATE, (5 * sizeof(sem_t)) + (3 * sizeof(int)), 0600);
	KO = shmat (ID, NULL, 0);
	p1 = (sem_t *)(KO + 1);
	p2 = (sem_t *)(p1 + 1);
	p3 = (sem_t *)(p2 + 1);
	stol_prazan = (sem_t *)(p3 + 1);
	sastojci_na_stolu = (int *)(stol_prazan + 1);
	
	shmctl (ID, IPC_RMID, NULL);
	/* inicijalizacija semafora */
	sem_init (KO, 1, 1);
	sem_init (p1, 1, 0);
	sem_init (p2, 1, 0);
	sem_init (p3, 1, 0);
	sem_init (stol_prazan, 1, 0);

	/* pokretanje procesa */
	for(int i = 0; i < 4; ++i) {
		switch (fork()) {
        	case 0:
                	if(i < 3) {
                		pusac(i);
                	} else {
                		trgovac();
                	}
                	break;
        	case -1:
                	printf("Nije bilo moguće stvoriti proces!");
                	exit(1);
        	}
	}

	for(int i = 0; i < 4; ++i)
		wait(NULL);

	/*uništavanje semafora*/
	sem_destroy (KO);
	sem_destroy (p1);
	sem_destroy (p2);
	sem_destroy (p3);
	sem_destroy (stol_prazan);

	/*odspajanje sa zajedničke memorije*/
	shmdt (KO);
	shmdt (p1);
	shmdt (p2);
	shmdt (p3);
	shmdt (stol_prazan);
	shmdt (sastojci_na_stolu);

	return 0;
}
