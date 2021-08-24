#include<stdlib.h>
#include<stdbool.h>
#include <pthread.h>	/* for pthread functions */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include<semaphore.h>
#define NUM_THREADS 5

#define BUFFER_SIZE 5		//defining my buffer max size
int buffer[BUFFER_SIZE];	//I will be using an integer buffer

sem_t empty;		//Initializing my semaphores empty and full
sem_t full;
pthread_mutex_t mutex;		//Initializing my mutex locks

//These long int arrays will hold my thread id's for displaying later
long int pro_id[BUFFER_SIZE], con_id[BUFFER_SIZE];		

//My function prototypes for insert, remove, consumer, and producer
void insert_item(long int);
void remove_item(long int);
void *consumer(void *param);
void *producer(void *param);
//The in and out are for looping inside my buffer array, the inCount and outCount are 
//keeping track of how many items have been produced or removed
int in=0, out=0, inCount=0,outCount=0;


main(int argc, char *argv[])
{
	int sleepTime, cons, prod;		//For keeping track of how much sleep, producer/consumers
	int i, a[10]={1,2,3,4,5,6,7,8,9,10};	//This was used for indexing when I create the threads
	
	sleepTime = atoi(argv[1]);
	prod = atoi(argv[2]);
	cons = atoi(argv[3]);
	pid_t pid;
	printf("Main thread beginning\n");
	fflush(stdout);
	printf("Number of Producers = %d  Number of Consumers = %d\n",prod,cons);
	printf("Main thread sleeping for %d seconds\n\n",sleepTime);
	fflush(stdout);
	
	pthread_t pro[prod], con[cons];	//declaring prod # producer and cons # consumer threads
	pthread_mutex_init(&mutex,NULL);	//initializing mutex for threads
	sem_init(&empty,0,BUFFER_SIZE);	//initializing semaphore with max as buffersize 
	sem_init(&full,0,0);
	
      	//I used a forking method where the parent sleeps and wakes up to terminate the threads
      	//The child will be producing and consuming until the parent wakes up
      	pid = fork();	
      		        
       if(pid<0){
       	fprintf(stderr, "Fork Failed");
       }else if(pid == 0){		//Child process
       	//These for loops are for creating producer and consumer threads and
       	//recording their id's
       	for(int i=0; i<prod;i++){
			pthread_create(&pro[i],NULL,(void*)producer,(void*)&a[i]);
			printf("***Creating producer thread with id %lu***\n",pro[i]);
			pro_id[i]=pro[i];
		}
		for(int i=0; i<cons;i++){
			pthread_create(&con[i],NULL,(void*)consumer,(void*)&a[i]);
			printf("***Creating consumer thread with id %lu***\n",con[i]);
			con_id[i]=con[i];
		}
		//These for loops are for joining my consumer and producer threads
		for(int i=0; i<prod;i++){
			pthread_join(pro[i],NULL);
		}
		for(int i=0; i<cons;i++){
			pthread_join(con[i],NULL);
		}
			
		wait(NULL);	//For zombie prevention of child
       	exit(0);	//This exit is for the child process to terminate
       	}else{
       		//This is the parent process, it just sleeps then wakes up to end the program
       		sleep(sleepTime);
       		//Ending all threads and program
       		kill(pid,SIGKILL);	//To make sure the child is dead
			printf("\nMain thread exiting.\n\n");
			exit(0);
      		}
}

void remove_item(long int id){
	//This function removes the item only when it can enter it's critical section
	//Since items are never 0, and empty item will be represented with 0
	int item;
	sem_wait(&full);
	pthread_mutex_lock(&mutex);
	item = buffer[out];
	buffer[out]=0;
	printf("Consumer thread %lu removed value %d\n",id,item);
	printf("Consumer %lu: Removed Item %d from index %d\n",id,item,out);
	fflush(stdout);
	out = (out+1)%BUFFER_SIZE;
	//outCount++;		//Unnecessary counter, was used for debugging
	pthread_mutex_unlock(&mutex);
	sem_post(&empty);
	return;
}
void insert_item(long int id){
	//This function produces an item but then waits to see if it can enter
	//the critical section to insert the item
	int item;
	item = (rand()%100+1);
	printf("Producer %lu produced %d item\n",id,item);
	sem_wait(&empty);
	pthread_mutex_lock(&mutex);
	buffer[in] = item;
	printf("Producer %lu: Inserted item %d at index %d\n",id,buffer[in],in);
	fflush(stdout);
	in = (in+1)%BUFFER_SIZE;
	//inCount++;		//Unnecessary counter, was used for debugging
	pthread_mutex_unlock(&mutex);
	sem_post(&full);
	return;
}

void *producer(void *pno){
	int time = (rand()%3+1);
	//This producer function will run until the parent wakes up and terminates all the threads
	while(1){
		printf("Producer thread %lu sleeping for %d seconds\n",pro_id[*((int*)pno)-1],time);
		fflush(stdout);
		sleep(time);
		insert_item(pro_id[*((int*)pno)-1]);
		
		//This section is for displaying the contents of the buffer
		printf("\n");
		for(int i =0; i<BUFFER_SIZE;i++){
			if(buffer[i]==0){
				printf("[empty]");
			}else{
				printf("[%d]",buffer[i]);
			}
		}
		printf(" in = %d, out = %d\n\n",in, out);
		fflush(stdout);
		time = (rand()%3+1);
	}
}
void *consumer(void *cno){
	int time=(rand()%3+1);
	//Similarly to the producer function, this will run until the parent terminates the
	//threads
	while(1){
		printf("Consumer thread %lu sleeping for %d seconds\n",con_id[*((int*)cno)-1],time);
		sleep(time);
		remove_item(con_id[*((int*)cno)-1]);
		
		//Displays contents of buffer
		printf("\n");
		for(int i =0; i<BUFFER_SIZE;i++){
			if(buffer[i]==0){
				printf("[empty]");
			}else{
				printf("[%d]",buffer[i]);
			}
		}
		printf(" in = %d, out = %d\n\n",in, out);
		fflush(stdout);
		time=(rand()%3+1);
	}
	
}
