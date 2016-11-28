#include <stdio.h>
#include "structs_ma2.h"
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <math.h>

struct Node_ma{
	char name[12]; //name of item
	int *array;    // array of distances from all other items
	int pos;		//position in DistanceMatrix
	int visited;	//checks if node is visited to avoid duplicates
	int belongs;
};

struct List_nodes_ma{        
	Node_ma point;           //list of nodes (each node represents an item)
	List_nodes_ma *next;
};

struct List_pointers_ma{
	Node_ma *nodeptr;         //list of pointers to nodes (each pointer points to a specific node)
	List_pointers_ma *next;
};

struct ma_cluster{
	Node_ma *nodeptr;
	List_pointers_ma *points;
};

struct Dist_points{
	int x1;                        
	int x2;
	float t1;
};

List_nodes_ma* matrix_input(FILE *fd,int* final_size, int * item,int ***array){
	int items=0,i,j;
	List_nodes_ma *listn=NULL;
    int size=0;
    char bla[12];
    char c;
    c='t';        
    fscanf(fd, "%s ",bla);
	while(c!='\n')     //if c is character of new line it means that first line of file is over
    {
    	fscanf(fd, "%[^,\n]%c", bla,&c);
    	//printf(":%s\n ",bla);
        items++;
	   	List_nodes_ma *tempnod;                     //construct list filling only the name and the position of each item                
    	tempnod=malloc(sizeof(List_nodes_ma));
    	strcpy(tempnod->point.name,bla);              
    	memset(bla, 0, sizeof(bla));
    	tempnod->point.pos=items-1;
    	tempnod->point.visited=0;
    	tempnod->next=listn;
    	listn=tempnod;
	}
	*array = malloc( sizeof(int *) * items);           //initialisation of array of DistanceMatrix
	 if (*array == NULL){
        printf("ERROR: out of memory\n");
    }
	for (i=0;i<items;i++){
        (*array)[i] = malloc( sizeof(int) * items);
    }
	i=0;
	char d[4];
    for(i=0;i<items;i++){
    	for(j=0;j<items;j++)
        {
        	fscanf(fd, "%s", d);
        	if (!strcmp(d,"")) break;              //fill Distance matrix from dataset
        	(*array)[i][j]=atoi(d);
        }
    }
    List_nodes_ma *pointer=listn;
    while(pointer!=NULL){
    	pointer->point.array=malloc(items*sizeof(int));
		for(i=0;i<items;i++){	
			pointer->point.array[i]=(*array)[pointer->point.pos][i];     //fill the array of each node in list
		}
    	pointer=pointer->next;
    }
    *final_size=items;
	*item=items;
	printf("File Read with success\n");
	return listn;
}

void init_array(Node_ma ***array,List_nodes_ma *listn, int items){ 
	int i;
	(*array)=malloc(items*(sizeof(Node_ma*)));
	i=items-1;
	List_nodes_ma *pointer=listn;
	while(pointer!=NULL){
		(*array)[i]=&pointer->point;
		(*array)[i]->belongs=-1;
		pointer=pointer->next;
		i--;
	}
}

void print_array(Node_ma **array, int items){
	int i;
	for(i=0; i<items ;i++){
		printf("%s,%d\n",array[i]->name,array[i]->array[0]);
	}
}

void init_ma_cl(ma_cluster **clusters,int no_cl){
 (*clusters)=malloc(no_cl*sizeof(ma_cluster));
 printf("Clusters initialized!\n");
}

void print_clusters(ma_cluster *clusters,int no_cl){
	int i;
	for(i=0;i<no_cl;i++)
		printf("%s \n",clusters[i].nodeptr->name);
	
}

void free_list_nodes(List_nodes_ma **listn, int size){
	List_nodes_ma *templist;
	int i;
	while((*listn)!=NULL){
		templist=(*listn);
		(*listn)=(*listn)->next;
		free(templist->point.array);
		free(templist);
	}
}

void k_medoids(int k,int size,int items,Node_ma ***array,ma_cluster **clusters,int **distance_matrix){
	int centers_count=1;
	int random,*cent_pos,flag,flag1,i,l,j;
	srand(time(NULL));
	float *prob,sum,sumtemp,rand_sum,temp,min;
	float *previous_rands;
	random=rand() % items;
	cent_pos=malloc(k*sizeof( int));
	prob=malloc((items+1)*sizeof( float));
	previous_rands=malloc(k*sizeof(float));
	
	cent_pos[0]=random;
	(*clusters)[centers_count-1].nodeptr=(*array)[random];
	centers_count++;
	while(centers_count<=k){
		sum=0.0;
		for (i=0;i<items;i++){
			flag=0;
			for(l=0;l<centers_count;l++){
				if(i==cent_pos[l])
					flag=1;
			}
			if(flag==0){
				min=distance_matrix[(*array)[i]->pos][0];
				for(j=1;j<centers_count-1;j++){
					temp=distance_matrix[(*array)[i]->pos][j];
					if(temp<min){
						min=temp;
					}
				}
				prob[i]=min;
				sum+=pow(prob[i],2);  
			}
		}
		
		rand_sum = (float)rand()/(float)(RAND_MAX/sum);
		flag1=1;
		flag=0;
		while(flag==0){
			for(j=0;j<k;j++){
				if(rand_sum==previous_rands[j])
					flag1=0;
			}
			if(flag1==1){
				previous_rands[centers_count-1]=rand_sum;
				flag=1;
			}
			else
				rand_sum = (float)rand()/(float)(RAND_MAX/sum);
		}
		flag=0;
		i=0;
		sumtemp=0;
		while(i<items && flag==0){		

			flag1=0;
			for(l=0;l<centers_count;l++){
				if(i==cent_pos[l])
					flag1=1;
			}

			if(flag1==0){
				sumtemp+=pow(prob[i],2);
				if(sumtemp>=rand_sum){							//if the new temp surpasses the rand ,means we found our new center
					(*clusters)[centers_count-1].nodeptr=(*array)[i];
					cent_pos[centers_count-1]=i;
					flag=1;
				}	
			}		
			i++;
		}
			printf("sumtemp %f   pos %d  rand %f\n",sumtemp,cent_pos[centers_count-1],rand_sum);

		centers_count++;
	}
	free(cent_pos);
	free(prob);
	free(previous_rands);
}

void free_matrix_array(int ***array,int size){
	int i;
	for(i=0;i<size;i++)
		free((*array)[i]);
	free(*array);
}