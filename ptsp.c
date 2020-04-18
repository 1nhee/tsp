#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

int Maxlen;
int weight[Maxlen][Maxlen] ;
int path[Maxlen] ;
int used[Maxlen] ;
int length = 0 ;
int min = -1 ;

void _travel(int idx) {
	int i ;

	if (idx == Maxlen) {
		length += weight[path[16]][path[0]] ;
		if (min == -1 || min > length) {
			min = length ;

			printf("%d (", length) ;
			for (i = 0 ; i < 17 ; i++) 
				printf("%d ", path[i]) ;
			printf("%d)\n", path[0]) ;	
		}
		length -= weight[path[16]][path[0]] ;
	}
	else {
		for (i = 0 ; i < 17 ; i++) {
			if (used[i] == 0) {
				path[idx] = i ;
				used[i] = 1 ;
				length += weight[path[idx-1]][i] ;
				_travel(idx+1) ;
				length -= weight[path[idx-1]][i] ;
				used[i] = 0 ;
			}
		}
	}
}

void travel(int start) {
	path[0] = start ;
	used[start] = 1 ;
	_travel(1) ;
	used[start] = 0 ;
}

int main(int argc, char *argv[]) {
	char filename[100];
    char num[5];
	int limit_child, pw;

	//Store input data
	strcpy(filename, argv[1]);
	limit_child = agrv[2];
	
	//Get Maxlen
	Maxlen = atoi(filename);

	FILE * fp = fopen(filename, "r") ;

	for (int i = 0 ; i < Maxlen ; i++) {
		for (int j = 0 ; j < Maxlen ; j++) {
			fscanf(fp, "%d", &pw) ;
			weight[i][j] = pw ;
		}
	}
	fclose(fp) ;

	for (int i = 0  ; i < Maxlen ; i++) 
		travel(i) ;

	return 0;
}
