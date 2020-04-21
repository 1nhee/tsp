#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int _travel(int idx, int end);
void travel(int start, int end);
void Signal_handler(int sig);
void Print_result();
void Get_Input(char *argv[]);
int factorial(int n);

#define BUFSIZE 256

int Maxlen, limit_child, count_route, per_child;
int weight[50][50];
int path[50];
int used[50];
int minpath[50];
int length = 0;
int min = -1;

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
	printf("\n\n");
}

void Signal_handler(int sig)
{
	printf("You pressed ctrl+c so that it is quit\n");
	//kill(pid, SIGTERM);
	exit(1);
}

void Print_result()
{
	//double result;

	printf("Shortest weight: %d\nShortest path is ", min);
	for (int i = 0; i < Maxlen; i++)
	{
		printf("%d ", minpath[i]);
	}
	printf("%d\nThe total number of checked routes %d\n", minpath[0], count_route);

	//result = (double)(end - start);
	//printf("Time: %f\n", result);
}

int factorial(int n)
{
	if (n == 1)
	{
		return 1;
	}
	return n * factorial(n - 1);
}

int _travel(int idx, int end)
{
	if (idx == end)
	{
		length += weight[path[(end - 1)]][path[0]];

		count_route++;

		if (min == -1 || min > length)
		{
			min = length;
			printf("%d (", length);
			for (int i = 0; i < end; i++)
			{
				minpath[i] = path[i];
				printf("%d ", path[i]);
			}
			printf("%d)\n", path[0]);
		}
		length -= weight[path[(end - 1)]][path[0]];
	}
	else
	{
		for (int i = 0; i < end; i++)
		{
			if (used[i] == 0)
			{
				path[idx] = i;
				used[i] = 1;
				length += weight[path[idx - 1]][i];
				_travel(idx + 1, end);
				length -= weight[path[idx - 1]][i];
				used[i] = 0;
			}
		}
	}
}

void travel(int start, int end)
{
	path[0] = start;
	used[start] = 1;
	_travel(start, end);
	used[start] = 0;
}

int main(int argc, char *argv[])
{

	time_t start, end;
	count_route = 0;
	int total_count, idx;

	/*
	start = time(NULL);
	end = time(NULL);
	*/
	Get_Input(argv);
	total_count = factorial((Maxlen - 1));
	per_child = total_count / limit_child;
	printf("\n%d %d : per child needs to calculte %d\n\n", total_count, limit_child, per_child);

	pid_t pid = fork();

	//parent p
	if (pid > 0) { 
    	travel(0, 3);
		Print_result();
    } 
    // If pid is 0, in child process 
    else { 
        travel(4, 7);
		Print_result();
    } 


	return 0;
}
