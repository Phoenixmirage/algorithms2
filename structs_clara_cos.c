#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structs_clara_cos.h"

struct Node{
	char name[12];
	int visited;
	int center;
	int center2;
	float dist_center;
	float dist_center2;									
	float* array;     //array of coordinates of item 
};

struct List_nodes{
	Node point;               //list of nodes (each node represents an item)
	List_nodes *next;
};

struct List_pointers{
	long int id;					//we use ID to avoid unnecessary computations 
	Node *nodeptr;                 //list of pointers to nodes (each pointer points to a specific node)
	List_pointers *next;
};

struct cos_cluster{
	Node *nodeptr;
	List_pointers *points;
};

void init_array_clara(Node ***array ,Node ***array1,List_nodes *listn, int items,int rand,int sample){ 
	int i;
	(*array)=malloc(sample*(sizeof(Node*)));				//sample table
	(*array1)=malloc(items*(sizeof(Node*)));				//full table
	i=items-1;
	List_nodes *pointer=listn;
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

void print_array(Node **array, int items){
	int i;
	for(i=0; i<items ;i++){
		printf("%s,%f\n",array[i]->name,array[i]->array[0]);
	}
}


void init_cos_cl(cos_cluster **clusters,int no_cl){
 (*clusters)=malloc(no_cl*sizeof(cos_cluster));
 printf("Clusters initialized!\n");
}

List_nodes* Cosine_input(FILE *fd,int* final_size, int * item){
		int items=0;
		List_nodes *listn=NULL;
		int size=0;
		int tempsize=2;
		char bla[12];
		float * array;
		array=malloc(tempsize*sizeof(float));
		fscanf(fd, "%s",bla);
		items++;
		char c;
		c='t';
		while(c!='\n')
		{
				fscanf(fd, "%f%c", &(array[size]), &c);
				size++;                                                         //finds the dimension of vector
				if (size==tempsize-1){                                           
						tempsize*=2;
						array=realloc(array, tempsize*sizeof(float));
				}
		}	
		array=realloc(array, size*sizeof(float));
		List_nodes *tempnod;
		tempnod=malloc(sizeof(List_nodes));
		strcpy(tempnod->point.name,bla);
		memset(bla, 0, sizeof(bla));
		tempnod->point.array=array;
		tempnod->point.visited=0;
		tempnod->next=listn;
		listn=tempnod;
		int i;
		while(!feof(fd)){
			fscanf(fd, "%s", bla);
			items++;
			if (!strcmp(bla,"")){
				items--;
					break;
			}
			tempnod=malloc(sizeof(List_nodes));
			strcpy(tempnod->point.name,bla);
			memset(bla, 0, sizeof(bla));
			tempnod->point.array=malloc(size*sizeof(float));
			tempnod->point.visited=0;                                                    //fill list of items
			for(i=0;i<size;i++)
			{
					fscanf(fd, "%f", &(tempnod->point.array[i]));
			}
			tempnod->next=listn;
			listn=tempnod;
		}
		*final_size=size;
		*item=items;
		printf("File Read with success\n");
		List_nodes *pointer=listn;
		return listn;
	}

float cosine_distance(float *point, float *item, int size){
	int i,j=0;
	float sum=0,square,meterA=0,meterB=0;
	for(i=0; i<size; i++){
		if(point[i]==item[i]) j++;
		meterA= meterA + pow(point[i],2);                                // cosine of p and q
		meterB= meterB + pow(item[i],2);
		sum=sum + point[i]*item[i];
	}
	meterA=sqrt(meterA);
	meterB=sqrt(meterB);
	sum=sum/(meterA*meterB);
	sum=1-sum;
	if(j==size) return 0;
	return sum;
}

void print_clusters(cos_cluster *clusters,int no_cl){
	int i;
	for(i=0;i<no_cl;i++)
		printf("%s \n",clusters[i].nodeptr->name);
	
}

void free_list_nodes(List_nodes **listn, int size){
	List_nodes *templist;
	int i;
	while((*listn)!=NULL){
		templist=(*listn);
		(*listn)=(*listn)->next;
		free(templist->point.array);
		free(templist);
	}
}

void pam_init(int k,int size,int items,Node ***array,cos_cluster **clusters){
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


void cos_pam_ass(cos_cluster *clusters,int no_cl,int items, Node **array,int size){
	int i,j;
	float distance;
	for(i=0;i<items;i++){
		distance=cosine_distance(array[i]->array,clusters[0].nodeptr->array,size);
		array[i]->center2=-1;
		array[i]->dist_center=distance;
		array[i]->center=0;
		for(j=1;j<no_cl;j++){
			distance=cosine_distance(array[i]->array,clusters[j].nodeptr->array,size);
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
				distance=cosine_distance(array[i]->array,clusters[j].nodeptr->array,size);
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


float objective_value(int items, Node **array){
	float cost=0;
	int i;
	for(i=0;i<items;i++){
		cost+=array[i]->dist_center;
	}
	return cost;
}


float clara_update(cos_cluster **clusters, int no_cl, int items, Node **array, int size,float original_cost){
	int i,j,flag=0,k,t;
	float J=-1,distance1,distance2,sum,total_cost;
	Node *p;
	total_cost=original_cost;
	for(j=0;j<no_cl;j++){				//for every center
		for (t=0;t<items;t++){			//for every non centroid
			sum=0;
			for(i=0;i<items;i++){
				if(i!=t){
					if(array[i]->center==j){
						distance1=cosine_distance(array[i]->array,array[t]->array,size);   			//dist(i,t)
						distance2=array[i]->dist_center2;												//dist(i,c')
						if(distance1<=distance2)
							sum+=distance1-array[i]->dist_center;								
						else
							sum+=distance2-array[i]->dist_center;	
					}
					else{
						distance1=cosine_distance(array[i]->array,array[t]->array,size);   			//dist(i,t)
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
				cos_pam_ass((*clusters),no_cl,items,array,size);					//assign objects to the new centroids
			}
		}
	}
		
	return total_cost;
}

void cmp_clusters(cos_cluster *clusters,cos_cluster **temp,int no_cl){
	int i;
	for(i=0;i<no_cl;i++){
		(*temp)[i]=clusters[i];
	}

}