#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv){
	int sizeOfBuffer = 21; //One extra because mem[20] = the readCount var
	time_t rawtime;
	struct tm* timeinfo;
	int id=0, err=0;
	int *mem;
	int i;
	int j;
	int hour,min,sec;
	int retval;

	/* Make shared memory segment */
	id = shmget(IPC_PRIVATE,10,0666); 
	if (id == -1) 
		perror ("Creation");
	else 	
		printf("Allocated Shared Memory with ID: %d\n",(int)id);

	mem = (int *) shmat(id, (void*) 0,0);
	 for (j=0;j<sizeOfBuffer;j++){
                mem[j] = 0;
        }

	/*-- Mutex Semaphore Setup --*/
	char* semname = "mutex";
	sem_t *sem=sem_open(semname, O_CREAT|O_EXCL, S_IRUSR|S_IWUSR, 1);
	
	if (sem!= SEM_FAILED){
	        printf("created new semaphore!\n");
	}
	else if (errno== EEXIST ) {
        	printf("semaphore appears to exist already!\n");
	        sem = sem_open(semname, 1);
        }

	/* end mutex semaphore set up */

	/*-- Order Semaphore Setup --*/
        char* ordersemname = "order";
        sem_t *orderSem=sem_open(ordersemname, O_CREAT|O_EXCL, S_IRUSR|S_IWUSR, 1);

        if (orderSem!= SEM_FAILED){
                printf("created new order semaphore!\n");
        }
        else if (errno== EEXIST ) {
                printf("order semaphore appears to exist already!\n");
                orderSem = sem_open(ordersemname, 1);
        }

        /* end order semaphore set up */


	/*-- Read Count Semaphore Setup --*/
        char* readcountsemname = "readcount";
        sem_t *readCountSem=sem_open(readcountsemname, O_CREAT|O_EXCL, S_IRUSR|S_IWUSR, 1);

        if (readCountSem!= SEM_FAILED){
                printf("created new read count semaphore!\n");
        }
        else if (errno== EEXIST ) {
                printf("read count semaphore appears to exist already!\n");
                readCountSem = sem_open(readcountsemname, 1);
        }

        /* end read count semaphore set up */


	printf("click to remove segment && close semaphore\n"); getchar();
	/* Remove segment */
	err = shmctl(id, IPC_RMID, 0);
	if (err == -1) 
		perror ("Removal.");
	else 
		printf("Just Removed Shared Segment. %d\n", (int)(err));

	sem_close(sem);
	sem_unlink(semname);
	printf("destroyed semaphore\n");	

	sem_close(orderSem);
        sem_unlink(ordersemname);
        printf("destroyed order semaphore\n");

	sem_close(readCountSem);
        sem_unlink(readcountsemname);
        printf("destroyed read count semaphore\n");

	return 0;
}


