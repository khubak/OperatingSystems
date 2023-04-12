/*  raspoređivanje podjelom vremena (RR) */
/* Tq - kvant vremena. Prioritet u ovome zadatku se ne uzima u obzir! */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_DRETVI	6

int t = 0; //simulacija vremena (trenutno vrijeme)
int Tq; //kvant vremena. Može se izmjeniti prema želji!
int ima = MAX_DRETVI;
int containedThreads = 0;
int threadRemoved = 0;

struct dretva {
	int id; //1, 2, 3, ...
	int p;  //preostalo vrijeme rada
	int prio; //prioritet
	int rasp; //način raspoređivanja (za opcionalni zadatak)
};
/* hipotetski, moguće je da sve dretve, iako različitih prioriteta, stignu u istom trenutku */

/* podaci o događajima pojave novih poslova - dretvi */
struct dretva *P[MAX_DRETVI]; //red pripravnih dretvi, P[0] = aktivna dretva
int nove[MAX_DRETVI][6] =
{
	/* trenutak dolaska, id, p, prio, rasp (prio i rasp se koriste ovisno o rasporedivacu), obrađena */
	{1,3,5,3,1,0}, /* rasp = 0 => PRIO+FIFO; 1 => PRIO+RR */
	{3,5,6,5,1,0},
	{7,2,3,5,0,0},
	{12,1,5,3,0,0},
	{20,6,3,6,1,0},
	{20,7,4,7,1,0},
//	{4,1,5,2,1,0},
//	{6,2,5,4,1,0},
//	{10,4,5,5,1,0},
};

void ispis_stanja (int ispisi_zaglavlje)
{
	int i;

	if(ispisi_zaglavlje) {
		printf ("  t    AKT");
		for (i = 1;i < MAX_DRETVI; i++)
			printf( "     PR%d", i );
		printf ( "\n" );

		return;
	}

	printf("%3d ", t);
	for (i = 0;i < MAX_DRETVI; i++)
		if (i < containedThreads)
			printf("  %d/%d/%d ",
				 P[i]->id, P[i]->prio, P[i]->p);
		else
			printf("  -/-/- ");
	printf("\n");
}

/* stavlja aktivnu dretvu na kraj MS, aktivira iducu */
void switchThread(struct dretva *P[])
{
	/* pospremi adresu prve dretve */
	struct dretva *help = P[0];
	
	/* pomakni cijeli array za jedan u lijevo */
	for (int i = 0; i < containedThreads - 1; ++i)
        P[i] = P[i + 1];
 	
 	/* na zadnje mjesto umetni (nekada) prvu dretvu */
    P[containedThreads - 1] = help;
}

/* miče dretvu iz sustava po njenom završetku */
void removeThread(struct dretva *P[])
{
	printf(" Dretva %d zavrsila\n", P[0]->id);
	
	/* oslobodi prostor maknute dretve*/
	free(P[0]);
	
	/* smanji brojac ukupnog broja dretvi */
	ima = ima - 1;
	
	/* pomakni cijeli array za jedan u lijevo */
	for (int i = 0; i < containedThreads - 1; ++i)
		P[i] = P[i + 1];
	
	/* smanji brojac dretvi u MS */
	containedThreads = containedThreads - 1;
}

/* dodaje nove dretve na kraj MS */
void dodaj_u_pripravne(struct dretva *thread)
{
	P[containedThreads] = thread;
	containedThreads = containedThreads + 1;
	
	printf("%3d -- nova dretva id=%d, p=%d\n",
		t, thread->id, thread->p);

	ispis_stanja(0);
}

int main (void)
{
	printf("Unesite kvant Tq: ");
	scanf("%d", &Tq);
	ispis_stanja(1);
		
	while(ima > 0) {
		/* u trenutku t, prvo obavi sve potrebne akcije,
			zatim dodaj nove dretve ak ih ima */
		
		/* ima bar jedna dretva */
		if(P[0] != NULL) {
			P[0] -> p = (P[0]->p) - 1;
			/* je li dretva gotova? ima--, makni, free */
			if(P[0] -> p == 0){
				removeThread(P);
				threadRemoved = 1;
			}
		}

		/* ukoliko je dretva maknuta, nema potrebe za
			dodatnim prebacivanjem */
		if(t % Tq == 0 && threadRemoved == 0)
			switchThread(P);
		
		threadRemoved = 0;
		ispis_stanja(0);

		for(int i = 0; i < MAX_DRETVI; ++i) {
			if (nove[i][0] <= t && nove[i][5] == 0) {
				/* dodaj novu dretvu */
				nove[i][5] = 1;
				struct dretva *nd;
				nd = (struct dretva *)malloc(sizeof(struct dretva));
				nd->id = nove[i][1];
				nd->p = nove[i][2];
				nd->prio = nove[i][3];
				dodaj_u_pripravne(nd);
			}
		}
		
		t = t + 1;
		
		sleep(1);
	}
	
	return 0;
}