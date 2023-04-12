#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int A;

void *increment(void *x) {
   	int temp = *((int *) x);

	for (int i = 0; i < temp; ++i) {
		++A;
	}

	pthread_exit(x);
}

int main(int argc, char *argv[])
{
	int N = atoi(argv[1]); // atoi: string u int
	int M = atoi(argv[2]);
	pthread_t thr_id[N];

	int *arg = malloc(sizeof(int));
	*arg = M;
   	A = 0;

   	for (int i = 0; i < N; i++) {
		if (pthread_create(&thr_id[i], NULL, increment, arg) != 0) {
     			printf("Greska pri stvaranju dretve!\n");
      			exit(1);
		}
	}

	for (int i = 0; i < N; i++) {
		pthread_join(thr_id[i], NULL);
	}

	printf("A = %d\n", A);

	free(arg);
   	return 0;
}
