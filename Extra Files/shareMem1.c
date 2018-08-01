#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>

int main(int argc, char **argv){
	int sizeOfBuffer = 20;
	time_t rawtime;
	struct tm* timeinfo;
	int id=0, err=0;
	int *mem;
	int i;
	int j;
	int hour,min,sec;
	int flag,sleepTime,memSeg,lb,ub;
	int mutexVal = 0;
	int readCount = 0;
	int writeVal = 0;
	extern char* optarg;

        while((flag = getopt(argc,argv,"l:u:d:s:")) != -1){
                switch(flag){
                        case 'd':
                                sleepTime = atoi(optarg);
                                break;
                        case 's':
                                memSeg = atoi(optarg);
                                break;
                        case 'l':
                                lb = atoi(optarg);
                                break;
                        case 'u':
                                ub = atoi(optarg);
                                break;
                }
        }


        /* Get id from command line. */
        id = memSeg;
        printf(">> The Shared Segment Id is: %d\n", id);

        /* Attach the segment */
        mem = (int *) shmat(id, (void*) 0,0);
        if (*(int *) mem == -1) perror("Attachment.");
        else printf(">> Attached Shared Segment whose Mem content is: %d\n",*mem);
	if ( *(int *) mem == -1) 
		perror("Attachment.");
//	else{
//		for(i=lb;i<ub;i++){
//			printf("Reading buffer: content at %d is: %d\n",i,mem[i]);
//		}
//	}

	/*-- Create and Open the Named Semaphores --*/
	sem_t *orderSem;
	sem_t *mutexSem;
	sem_t *readCountSem;
	const char* orderSemName = "order";
	const char* mutexSemName = "mutex";
	const char* readCountSemName = "readcount";
	mutexSem = sem_open(mutexSemName, 0); 
	orderSem = sem_open(orderSemName,0);
	readCountSem = sem_open(readCountSemName,0);


	sem_getvalue(mutexSem, &mutexVal);
	printf("Semaphore is open and init value is %d\n",mutexVal);

	sem_wait(orderSem);
	sem_wait(readCountSem);
	if(mem[20] == 0){
		sem_wait(mutexSem);
	}
	mem[20]++;
	sem_post(orderSem);
	sem_post(readCountSem);
	/*-- CRITIAL SECTION HERE  --*/
	int readLine;
	for(readLine=lb;readLine<ub;readLine++){
		printf("value of %d is: %d\n",readLine,mem[readLine]);
	}
	printf("Sleeping now for %d seconds\n",sleepTime);
        sleep(sleepTime);
	sem_wait(readCountSem);
	mem[20]--;
	if(mem[20] == 0){
		sem_post(mutexSem);
	}
	sem_post(readCountSem);

//	sem_getvalue(sem,&val);
//	printf("sem now %d\n",val);

//	printf("Sleeping now for %d seconds\n",sleepTime);
//      sleep(sleepTime);

	/* Remove segment */
	err = shmctl(id, IPC_RMID, 0);
	if (err == -1) 
		perror ("Removal.");
	else 
		printf("Just Removed Shared Segment. %d\n", (int)(err));

	return 0;
}


