#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

void _travel(int idx);
void travel(int start);
void Signal_handler(int sig);

int Maxlen;
int limit_child;
int weight[50][50];
int path[50];
int used[50];
int minpath[50];
int length = 0;
int count_route;
int min = 999999999;

void Signal_handler(int sig)
{
	printf("You pressed ctrl+c so that it is quit\n");
    //pass result
    exit(1);
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

int main(){

    signal(SIGINT, Signal_handler);

    return 0;
}