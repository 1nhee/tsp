#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <pthread.h>
#include <sched.h>
#include <unistd.h>

int Maxlen;
int limit_child;
int weight[50][50] ;
int path[50] ;
int used[50] ;
int length = 0 ;
int min = -1 ;
int completed[50],n,cost=0;

//Problem main func
void mincost(int city)
{
	int i, ncity;
	pthread_t p_thread[limit_child];

	completed[city]=1;
 
	printf("%d--->",city+1);
	ncity=least(city);
 
	if(ncity==999)
	{
		ncity=0;
		printf("%d",ncity+1);
		cost+=weight[city][ncity];
 
		return;
	}

	/*
	for(int i = 0; i < limit_child; i++){
		
	//The function that you will use for thread print_message_function // *arg
		thr_id = pthread_create(&p_thread[i], NULL, t_function, (void *)&a);
    if (thr_id < 0)
    {
        perror("thread create error : ");
        exit(0);
    }

		
	//thread 종료시 필요함
	pthread_join(p_thread[i], (void **)&status);
		if(status != 0){
		printf("Thread Kill error %d\n", status);
	}
	printf("Thread End %d\n", status);
	}



	pthread_mutex_lock(&m) ;
			while (turn != 1)
				pthread_cond_wait(&c, &m) ; // unlock(&m) when it goes waiting.
		pthread_mutex_unlock(&m) ;

		printf("%s\n", message) ;

		pthread_mutex_lock(&m) ;
			turn = 2 ;
			pthread_cond_signal(&c) ;
		pthread_mutex_unlock(&m) ;
	*/
 
	mincost(ncity);
}

//Problem sub-func
int least(int c)
{
	int i,nc=999;
	int min=999,kmin;
 
	for(i=0;i < n;i++)
	{
		if((weight[c][i]!=0)&&(completed[i]==0))
			if(weight[c][i]+weight[i][c] < min)
			{
				min=weight[i][0]+weight[c][i];
				kmin=weight[c][i];
				nc=i;
			}
	}
 
	if(min!=999)
		cost+=kmin;
 
	return nc;
}

int main(int argc, char *argv[]) {
	char filename[100];
    char num[5];
	int pw;

	//Store input data
	strcpy(filename, argv[1]);
	limit_child = atoi(argv[2]);
	
	//Get Maxlen
	sscanf(filename, "%2s %2d", num,&Maxlen);

	//Read the data from .tsp
	FILE * fp = fopen(filename, "r") ;
	for (int i = 0 ; i < Maxlen ; i++) {
		for (int j = 0 ; j < Maxlen ; j++) {
			fscanf(fp, "%d", &pw) ;
			weight[i][j] = pw ;
		}
	}
	fclose(fp) ;

	mincost(0);

	return 0;
}
