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

#include "semaphore_.h"

void sem_init(int *semaphore)
{
    *semaphore = semget(IPC_PRIVATE,1,SEMPERM|IPC_CREAT|IPC_EXCL);
    //printf("sem = %d\n",*semaphore);
    if(*semaphore<0)
    {
        perror("semget() fail\n");
        exit(1);
    }
}

void sem_rm(int *semaphore)
{
    int rc;
    rc=semctl(*semaphore, 0,IPC_RMID,NULL);
    if(rc<0)
    {
        perror("semctl() fail\n");
        exit(1);
    }
}

void acquire(int sem_id)
{
    struct sembuf p_buf;
    p_buf.sem_num=0;
    p_buf.sem_op=-1;
    p_buf.sem_flg=SEM_UNDO;
    if(semop(sem_id, &p_buf,1)<0)
    {
        perror("semop() p fail\n");
        exit(1);
    }
}

void release(int sem_id)
{
    struct sembuf v_buf;
    v_buf.sem_num=0;
    v_buf.sem_op=1;
    v_buf.sem_flg=SEM_UNDO; 
    if(semop(sem_id, &v_buf,1)<0)
    {
        perror("semop() v fail\n");
        exit(1);
    }  
}
