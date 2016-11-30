#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include "structs.h"
#include "structs_cos.h"
#include "structs_ham.h"
#include "structs_ma.h"
#include "functions.h"
#define W 300
int main(int argc, char** argv){
	srand(time(NULL));
	FILE *input, *config, *output;
	int flagd=1, flagq=1, flago=1,i,com=0;
	if (argc!=7 && argc!=8){
		printf("something is wrong with the arguments\n");
		return 0;
	}
	char inputstr[50], conf[50], outputstr[50];
	for(i=1; i<=argc-2; i=i+2){
		if(!strcmp(argv[i], "-d")){
				strcpy(inputstr,argv[i+1]);
				flagd=0;
		}	
		else if(!strcmp(argv[i], "-c")){
			strcpy(conf,argv[i+1]);
			flagq=0;
		}
		else if(!strcmp(argv[i], "-o")){
			strcpy(outputstr,argv[i+1]);
			flago=0;
		}
		else{
			printf("Wrong arguments\n");
			return 0;
		}
	}
	if(argc==8){
		if(!strcmp(argv[7], "-complete")){
			com=1;
		}
	} 
	 if ((input = fopen(inputstr, "r")) == NULL){
     		printf("wrong file name\n");
     		return -1;
		}
	 if ((config = fopen(conf, "r")) == NULL){
     		printf("wrong file name\n");
     		return -1;
	}
	 if ((output = fopen(outputstr, "w")) == NULL){
     		printf("wrong file name\n");
     		return -1;
	}
	char metric_space[100];
    char metric[100];
    int size,j,o,items,no_cl,Q,s,co,L,k;
    co=configuration(config,&Q,&k,&L,&no_cl,&s);         //read configuration file
	if(co==-1) return -1;
	printf("L= %d\nk= %d\nQ= %d\ns= %d\nclusters= %d\n",L,k,Q,s,no_cl);
    fscanf(input, "@metric_space %s\n", metric_space);
    if (!strcmp(metric_space, "vector")){
		fscanf(input, "@metric %s\n", metric);
		if(!strcmp(metric,"euclidean")){
			euc_main(input,output,k,no_cl,Q,s,L,W,com);
		}
		else{                                                 
			cos_main(input,output,k,no_cl,Q,s,L,com);
   		 }
   }
	else if(!strcmp(metric_space,"hamming")){
		ham_main(input,output,k,no_cl,Q,s,L,com);
	}	
	else if(!strcmp(metric_space,"matrix")){
		ma_main(input,output,k,no_cl,Q,s,L,com);
	}
}
