#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include "structs_clara_cos.h"

int main(int argc, char** argv){
	int flagd,i;
	FILE *input,*output;
	time_t t;
	srand((unsigned) time(&t));
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
    if (!strcmp(metric_space, "vector")){
		fscanf(input, "@metric %s\n", metric);
		if(!strcmp(metric,"cosine")){
			int size, items,n_sample,i,ran;
		    int no_cl=4,s=5;			
		    List_nodes *listn;
		    float total_cost,min_cost,cost;
		    cos_cluster *clusters;
		    cos_cluster *temp;
		    float *costs;
		    Node **objects;
		    Node **whole;

		    n_sample=40+2*no_cl;
		    listn=Cosine_input(input,&size,&items);	
		    ran=rand() % (items-n_sample);	
		    init_array_clara(&objects,&whole,listn,items,ran,n_sample);			
			init_cos_cl(&clusters,no_cl);
			init_cos_cl(&temp,no_cl);
			
		    for(i=0;i<s;i++){			    	
			    pam_init(no_cl,size,n_sample,&objects,&clusters);
			    printf("init done\n");
			    cos_pam_ass(clusters,no_cl,n_sample,objects,size);
			    printf("assign done\n");
			    cost=objective_value(n_sample,objects);
			    printf(" initial cost = %f\n",cost );
			    total_cost= clara_update(&clusters, no_cl, n_sample, objects, size,cost);
			    printf(" updated_cost= %f\n",total_cost );
			    print_clusters(clusters,no_cl);
			    cos_pam_ass(clusters,no_cl,items,whole,size);			//assignment with the full table    
			    cost=objective_value(items,whole);						//final cost of cluster
			    if(i==0){
			    	min_cost=cost;
			    	cmp_clusters(clusters,&temp,no_cl);
			    }
			    else{
				    if(cost<min_cost){
				    	min_cost=cost;
				    	cmp_clusters(clusters,&temp,no_cl);
				    }
				}
			}
			cos_pam_ass(temp,no_cl,items,whole,size);
			printf(" min_cost is %f\n",min_cost );
			free (clusters);
			free(temp);
		}
		

	}


}