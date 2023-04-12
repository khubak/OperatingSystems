/*  raspoređivanje podjelom vremena (RR) */
/* FIFO -Pri obradi dretve, ukoliko stigne dretva jednakog prioriteta, ona koja se izvodi
	nastavlja s izvođenjem */
/* PRIO - viši broj >> viši prioritet */
/* Ako je aktivna dretva tipa RR, onda će nakon jednog koraka (kvanta/sekunde) 
biti vraćena u red pripravnih, iza dretvi istog prioriteta, ali ispred onih manjeg prioriteta. */
/* Ako je dretva tipa FIFO onda će ona raditi do završetka ili do pojave prioritetnije dretve. */
#include <stdlib.h>
#include <stdio.h>

/* promjeniti MAX_DRETVI ukoliko se
	proširi polje nove */
#define MAX_DRETVI	6

int t = 0; //simulacija vremena (trenutno vrijeme)
int ima = MAX_DRETVI;
int containedThreads = 0;
int threadRemoved = 0;

struct dretva {
	int id; //1, 2, 3, ...
	int p;  //preostalo vrijeme rada
	int prio; //prioritet
	int rasp; //način raspoređivanja (za opcionalni zadatak)
};

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
//	{22,9,5,7,0,0},
//	{24,10,3,7,1,0},
//	{10,4,5,5,1,0},
};

/* sortira dretve prema prioritetu */
void sortByPriority(struct dretva *P[])
{
	struct dretva *help;
	
	/* usporedi svaki element sa svim ostalim */
	for (int i = 0; i < containedThreads; ++i){
		for (int j = i + 1; j < containedThreads; ++j){
			/* ukoliko je prvi element manji od drugog,
				zamjeni ih */
			if (P[i] -> prio < P[j] -> prio){
				help =  P[i];
				P[i] = P[j];
				P[j] = help;
			}
		}
	}
}

/* stavlja aktivnu dretvu na kraj MS,
	zatim ih sortira */
void switchThread(struct dretva *P[])
{
	/* pospremi adresu prve dretve */
	struct dretva *help = P[0];
	
	/* pomakni cijeli array za jedan u lijevo */
	for (int i = 0; i < containedThreads - 1; ++i)
        P[i] = P[i + 1];
 	
 	/* na zadnje mjesto umetni (nekada) prvu dretvu */
    P[containedThreads - 1] = help;

    /* sortiraj dretve po prioritetu */
    sortByPriority(P);
}

/* miče dretvu iz sustava po njenom završetku */
void removeThread(struct dretva *P[])
{
	printf(" Dretva %d zavrsila!\n", P[0]->id);
	
	/* oslobodi prostor */
	free(P[0]);

	/* smanji brojac ukupnog broja dretvi */
	ima = ima - 1;
	
	/* pomakni cijeli array za jedan u lijevo */
	for (int i = 0; i < containedThreads - 1; ++i)
		P[i] = P[i + 1];
	
	/* smanji brojac dretvi u MS */
	containedThreads = containedThreads - 1;
}

void ispis_stanja (int ispisi_zaglavlje)
{
	int i;
	
	if(ispisi_zaglavlje) {
		printf(" Kvant vremena Tq iznosi 1\n 0 oznacava FIFO+PRIO, 1 RR+PRIO\n");
		printf ("  t    AKT");
		for (i = 1;i < MAX_DRETVI; i++)
			printf("     PR%d", i);
		printf("\n");

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

/* dodaje nove dretve u MS */
void dodaj_u_pripravne(struct dretva *thread)
{
	/* umetni dretvu na kraj MS */
	P[containedThreads] = thread;
	containedThreads = containedThreads + 1;
	
	printf("%3d -- nova dretva id=%d, trajanje=%d, prio=%d, rasp=%d\n",
		t, thread->id, thread->p, thread->prio, thread->rasp);

	/* sortiraj ih po prioritetu */
	sortByPriority(P);
}

int main (void)
{
	ispis_stanja(1);
	
	while(ima > 0) {

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
		/* stackani uvjeti izbjegavaju segmentation
			fault error */
		if(containedThreads >= 2 && threadRemoved == 0)
			if(P[0] -> rasp == 1 && P[0] -> prio == P[1] -> prio)
				switchThread(P);

		threadRemoved = 0;
		ispis_stanja(0);

		for(int i = 0; i < MAX_DRETVI; ++i) {
			if (nove[i][0] == t && nove[i][5] == 0) {
				/* dodaj novu dretvu */
				nove[i][5] = 1;
				struct dretva *nd;
				nd = (struct dretva *)malloc(sizeof(struct dretva));
				nd->id = nove[i][1];
				nd->p = nove[i][2];
				nd->prio = nove[i][3];
				nd->rasp = nove[i][4];
				dodaj_u_pripravne(nd);
				ispis_stanja(0);
			}
		}

		t = t + 1;
	}
	
	return 0;
}