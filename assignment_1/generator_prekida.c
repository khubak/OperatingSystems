#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <values.h>
#include <time.h>

int pid = 0;

void prekidna_rutina(int sig)
{	
	/* pošalji SIGKILL procesu 'pid'*/
	kill(pid, SIGKILL);
	exit(0);
}

int main(int argc, char *argv[])
{
	srand(time(0));
	pid = atoi(argv[1]);

	sigset(SIGINT, prekidna_rutina);

	printf("%d\n", pid);
	while (1) {
		/* odspavaj 3-5 sekundi;
			rand() % ((upper - lower + 1) + lower) */
		sleep((rand() % 3) + 3);

		/* slučajno odaberi jedan signal (od 4) */
		int random_number = (rand() % 4) + 1;

		/* pošalji odabrani signal procesu 'pid' funkcijom kill*/
		switch (random_number) {
		case (1):
			kill(pid, SIGUSR1);
			break;
		case (2):
			kill(pid, SIGUSR2);
			break;
		case (3):
			kill(pid, SIGCONT);
			break;
		case (4):
			kill(pid, SIGFPE);
			break;
		}
	}

	return 0;
}
