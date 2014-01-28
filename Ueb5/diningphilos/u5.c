/********************************************************************************/
/* Title: Uebung 5 								*/
/* Description: Synchronisation zwischen parallel ablaufenden Threads		*/
/*                                              				*/
/* Creator:									*/
/* Matr.No: s721011 s782688							*/
/* Time of creation: 								*/
/* Time of modification:							*/
/* Compile options: gcc -Wall u5.c -o u5 					*/
/********************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#define NLOOP 10


int counter= 0;
/* shared variable */
pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;

void dawdle(int cnt) {
  int i;
  double dummy= 3.141;
  for(i=0;i<cnt*10;i++) {
    dummy= dummy*dummy/dummy;
  }
  sleep(1);
}

/******************************************************************************/
void *thread(void *args) {
  int i,val;
  int arg = *((int*)args);
  
  for (i= 0; i<NLOOP; i++) {
    pthread_mutex_lock(&mutex);
    val= counter;
    dawdle(arg);
    printf("Thread %lu: %d\n",pthread_self(),val+1);
    counter= val+1;
    pthread_mutex_unlock(&mutex);
  }
  pthread_exit(NULL); 
}

/*******************************************************************************/
int main(int argc,char **argv) {
  int arg1=1, arg2=0;

  /*counter = shmget(IPC_PRIVATE,sizeof(int),IPC_CREAT|IPC_EXCL|0666);
  if(counter<0){
        perror("Fehler beim erzeugen von Schared Memory \n");
        exit(1);
  }*/
  
  pthread_t tidA,tidB;
  
  
  if (pthread_create(&tidA,NULL,thread,(void *)&arg1) != 0) {
    fprintf (stderr, "Konnte Thread 1 nicht erzeugen\n");
    exit (EXIT_FAILURE);
  }
  if (pthread_create(&tidB,NULL,thread,(void *)&arg2) != 0) {
    fprintf (stderr, "Konnte Thread 2 nicht erzeugen\n");
    exit (EXIT_FAILURE);
  }
  
 
  
  pthread_join(tidA, NULL);		//warte auf ende des Prozesses
  pthread_join(tidB, NULL);
  pthread_mutex_destroy(&mutex);	//pthread_mutex_destroy() function shall destroy the mutex object referenced by mutex
  
  return 0;

}
