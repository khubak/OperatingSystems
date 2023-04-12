#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <values.h>
#include <unistd.h>

int N, M, ID, *BROJ, *ULAZ, *A, *UVJET;
static int print = 0;

void printProcessID()
{
	printf("Process ID je: %d\n", getpid());
}

void enterCriticalSection(int i)
{
	int j, max = 0;
	ULAZ[i] = 1;

	for (j = 0; j < N; j++) {
		if (BROJ[j] > max) {
			max = BROJ[j];
		}
	}
	
	BROJ[i] = max + 1;
	ULAZ[i] = 0;
	
	for (j = 0; j < N; j++) {
		while (ULAZ[j] != 0) {
			continue;
		}
		while (BROJ[j] != 0 && (BROJ[j] < BROJ[i] || (BROJ[j] == BROJ[i] && j < i))) {
			continue;
		}
	}
	sleep(2);
}

void memory_remove(int sig)
{
	printf("Vrijednost od A prije unistenja zajednickog spremnika je: %d\n", *A);
	
	shmdt((char *)ULAZ);						//otpuštanje svih dijelova spremnika
	shmdt((char *)BROJ);
	shmdt((char *)A);
	shmdt((char *)UVJET);						
	
	int d = shmctl(ID, IPC_RMID, NULL);

	if (d == 0) {
		printf("Spremnik je uspješno uništen!\n");
	}
	else {
		printf("Spremnik nije uspješno uništen!\n");
	}

	exit(1);
}

void exitCriticalSection(int i)
{
   	BROJ[i] = 0;
}

void syncProcesses(int i)
{
	printf("Stvoren proces %d\n", i+1);

	do {
		sleep(1);
		enterCriticalSection(i);
		printf("Proces %d: povecava A, trenutne vrijednosti %d, za %d\n", i+1, *A, M);
		*A = *A + M;
		++UVJET;
        exitCriticalSection(i);
		}
		
	while (*UVJET < N);
	return;
}

int main(int argc, char *argv[])
{
	int i;

	if (argc != 3) {
		printf("Program pozovite s <N> <M>!\n");
		exit(1);
	} else {
		N = atoi(argv[1]);
        M = atoi(argv[2]);

        if(print == 0) {
        	printProcessID();
        	print = 1;
        }

		ID = shmget(IPC_PRIVATE, (2 * N + 2)*sizeof(int), 0600);		//adresa zajedničkog spremnika
		if (ID == -1) {
			printf("Error while requesting memory!\n");
			exit(1);
		}

		ULAZ = (int *)shmat(ID, NULL, 0);								//ulaz[n] na početak spremnika
		BROJ = (int *)(ULAZ + N);										
		A = (int *)(BROJ + N);											
		UVJET = (int *)(A + 1);
        UVJET = 0;

		for (i = 0; i < N; i++) {
			ULAZ[i] = 0;
			BROJ[i] = 0;
		}

		sigset(SIGINT, memory_remove);		//terminal kill -SIGINT *broj procesa*
											//terminal ps -a za ispis aktivnih procesa u komandnoj liniji
		for (i = 0; i < N; i++) {
			switch (fork()) {
				case 0:
					syncProcesses(i);
					exit(0);
					break;

				case -1:
					printf("FAIL!\n");
					exit(1);
					break;

				default:
					break;
			}
		}
		while (i--) {
			wait(NULL);
		}
	}

	return 0;
}
