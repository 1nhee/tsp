#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
//#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void _travel(int idx);
void travel(int start);
void Signal_handler(int sig);

#define BUFSIZE 256

int Maxlen, limit_child, count_route;
int weight[50][50];
int path[50];
int used[50];
int minpath[50];
int length = 0;
int min = 999999999;

void Print_result(time_t start, time_t end);
void Get_Input(char *argv[]);
void Signal_handler(int sig);

void Get_Input(char *argv[])
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
			fscanf(fp, "%d ", &pw);
			weight[i][j] = pw;
		}
	}
	fclose(fp);

	printf("\n\nThe cost list is:");

	for (int i = 0; i < Maxlen; i++)
	{
		printf("\n");

		for (int j = 0; j < Maxlen; j++)
			printf("\t%d", weight[i][j]);
	}
	printf("\n");
}

void Signal_handler(int sig)
{
	printf("You pressed ctrl+c so that it is quit\n");
	//kill(pid, SIGTERM);
	exit(1);
}

void Print_result(time_t start, time_t end)
{
	double result;

	printf("Shortest weight: %d\nShortest path is ", min);
	for (int i = 0; i < Maxlen; i++)
	{
		printf("%d ", minpath[i]);
	}
	printf("%d\nThe total number of checked routes %d\n", minpath[0], count_route);

	result = (double)(end - start);
	printf("Time: %f\n", result);
}

void _travel(int idx)
{
	int i;

	if (idx == Maxlen)
	{
		length += weight[path[(Maxlen - 1)]][path[0]];

		count_route++;
		if (min == -1 || min > length)
		{
			min = length;

			//printf("%d (", length);
			for (i = 0; i < Maxlen; i++)
			{
				minpath[i] = path[i];
				//printf("%d ", path[i]);
			}
			//printf("%d)\n", path[0]);
		}
		length -= weight[path[(Maxlen - 1)]][path[0]];
	}
	else
	{
		for (i = 0; i < Maxlen; i++)
		{
			if (used[i] == 0)
			{
				path[idx] = i;
				used[i] = 1;
				length += weight[path[idx - 1]][i];
				_travel(idx + 1);
				length -= weight[path[idx - 1]][i];
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

	time_t start, end;
	count_route = 0;

	start = time(NULL);
	Get_Input(argv);

	for (int i = 0; i < Maxlen; i++)
	{
		travel(i);
	}

	end = time(NULL);
	Print_result(start, end);

	return 0;
}
