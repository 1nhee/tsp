#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

int least(int c);
void Get_Input(char *argv[]);
void mincost(int city);
int Maxlen,limit_child, min;

int weight[50][50],completed[50];

 
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
 
void mincost(int city)
{
	int i,ncity;
 
	completed[city]=1;
 
	printf("%d--->",city);
	ncity = least(city);
 
	if(ncity==999)
	{
		ncity=0;
		printf("%d",ncity);
		min+=weight[city][ncity];
 
		return;
	}
 
	mincost(ncity);
}
 
int least(int c)
{
	int i,nc=999;
	int min=999,kmin;
 
	for(i=0;i < Maxlen; i++)
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
		min+=kmin;
 
	return nc;
}
 
int main(int argc, char *argv[])
{
	Get_Input(argv);
 
	printf("\n\nThe Path is:\n");
	mincost(0); //passing 0 because starting vertex
 
	printf("\n\nMinimum cost is %d\n ", min);

	return 0;
}