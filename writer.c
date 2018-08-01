#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>

int main(int argc, char **argv) {
	int id, err;
	int *mem;
	time_t rawtime;
        struct tm* timeinfo;
	int hour,min,sec;
	int flag;
	int sleepTime;
	int memSeg,lb,ub;
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
	printf("lb is %d ub is %d\n",lb,ub);
	printf("sleep time is %d\n",sleepTime);
	/* Attach the segment */
	mem = (int *) shmat(id, (void*) 0,0);
	if (*(int *) mem == -1) perror("Attachment.");
	else printf(">> Attached Shared Segment\n"); 
			// whose Mem content is: %d\n",*mem);


	/*-- Open the named semaphore --*/
        sem_t *orderSem;
	sem_t *mutexSem;
	const char* mutexSemName = "mutex";
        const char* semname = "order";
        orderSem = sem_open(semname, 0);
	mutexSem = sem_open(mutexSemName,0);
        sem_getvalue(orderSem, &writeVal);
        printf("semaphore's value is %d\n",writeVal);


        sem_wait(orderSem);
	sem_wait(mutexSem);
	sem_post(orderSem);
	/*-- CRITITAL SECTION  --*/
	/* Give it a different value */
	int j;
        for(j=lb;j<ub;j++){

		time(&rawtime);
	        timeinfo = localtime(&rawtime);
	  //      hour = timeinfo->tm_hour;
	//	char hrs[2];
		char mins[2];
		char secs[2];
	        min = timeinfo->tm_min;
	        sec = timeinfo->tm_sec;
	//	sprintf(hrs,"%d",hour);
		sprintf(mins,"%d",min);
		sprintf(secs,"%d",sec);
	        char totalTime[25];
	        printf("Min %d Sec %d\n",min,sec);
	        sprintf(totalTime,"%s%s",mins,secs);
		char mypid[10];
		int pidInt = getpid();
		sprintf(mypid,"%d",pidInt);
		printf("my pid is %s\n",mypid);
		printf("ttime is %s\n",totalTime);
		char toWrite[50];
		sprintf(toWrite,"%s%s",mypid,totalTime);
		printf("to write is %s\n",toWrite);
	        mem[j] = atoi(toWrite);
		printf(">> Changed Shared Segment ; Mem[%d] is now: %d\n",j, mem[j]);
	}
	printf("Sleeping now for %d seconds\n",sleepTime);
        sleep(sleepTime);

	sem_post(mutexSem);

	/* Detach segment */
	err = shmdt((void *) mem);
	if (err == -1) perror ("Detachment.");
	else printf(">> Detachment of Shared Segment %d\n", err);

	return 0;
}

