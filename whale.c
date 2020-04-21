#include <stdio.h>
#include <time.h>
#include <stdbool.h> 
#include <string.h>
#include <stdlib.h>
 
int Maxlen, limit_child; 
int cost[50][50];
bool visited[50];
int lowCost = 99999999;

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
        visited[i] = false;
		for (int j = 0; j < Maxlen; j++)
		{
			fscanf(fp, "%d ", &pw);
			cost[i][j] = pw;
		}
	}
	fclose(fp);

	printf("\n\nThe cost list is:");

	for (int i = 0; i < Maxlen; i++)
	{
		printf("\n");

		for (int j = 0; j < Maxlen; j++)
			printf("\t%d", cost[i][j]);
	}
	printf("\n\n");
}
 
void tsp(int node, int costSum, int count) {
    visited[node] = true;
    if (count == Maxlen) {
        lowCost = (lowCost < costSum ? lowCost : costSum);
        visited[node] = false;
        return;
    }
 
    for(int i=0;i<Maxlen;i++){
        if(!visited[i] && cost[node][i] != 0){
            if (costSum + cost[node][i] < lowCost) { 
                tsp(i, costSum + cost[node][i], count + 1);
            }
        }
    }
    visited[node] = false;
}
 
int main(int argc, char *argv[]){

    time_t start, end;
  
    Get_Input(argv);

    for (int i = 0; i < Maxlen; i++)
        tsp(i, 0, 1);
     
    printf("%d\n", lowCost);

    end = time(NULL);
	//Print_result(start, end);

    return 0;
}
