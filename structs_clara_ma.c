#include <stdio.h>
#include "structs_clara_ma.h"
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
	int center;
	int center2;
	float dist_center;
	float dist_center2;	
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

void free_matrix_array(int ***array,int size){
	int i;
	for(i=0;i<size;i++)
		free((*array)[i]);
	free(*array);
}

void init_array_clara(Node_ma ***array ,Node_ma ***array1,List_nodes_ma *listn, int items,int rand,int sample){ 
	int i;
	(*array)=malloc(sample*(sizeof(Node_ma*)));				//sample table
	(*array1)=malloc(items*(sizeof(Node_ma*)));				//full table
	i=items-1;
	List_nodes_ma *pointer=listn;
	while(pointer!=NULL){
		if(rand+sample>i && rand<=i){
			(*array)[i-rand]=&pointer->point;
			(*array)[i-rand]->center=-1;
		}
		(*array1)[i]=&pointer->point;
		(*array1)[i]->center=-1;


		pointer=pointer->next;
		i--;
	}
}

void pam_init(int k,int size,int items,Node_ma ***array,ma_cluster **clusters){
	int centers_count=0;
	int random,flag,flag1,i,l,j;
	float *prob,sum,sumtemp,rand_sum,temp,min;
	
	prob=malloc(k*sizeof( float));								//where the previous rands are				
	random=rand() % items;
	prob[centers_count]=random;
	(*clusters)[centers_count].nodeptr=(*array)[random];		//initialize randomly the centroids
	centers_count++;
	while(centers_count<k){
		flag=0;
		while(flag==0){
			random=rand() % items;
			flag1=0;
			for(i=0;i<centers_count;i++){
				if(random==prob[i])
					flag1=1;
			}
			if(flag1==0){
				(*clusters)[centers_count].nodeptr=(*array)[random];
				prob[centers_count]=random;
				flag=1;
			}
		}
		centers_count++;
	}
	free(prob);
}

void ma_pam_ass(ma_cluster *clusters,int no_cl,int items, Node_ma **array,int size,int **distance_matrix){
	int i,j;
	float distance;
	for(i=0;i<items;i++){
		distance=distance_matrix[array[i]->pos][clusters[0].nodeptr->pos];
		array[i]->center2=-1;
		array[i]->dist_center=distance;
		array[i]->center=0;
		for(j=1;j<no_cl;j++){
			distance=distance_matrix[array[i]->pos][clusters[j].nodeptr->pos];
			if(distance<array[i]->dist_center){
				array[i]->dist_center2=array[i]->dist_center;
				array[i]->center2=array[i]->center;
				array[i]->dist_center=distance;
				array[i]->center=j;			
			}
		}
	}
	for(i=0;i<items;i++){
		if(array[i]->center2==-1){
			for(j=1;j<no_cl;j++){
				distance=distance_matrix[array[i]->pos][clusters[j].nodeptr->pos];
				 if(distance!=array[i]->dist_center){
						if(array[i]->center2==-1){
							array[i]->dist_center2=distance;
							array[i]->center2=j;
						}
						else if(distance<array[i]->dist_center2 ){
							array[i]->dist_center2=distance;
							array[i]->center2=j;
						}
				}
			}
		}
	}
}

float objective_value(int items, Node_ma **array){
	float cost=0;
	int i;
	for(i=0;i<items;i++){
		cost+=array[i]->dist_center;
	}
	return cost;
}

float clara_update(ma_cluster **clusters, int no_cl, int items, Node_ma **array, int size,float original_cost,int **distance_matrix){
	int i,j,flag=0,k,t;
	float J=-1,distance1,distance2,sum,total_cost;
	total_cost=original_cost;
	for(j=0;j<no_cl;j++){				//for every center
		for (t=0;t<items;t++){			//for every non centroid
			sum=0;
			for(i=0;i<items;i++){
				if(i!=t){
					if(array[i]->center==j){
						distance1=distance_matrix[array[i]->pos][array[t]->pos];   			//dist(i,t)
						distance2=array[i]->dist_center2;												//dist(i,c')
						if(distance1<=distance2)
							sum+=distance1-array[i]->dist_center;								
						else
							sum+=distance2-array[i]->dist_center;	
					}
					else{
						distance1=distance_matrix[array[i]->pos][array[t]->pos];   			//dist(i,t)
						distance2=array[i]->dist_center;												//dist(i,c)
						if(distance1<distance2){
							sum+=distance1-distance2;
						}
					}
				}			
			}
			if(sum<0){																//if sum is negative then the total cost is lower
				total_cost=total_cost+sum;
				(*clusters)[j].nodeptr=array[t];
				ma_pam_ass((*clusters),no_cl,items,array,size,distance_matrix);					//assign 
			}
		}
	}
		
	return total_cost;
}

void cmp_clusters(ma_cluster *clusters,ma_cluster **temp,int no_cl){
	int i;
	for(i=0;i<no_cl;i++){
		(*temp)[i]=clusters[i];
	}
}

