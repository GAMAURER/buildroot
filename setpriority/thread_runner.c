#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/sched.h>

#define nthread 2
int indice;
int buffersize;
char * buffer;
char chars[]= {'A','B','C','D'};
void *run(void *data)
{
	int id = (int) data;
    printf("Thread %d Meu char e esse %c\n",id,chars[id]);
    while(indice<buffersize)}{
        buffer[indice]=chars[id];
        indice++;
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

	pthread_t thr[2];

	if (argc < 2){
		printf("usage: ./%s <execution_time>\n\n", argv[0]);

		return 0;
	}
    buffersize=atoi(argv[1]);//recebe o tamanho
	buffer = (char *) malloc(buffersize*sizeof(char));//aloca
	memset(buffer,0,buffersize);//inicializa

	
	
	//setpriority(&thr, SCHED_RR, 1);
	//sleep(timesleep);
	int i;
	for(i=0;i<nthread;i++){
		pthread_create(&thr[i], NULL,run,(void *)i);
		
	}
	for(i=0;i<nthread;i++){
		pthread_join(thr[i], NULL);
	}
	return 0;
}
