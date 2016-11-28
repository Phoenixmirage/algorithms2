#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structs_cos2.h"


struct Node{
	char name[12];
	int visited;
	int belongs;
	float distance;										
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


void init_array(Node ***array,List_nodes *listn, int items){ 
	int i;
	(*array)=malloc(items*(sizeof(Node*)));
	i=items-1;
	List_nodes *pointer=listn;
	while(pointer!=NULL){
		(*array)[i]=&pointer->point;
		(*array)[i]->belongs=-1;
		// printf("%s,%d\n",(*array)[i]->name,i);
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



void k_medoids(int k,int size,int items,Node ***array,cos_cluster **clusters){
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
				min=cosine_distance((*array)[i]->array,(*clusters)[0].nodeptr->array,size);
				for(j=1;j<centers_count-1;j++){
					temp=cosine_distance((*array)[i]->array,(*clusters)[j].nodeptr->array,size);
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
		//printf("sum %f\n",sum);
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
				//printf("sumtemp %f  prob %f\n",sumtemp,prob[i]);
				if(sumtemp>=rand_sum){							//if the new temp surpasses the rand ,means we found our new center
					//printf("sumtemp %f  rand %f\n",sumtemp,rand_sum);
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