#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>

int Maxlen;
int limit_child;
int weight[50][50];
int path[50];
int used[50];
int length = 0;
int min = -1;

void _travel(int idx)
{
	int i;

	if (idx == 17)
	{
		length += m[path[16]][path[0]];
		if (min == -1 || min > length)
		{
			min = length;

			printf("%d (", length);
			for (i = 0; i < 17; i++)
				printf("%d ", path[i]);
			printf("%d)\n", path[0]);
		}
		length -= m[path[16]][path[0]];
	}
	else
	{
		for (i = 0; i < 17; i++)
		{
			if (used[i] == 0)
			{
				path[idx] = i;
				used[i] = 1;
				length += m[path[idx - 1]][i];
				_travel(idx + 1);
				length -= m[path[idx - 1]][i];
				used[i] = 0;
			}
		}
	}
}

void travel(int start)
{
	path[0] = start;
	used[start] = 1;
	_travel(1);
	used[start] = 0;
}

int main(int argc, char *argv[])
{
	char filename[100];
	char num[5];
	int pw;

	//Store input data
	strcpy(filename, argv[1]);
	limit_child = atoi(argv[2]);

	//Get Maxlen
	sscanf(filename, "%2s %2d", num, &Maxlen);

	//Read the data from .tsp
	FILE *fp = fopen(filename, "r");
	for (int i = 0; i < Maxlen; i++)
	{
		for (int j = 0; j < Maxlen; j++)
		{
			fscanf(fp, "%d", &pw);
			weight[i][j] = pw;
		}
	}
	fclose(fp);

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

	for (i = 0; i < 17; i++)
	{
		travel(i);
	}

	return 0;
}
