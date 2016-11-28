#include "structs_ham2.h"
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
	int belongs;
	float distance;	
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

void init_array(Node_Ham ***array,List_nodes_Ham *listn, int items){ 
	int i;
	(*array)=malloc(items*(sizeof(Node_Ham*)));
	i=items-1;
	List_nodes_Ham *pointer=listn;
	while(pointer!=NULL){
		(*array)[i]=&pointer->point;
		(*array)[i]->belongs=-1;
		// printf("%s,%d\n",(*array)[i]->name,i);
		pointer=pointer->next;
		i--;
	}
}

void print_clusters(ham_cluster *clusters,int no_cl){
	int i;
	for(i=0;i<no_cl;i++)
		printf("%s well\n",clusters[i].nodeptr->name);
	
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

void k_medoids(int k,int size,int items,Node_Ham ***array,ham_cluster **clusters){
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
				min=hamming_distance((*array)[i]->binarystr,(*clusters)[0].nodeptr->binarystr,size);
				for(j=1;j<centers_count-1;j++){
					temp=hamming_distance((*array)[i]->binarystr,(*clusters)[j].nodeptr->binarystr,size);
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