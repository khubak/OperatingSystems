#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <values.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int shmpar;
int *lok;

void destroyContainer(int sig)
{
   /* oslobađanje zajedničke memorije */
   shmdt((char *) lok);
   int d = shmctl(shmpar, IPC_RMID, NULL);

   if(d == 0){
    printf("Spremnik je uspješno uništen!\n");
   } else {
    printf("Spremnik nije uspješno uništen!\n");
   }

   exit(0);
}

void proces(int M)
{
    (*lok) = *lok + M;
    return;
}

int main(int argc, char *argv[])
{
    int i;
    int N = atoi(argv[1]);
    int M = atoi(argv[2]);

    //rezervacija memorije
    shmpar = shmget(IPC_PRIVATE, sizeof(int), 0600);
    lok = (int *)shmat(shmpar, NULL, 0);
    sigset(SIGKILL, destroyContainer);
    *(lok) = 0;

    //forkanje
    for (i = 0; i < N; i++) {
        switch (fork()) {
        case 0:
                proces(M);
                exit(0);
        case -1:
                printf("Nije bilo moguce stvoriti proces!");
                exit(1);
        }
    }
    while (i--) {
        wait (NULL);
    }

    //oslobadanje memorije i print
    printf("%d\n", *lok);

    return 0;
}
