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

/*
Nur zur dataINformation: Die Implementierung von POSIX-Semaphoren erlaubt nun auch die Verwendung 
dieser zwischen Prozessen, wenn auf den Semaphor in einem gemeinsamen shared memory-Bereich
von beteiligten Prozessen zugegriffen werden kann. 

Der zweite Parameter von
sem_init(...),
pshared,
muss dazu ungleich 0 sein.
int sem_init(sem_t *sem, int pshared, unsigned int value);
( sehen wir uns späeter an)

--> Unter /pst/uebungen/doz/buchholz/ezs/shm/shm.c ist der Code für die System V-„wrapper“-
    Funktionen sowie ein Beispiel dafür angegeben, wie man über die Struktur eines
    Doppelpuffers Daten per Pointer oder per Array im shared memory verwalten kann.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "semaphore_.h"
#include <string.h>

#define MAX 100		// Groesse des Buffers
#define MAXLENGTH 2 	// Anzahl der Zeichen die Eigelesen werden

int segid;
int emptysem;
int fullsem;
int mutexsem;

void writeFile(char* filename, char* buff);
void readFile(char* filename, char* buff);

int main(int argc, char *argv[])
{
    char inFile[100]  = "./dataIN";
    char outFile[100] = "./dataOUT";

    pid_t child;
    int rc;
    
    //Semaphoreninitialisierung
    sem_init(&emptysem);
    //empty.val=SIZE;
    empty.val=1;
    rc = semctl(emptysem,0, SETVAL,empty);
    if(rc<0)
    {
        perror("semctl(emptysem) fail\n");
        exit(1);
    }
    
    sem_init(&fullsem);
    full.val=0;
    rc = semctl(fullsem,0, SETVAL,full);
    if(rc<0)
    {
        perror("Kann den Wert der Semaphore nicht setzen\n");
        exit(1);
    }
    
    sem_init(&mutexsem);
    mutex.val=1;
    rc = semctl(mutexsem,0, SETVAL,mutex);
    if(rc<0)
    {
        perror("Kann den Wert der Semaphore(mutex) nicht setzen\n");
        exit(1);
    }
    
    //Shared Memory initialisierung
    segid = shmget(IPC_PRIVATE,SIZE,IPC_CREAT|IPC_EXCL|SHMPERM);
    if(segid<0)
    {
        perror("Fehler beim erzeugen von Schared Memory \n");
        exit(1);
    }
    
    //Prozesserzeugung
    child = fork();
    switch(child)
    {
        case -1:
	{
            perror("fork() Fehler\n");
            exit(1);
            break;
        }
        case 0:
	{
	    //Kind Prozess gestartet
            //Shared Memory Attach
            char* buff= shmat(segid,(char *)0,0);
            if(buff<0)
	    {
                perror("Schared Memory(Kind) Fehler\n");
                exit(1);
            }
            //Schreibt Die dataOUTFile Thread-safe auf
            writeFile(outFile,buff);
            //Kind Prozess beendet
            exit(0);
            break;
        }
        default:
	{
	    //Eltern Prozess gestartet
            //Shared Memory Attach
            char* buff= shmat(segid,(char *)0,0);
            if(buff<0)
	    {
                printf("Schared Memory(Eltern) Fehler\n");
                exit(1);
            }
            //Liest Die dataINFile Thread-safe aus
            readFile(inFile,buff);
	    //Eltern Prozess beendet
	    //Warten auf Kind Prozess
            wait(&child);
            break;
        }
    }

    return 0;
}

//Liest Die dataINFile Thread-safe aus
void readFile(char* filename, char* buff)
{    
    char c,linebuf[MAX];
    FILE *infile;
    
 
    if((infile = fopen(filename, "r")) == NULL)
    {
        fprintf(stderr, "fopen(inFile) fail\n");
        exit(1);
    }
    
    /* Schleife ueber alle Datensaetze, speichern */
    while (fgets(linebuf, sizeof(linebuf), infile))
    {        
        /* Zeilenlaenge absichern */
        if (linebuf[strlen(linebuf)-1] != '\n')
	{
            fprintf(stderr,"  (>%d)!\n ", MAX);
            fprintf(stderr, "\n >%s...<\n", linebuf);
            while ((c = fgetc(infile)) != EOF)
	    {
                if (c == '\n') break; /* Rest der ¸berlangen Zeile ¸berlesen */
            }
            continue;   /* diesen Datensatz ignorieren */
        }
        acquire(emptysem);
        acquire(mutexsem);
              strncpy(buff, linebuf, sizeof(linebuf));
              printf("\t%s\n", buff);
        release(mutexsem);
        release(fullsem);
    }
    acquire(emptysem);
    acquire(mutexsem);
       buff[0] = '\0';
    release(mutexsem);
    release(fullsem);
    fclose(infile);
}

//Schreibt Die dataOUTFile Thread-safe auf
void writeFile(char* filename, char* buff)
{   
    FILE *outfile;
    int out = 0;
    
    printf(" %s\n", filename);
    if((outfile = fopen(filename, "w")) == NULL)
    {
        perror("fopen(dataOUTFile) fail\n");
        return;
    }
    
    while(buff[out] != '\0')
    {
        acquire(fullsem);
        acquire(mutexsem);
            fprintf(outfile,"%s",buff);
            printf("\t\t%s\n", buff);
        release(mutexsem);
        release(emptysem);
    }
    fclose(outfile);
}
