#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>

/* red[0] - red studenata ispred sobe
	red[1] - red za partibrejkera */

pthread_mutex_t m1, m2, m3;
pthread_cond_t red[2];
static int N;
int brojStudenataUSustavu;
int partyBreakerUSobi = 0;
int brojStudenataUSobi = 0;
int *polje;

void *student(int K)
{
	srand((unsigned int)time(NULL));
	/* rand() % ((upper - lower + 1) + lower) 
		spavaj X milisekundi */
	usleep(1000 * (rand() % (500 - 100 + 1) + 100));
	
	while(polje[K] < 3)	{
		/* samo jedan student smije ulaziti istovremeno */
		pthread_mutex_lock (&m1);
		
		while(partyBreakerUSobi == 1)
			pthread_cond_wait(&red[0], &m1);
		
		/*polje[K] - broj ulazaka studenta u sobu */
		polje[K] = polje[K] + 1;	
		brojStudenataUSobi = brojStudenataUSobi + 1;
		printf("Student %d je ušao u sobu %d. put!\n", K + 1, polje[K]);
		/* signaliziraj partibrejkeru da može probat uć ako nije unutra */
		pthread_cond_signal (&red[1]);	
		/* drugi studenti mogu probati ući */
		pthread_mutex_unlock (&m1);	
		/* zabavi se između 1000 i 2000 milisekundi */
		usleep(1000 * (rand() % (2000 - 1000 + 1) + 1000));
		/* izađi iz sobe za zabavu */
		pthread_mutex_lock (&m2);
		brojStudenataUSobi = brojStudenataUSobi - 1;
		printf("Student %d je izašao iz sobe %d. put!\n", K + 1, polje[K]);
		/* signaliziraj partybrejkeru da je student napustio prostoriju */
		pthread_cond_signal (&red[1]);
		pthread_mutex_unlock (&m2);
		
		/* odmori se vani između 1000 i 2000 milisekundi */
		usleep(1000 * (rand() % (2000 - 1000 + 1) + 1000));
	}
	/* student odlazi doma */
	pthread_mutex_lock (&m3);
	brojStudenataUSustavu = brojStudenataUSustavu - 1;
	printf("Student %d odlazi doma! Broj studenata u sustavu je: %d!\n", K + 1, brojStudenataUSustavu);
	pthread_cond_signal (&red[1]);
	pthread_mutex_unlock (&m3);

	return NULL;
}

void *partyBreaker()
{
	srand((unsigned int)time(NULL));

	while(brojStudenataUSustavu > 0) {
		/* spavaj između 100 i 1000 milisekundi */
		usleep(1000 * (rand() % (1000 - 100 + 1) + 100));

		/* uvjet čekanja zadan u zadatku - partibrejker ne ulazi 
			u sobu ako nema bar 3 studenta u sobi */
		while(brojStudenataUSobi < 3 && brojStudenataUSustavu >= 3)	{
			pthread_cond_signal (&red[0]);
			pthread_cond_wait(&red[1], &m1);
		}

		/* uvjet čekanja za partybreakera kada je manje od 3 studenta 
			koji će još ulazit, omogućava da svi studenti koji još do 
			trenutka kad ih je ostalo 2 ili manje uđu u sobu ili odu doma */
		while(brojStudenataUSobi != brojStudenataUSustavu && brojStudenataUSustavu <= 2) {
			pthread_cond_signal (&red[0]);
			pthread_cond_wait(&red[1], &m1);
		}
		
		partyBreakerUSobi = 1;
		
		printf("Partibrejker je ušao u sobu! Broj studenata u sobi je: %d!\n", brojStudenataUSobi);

		/* izađi iz sobe ako u sobi nema više studenata */
		while(brojStudenataUSobi > 0)
			pthread_cond_wait(&red[1], &m1);
		
		partyBreakerUSobi = 0;
		printf("Partibrejker je izašao iz sobe! Broj stud u sobi: %d!\n", brojStudenataUSobi);
		
		/* dodan sleep kako se petlja ne bi još jednom zavrtila, iako su već svi
			 studenti napustili sobu i (trebali) sustav, tj brojStudenataUSustavu još nije ažuriran */
		sleep(2);
	}

	if(brojStudenataUSustavu == 0)
		printf("Partibrejker odlazi doma! Broj studenata u sustavu je: %d i party je gotov!\n", brojStudenataUSustavu);
	
	return NULL;
}

int main(void)
{	
	do {
		printf("Unesi broj partijanera! More than 3 or it didn't happen!\n");
		scanf("%d", &N);
	} while (N < 3);
	
	brojStudenataUSustavu = N;
	/* +1 za partyBrejkera */
	pthread_t t[N + 1];	

	polje = (int *)malloc(sizeof(int) * N);

	for(int i = 0; i < N; ++i)
		polje[i] = 0;
		
	/* inicijalizacija monitora i redova uvjeta */
	pthread_mutex_init (&m1, NULL);
	pthread_mutex_init (&m2, NULL);
	pthread_mutex_init (&m3, NULL);
	pthread_cond_init (&red[0], NULL);
	pthread_cond_init (&red[1], NULL);

	for (int i = 0; i < N + 1; i++)	{
		if(i < N) {
			pthread_create (&t[i], NULL, student, i);
			sleep(1);
		} else {
			pthread_create (&t[i], NULL, partyBreaker, NULL);
			sleep(1);
		}
	}

	for (int i = 0; i < N + 1; i++)
		pthread_join (t[i], NULL);
	
	pthread_mutex_destroy (&m1);
	pthread_mutex_destroy (&m2);
	pthread_mutex_destroy (&m3);
	pthread_cond_destroy (&red[0]);
	pthread_cond_destroy (&red[1]);

	return 0;
}
