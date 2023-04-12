#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>

static long pauza = 0L;
static long broj = 1000000001L;
static long zadnji = 1000000001L;

void print_periodically( int sig )
{
   printf("%ld\n", zadnji);
}

void set_pause(int sig){
   pauza = 1 - pauza;
}

void continue_after_pause(int sig)
{
   pauza = 0;
}

void stop(int sig)
{
   printf("%ld\n", zadnji);
   exit(0);
}

int prost ( unsigned long n )
{
   unsigned long i, max;

   if ( ( n & 1 ) == 0 ) /* je li paran? */
      return 0;

   max = sqrt (n);
   for ( i = 3; i <= max; i += 2 )
      if ( ( n % i ) == 0 )
         return 0;

   return 1; /* broj je prost! */
}


int main ()
{
   struct itimerval t;
   
   /*povezi_signale_s_funkcijama; na signal SIGTERM pozovi funkciju prekini() */
   sigset(SIGTERM, stop);
   sigset(SIGINT, set_pause);
   sigset(SIGCONT, continue_after_pause);
   
   /*postavi_periodicki_alarm; svakih 5 sekundi pozovi funkciju periodicki_ispis(); */
   sigset(SIGALRM, print_periodically);
   
   t.it_value.tv_sec = 5;  //u zadatku zadano 5
   t.it_value.tv_usec = 0;
   
   /* nakon prvog puta, periodicki sa periodom: */
   t.it_interval.tv_sec = 5;  //u zadatku zadano 5
   t.it_interval.tv_usec = 0;

   /* pokretanje sata s pridruženim slanjem signala prema "t" */
   setitimer ( ITIMER_REAL, &t, NULL );

   while (1) {
      if (prost(broj) == 1)
         zadnji = broj;

      broj++;
      
      while (pauza == 1){
         pause();
      }
   }

   return 0;
}
