#include "structs_ham2.h"
#include "functions.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <math.h>


int main(int argc, char** argv){
	int flagd,i;
	FILE *input,*output;
	char inputstr[50];
	for(i=1; i<=argc-2; i=i+2){
		if(!strcmp(argv[i], "-d")){
				strcpy(inputstr,argv[i+1]);
				flagd=0;
		}
	}	
	if(flagd==1){
		printf("Give input file: ");
		scanf("%s", inputstr);
	}
	if ((input = fopen(inputstr, "r")) == NULL){
			printf("wrong file name\n");
			return -1;
	}
	char metric_space[100];
    char metric[100];
    int size,j;
    fscanf(input, "@metric_space %s\n", metric_space);
    if (!strcmp(metric_space, "hamming")){
		int size, items;
	    int no_cl=4;
	    List_nodes_Ham *listn;
	    listn=Hamming_input(input,&size,&items);
		Node_Ham **objects;
		init_array(&objects,listn,items);
		ham_cluster *clusters;
		init_ham_cl(&clusters,no_cl);

	    k_medoids(no_cl,size,items,&objects,&clusters);
	    printf("medoids done\n");
	    print_clusters(clusters,no_cl);
		
	}



}

