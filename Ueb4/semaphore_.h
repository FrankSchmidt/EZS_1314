/****************************************************************************************/
/* Title: Uebung 4 									*/
/* Description: Prozesskommunikation zwischen parallel laufenden Prozessen		*/
/*                                              					*/
/* Creator:										*/
/* Matr.No: s721011 s782688								*/
/* Time of creation: 									*/
/* Time of modification:								*/
/* Compile options: gcc -Wall u4.c -o u4						*/
/****************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SEMPERM 0666
#define SHMPERM 0666
#define SIZE 256    //Größe shared Memory
#define MAXLL 80    //Lese- und Schreibpuffer Daten

void sem_init(int *semaphore);
void sem_rm(int *semaphore);
void infile(char* filename, char* buff);
void outfile(char* filename, char* buff);
void acquire(int sem_id);
void release(int sem_id);

//Steuerunions
union
{
        int val;
        struct semid_ds *buf;
        ushort *array;
}empty,full,mutex;



