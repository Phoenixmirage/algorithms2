#include "structs_clara_ham.h"
#include "functions.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Node_Ham{
	char name[12];                      
	unsigned long long int binarystr;    // binary string converted to unsigned long long int
	int visited;
	int center;
	int center2;
	float dist_center;
	float dist_center2;	
};

struct List_nodes_Ham{
	Node_Ham point;                       //list of nodes (each node represents an item)
	List_nodes_Ham *next;
};

struct List_pointers_Ham{
	Node_Ham *nodeptr;                   //list of pointers to nodes (each pointer points to a specific node)
	List_pointers_Ham *next;
};

struct ham_cluster{
	Node_Ham *nodeptr;
	List_pointers_Ham *points;
};

void print_clusters(ham_cluster *clusters,int no_cl){
	int i;
	for(i=0;i<no_cl;i++)
		printf("%s well\n",clusters[i].nodeptr->name);
	
}

void init_array_clara(Node_Ham ***array ,Node_Ham ***array1,List_nodes_Ham *listn, int items,int rand,int sample){ 
	int i;
	(*array)=malloc(sample*(sizeof(Node_Ham*)));				//sample table
	(*array1)=malloc(items*(sizeof(Node_Ham*)));				//full table
	i=items-1;
	List_nodes_Ham *pointer=listn;
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

List_nodes_Ham* Hamming_input(FILE *fd,int* final_size, int * item){
	char bin_string[65];
    char bla[12];
    int items=0;
    int size=0;
    int i,j;
    List_nodes_Ham *listn=NULL;
	fscanf(fd, "%s",bla);
   	fscanf(fd,"%s\n",bin_string);
    size=strlen(bin_string);
    items++;
    List_nodes_Ham *tempnode;
    tempnode=malloc(sizeof(List_nodes_Ham));
    strcpy(tempnode->point.name,bla);
    memset(bla, 0, sizeof(bla));
    tempnode->point.binarystr=string_to_int(bin_string,size);
    tempnode->point.visited=0;
    tempnode->next=listn;
   	listn=tempnode;
    while(!feof(fd)){
    	fscanf(fd, "%s", bla);
        items++;
        if (!strcmp(bla,"")){
        	items--;
            break;                                                        
        }
        fscanf(fd,"%s\n",bin_string);
       	tempnode=malloc(sizeof(List_nodes_Ham));
    	tempnode->point.binarystr=string_to_int(bin_string,size);
        tempnode->point.visited=0;                                      //initialisation of list with items from dataset
        strcpy(tempnode->point.name,bla);
        memset(bla, 0, sizeof(bla));
		tempnode->next=listn;
        listn=tempnode;
    }
    *final_size=size;
	*item=items;
	printf("File Read with success\n");
	return listn;
}
	
void init_ham_cl(ham_cluster **clusters,int no_cl){
 (*clusters)=malloc(no_cl*sizeof(ham_cluster));
 printf("Clusters initialized!\n");
}


void print_array(Node_Ham **array, int items){
	int i;
	for(i=0; i<items ;i++){
		printf("%s,%llu\n",array[i]->name,array[i]->binarystr);
	}
}


int hamming_distance(unsigned long long int number1, unsigned long long int number2, int size){
	int distance=0,i;
	unsigned long long int a_xor, final=1;
	a_xor=number1^number2;                       //XOR returns 0 if bits in a position are different and 1 if they aren't
	for(i=0; i<size; i++){
		if((a_xor&final)==0) distance++;        //counting 0s to find the similarity
		final=final<<1;
	}
	return size-distance;                       //distance is size-similarity
}



void free_list_nodes_ham(List_nodes_Ham **listn, int size){
	List_nodes_Ham *templist;
	int i;
	while((*listn)!=NULL){
		templist=(*listn);
		(*listn)=(*listn)->next;
		free(templist);
	}
}

void pam_init(int k,int size,int items,Node_Ham ***array,ham_cluster **clusters){
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


void ham_pam_ass(ham_cluster *clusters,int no_cl,int items, Node_Ham **array,int size){
	int i,j;
	float distance;
	for(i=0;i<items;i++){
		distance=hamming_distance(array[i]->binarystr,clusters[0].nodeptr->binarystr,size);
		array[i]->center2=-1;
		array[i]->dist_center=distance;
		array[i]->center=0;
		for(j=1;j<no_cl;j++){
			distance=hamming_distance(array[i]->binarystr,clusters[j].nodeptr->binarystr,size);
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
				distance=hamming_distance(array[i]->binarystr,clusters[j].nodeptr->binarystr,size);
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


float objective_value(int items, Node_Ham **array){
	float cost=0;
	int i;
	for(i=0;i<items;i++){
		cost+=array[i]->dist_center;
	}
	return cost;
}


float clara_update(ham_cluster **clusters, int no_cl, int items, Node_Ham **array, int size,float original_cost){
	int i,j,flag=0,k,t;
	float J=-1,distance1,distance2,sum,total_cost;
	Node_Ham *p;
	total_cost=original_cost;
	for(j=0;j<no_cl;j++){				//for every center
		for (t=0;t<items;t++){			//for every non centroid
			sum=0;
			for(i=0;i<items;i++){
				if(i!=t){
					if(array[i]->center==j){
						distance1=hamming_distance(array[i]->binarystr,array[t]->binarystr,size);   			//dist(i,t)
						distance2=array[i]->dist_center2;												//dist(i,c')
						if(distance1<=distance2)
							sum+=distance1-array[i]->dist_center;								
						else
							sum+=distance2-array[i]->dist_center;	
					}
					else{
						distance1=hamming_distance(array[i]->binarystr,array[t]->binarystr,size);   			//dist(i,t)
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
				ham_pam_ass((*clusters),no_cl,items,array,size);					//assign 
			}
		}
	}
		
	return total_cost;
}

void cmp_clusters(ham_cluster *clusters,ham_cluster **temp,int no_cl){
	int i;
	for(i=0;i<no_cl;i++){
		(*temp)[i]=clusters[i];
	}

}