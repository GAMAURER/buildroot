#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/sched.h>
#include <semaphore.h>

int nthread;
int indice;
int politica;
int prioridade;
int buffersize;
char * buffer;
char chars[]= {'A','B','C','D'};
sem_t mutex;
void *run(void *data)
{
    
	int id = (int) data;
    printf("Thread %d Meu char e esse %c\n",id,chars[id]);
    while(indice<buffersize){
        //inic secao critica
        sem_wait(&mutex);
        buffer[indice]=chars[id];
        indice++;
        //finaliza sessao critica
        sem_post(&mutex);
    }
    return 0;
}

void print_sched(int policy)
{
	int priority_min, priority_max;

	switch(policy){
		case SCHED_DEADLINE:
			printf("SCHED_DEADLINE");
			break;
		case SCHED_FIFO:
			printf("SCHED_FIFO");
			break;
		case SCHED_RR:
			printf("SCHED_RR");
			break;
		case SCHED_NORMAL:
			printf("SCHED_OTHER");
			break;
		case SCHED_BATCH:
			printf("SCHED_BATCH");
			break;
		case SCHED_IDLE:
			printf("SCHED_IDLE");
			break;
		default:
			printf("unknown\n");
	}
	priority_min = sched_get_priority_min(policy);
	priority_max = sched_get_priority_max(policy);
	printf(" PRI_MIN: %d PRI_MAX: %d\n", priority_min, priority_max);
}

int setpriority(pthread_t *thr, int newpolicy, int newpriority)
{
	int policy, ret;
	struct sched_param param;

	if (newpriority > sched_get_priority_max(newpolicy) || newpriority < sched_get_priority_min(newpolicy)){
		printf("Invalid priority: MIN: %d, MAX: %d", sched_get_priority_min(newpolicy), sched_get_priority_max(newpolicy));

		return -1;
	}

	pthread_getschedparam(*thr, &policy, &param);
	printf("current: ");
	print_sched(policy);

	param.sched_priority = newpriority;
	ret = pthread_setschedparam(*thr, newpolicy, &param);
	if (ret != 0)
		perror("perror(): ");

	pthread_getschedparam(*thr, &policy, &param);
	printf("new: ");
	print_sched(policy);

	return 0;
}

int main(int argc, char **argv)
{
    sem_init(&mutex, 0, 1);//inicializa o mutex
	pthread_t thr[nthread];

	if (argc < 5){
		printf("usage: ./%s <nthread<=4> <execution_time>\n\n", argv[0]);

		return 0;
	}
    nthread=atoi(argv[1]);
    buffersize=atoi(argv[2]);
    //recebe o tamanho
    politica=atoi(argv[3]);
    prioridade=atoi(argv[4]);
	buffer = (char *) malloc(buffersize*sizeof(char));//aloca
	memset(buffer,0,buffersize);//inicializa

	
	
	//setpriority(&thr, SCHED_RR, 1);
	//sleep(timesleep);
	int i;
	for(i=0;i<nthread;i++){
	    
		pthread_create(&thr[i], NULL,run,(void *)i);
		setpriority(&thr[i], politica, prioridade);
		
	}
	for(i=0;i<nthread;i++){
		pthread_join(thr[i], NULL);
	}
	
	
    i=0;
    char curr;
    int cont[] = {0,0,0,0};
    while(i<indice){
        if(buffer[i]!=curr){
            cont[buffer[i]-65]+=1;
            printf("%c",buffer[i]);            
        }
        curr=buffer[i];
        i++;
    }
    printf("\n\nA=%d\nB=%d\nC=%d\nD=%d\n",cont[0],cont[1],cont[2],cont[3]);
    //printf("%s \n", buffer);
	return 0;
}
