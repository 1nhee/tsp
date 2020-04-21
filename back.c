#include <iostream>
#include <cstdio>
using namespace std;

int weight[50][50];
int visit[50];
int Maxlen, min;

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
 
void tsp(int start, int now, int sum, int count){
    if(count == Maxlen && start == now){
        if(min > sum){
            m = sum;
            return;
        }
    }

    for(int i = 0; i < Maxlen; i++){
        if(weight[now][i] == 0){
            continue;
        }

        if(!check[now] && weight[now][i] > 0){
            check[now] = true;
            sum += weight[now][i];

            if(sum <= min){
                tsp(start, i, sum, count+1);
            }

            check[now] = false;
            sum -= weight[][]
        }
    }
}