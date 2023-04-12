#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#define N 6    /* broj razina proriteta */

int OZNAKA_CEKANJA[N];
int PRIORITET[N];
int TEKUCI_PRIORITET;
int sig[] = {SIGUSR1, SIGUSR2, SIGCONT, SIGFPE, SIGINT};

void prekidna_rutina(int sig);
void zabrani_prekidanje();
void dozvoli_prekidanje();
void obrada_signala(int i);

void zabrani_prekidanje()
{
    for (int i = 0; i < 5; i++)
        sighold(sig[i]);
}

void dozvoli_prekidanje()
{
    for (int i = 0; i < 5; i++)
        sigrelse(sig[i]);
}

void obrada_prekida(int i)
{
    /* obrada se simulira trošenjem vremena;
    	obrada traje 5 sekundi, ispis treba biti svake sekunde */
    sigset (SIGUSR1, prekidna_rutina);
    sigset (SIGUSR2, prekidna_rutina);
    sigset (SIGCONT, prekidna_rutina);
    sigset (SIGFPE, prekidna_rutina);
    sigset (SIGINT, prekidna_rutina);

    int j;

    switch(i){

    case(1):
        printf("- P - - - -\n");
        for(j = 1; j <= 5; j++)
        {
            printf("- %d - - - -\n", j);
            sleep(2);
        }
        printf("- K - - - -\n");
        break;

    case(2):
        printf("- - P - - -\n");
        for(j = 1; j <= 5; j++)
        {
            printf("- - %d - - -\n", j);
            sleep(2);
        }
        printf("- - K - - -\n");
        break;

    case(3):
        printf("- - - P - -\n");
        for(j = 1; j <= 5; j++)
        {
            printf("- - - %d - - \n", j);
            sleep(2);
        }
        printf("- - - K - -\n");
        break;

    case(4):
        printf("- - - - P -\n");
        for(j = 1; j <= 5; j++)
        {
            printf("- - - - %d - \n", j);
            sleep(2);
        }
        printf("- - - - K - \n");
        break;

    case(5):
        printf("- - - - - P\n");
        for(j = 1; j <= 5; j++)
        {
            printf("- - - - - %d\n", j);
            sleep(2);
        }
        printf("- - - - - K\n");
        break;

    }
}

void prekidna_rutina(int sig)
{
    int x;
    int n = -1;
    
    zabrani_prekidanje();
    //odredi uzrok prekida, tj. indeks n;
    switch (sig)
    {
    case SIGUSR1:
        n = 1;
        printf("- X - - - -\n");
        break;

    case SIGUSR2:
        n = 2;
        printf("- - X - - -\n");
        break;

    case SIGCONT:
        n = 3;
        printf("- - - X - -\n");
        break;
        
    case SIGFPE: //Erroneous arithmetic operation.
        n = 4;
        printf("- - - - X -\n");
        break;

    case SIGINT:
        n = 5;
        printf("- - - - - X\n");
        break;

    }

    OZNAKA_CEKANJA[n] = 1;
    do
    {
        /* odredi signal najveceg prioriteta koji ceka na obradu */
        x = 0;
        for (int j = TEKUCI_PRIORITET + 1; j <= N; ++j)
        {
            if (OZNAKA_CEKANJA[j] != 0)
            {
                x = j;
            }
        }

        /* ako postoji signal koji ceka i prioritetniji je od trenutnog posla, idi u obradu */

        if (x > 0)
        {
            OZNAKA_CEKANJA[x] = 0;
            PRIORITET[x] = TEKUCI_PRIORITET;
            TEKUCI_PRIORITET = x;
            dozvoli_prekidanje();

            obrada_prekida(x);

            zabrani_prekidanje();

            TEKUCI_PRIORITET = PRIORITET[x];
        }
    }
    while (x > 0);

    dozvoli_prekidanje();

}

int main ( void )
{
    sigset (SIGUSR1, prekidna_rutina);
    sigset (SIGUSR2, prekidna_rutina);
    sigset (SIGCONT, prekidna_rutina);
    sigset (SIGFPE, prekidna_rutina);
    sigset (SIGINT, prekidna_rutina);

    printf("Proces obrade prekida, PID=%d\n", getpid());
    printf("G 1 2 3 4 5\n----------\n");

    /* troši vrijeme da se ima šta prekinuti - 10 s */
    for(int i = 0; i < 10; ++i)
    {
        sleep(1);
        printf("%d - - - - -\n", i);
    }

    for(int i = 10; i < 20; ++i)
    {
        sleep(1);
        printf("%d - - - - -\n", i);
    }

    printf("\nZavršio osnovni program!\n");

    return 0;
}

