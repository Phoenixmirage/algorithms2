#include "structs.h"
#include "functions.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct euc_vec{
	float *vector;     //array of random coordinates
	float t;
};

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

struct euc_cluster{
	Node *nodeptr;
	List_pointers *points;
};

int F_Euclidean(unsigned long int ID,int hash_size){
	return ID%hash_size;
}
unsigned long int ID_euclidean(int **G_h, int no_G, Node p, int size, long int *random_r, int k,euc_vec *vectors,int W){
	unsigned long int M=pow(2,32)-5;
	int i,j;
	unsigned long int sum=0;
	float t;
	for(i=0;i<k;i++){
		t= H_euclidean(vectors[G_h[no_G][i]],p,size,W);    
		t=t*random_r[i];
		sum= sum+ t;
	}
	if(sum<0) sum=sum*(-1);
		return sum%M;                              
}

long int H_euclidean(euc_vec vector_t, Node p,int size,int W){
	int i;
	long int j;
	float sum=0;
	for(i=0; i<size; i++){
		sum=sum+ (vector_t.vector[i]*p.array[i]);               //inner product
	}
	sum=(sum+vector_t.t)/W;
	j=sum;
	return j;
}

float euclidean_distance(float *point, float *item, int size){
	int i;
	float sum=0,square;
	for(i=0; i<size; i++){
		square= point[i]-item[i];
		square=pow(square,2);                                     //(xi-yi)^2 for each coordinate
		sum=sum + square;
	}
	sum=sqrt(sum);
	return sum;
}

List_nodes* Euclidean_input(FILE *fd,int* final_size, int * item){
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
	/*	while(pointer!=NULL){
			printf("%s\n",pointer->point.name);
			pointer=pointer->next;
		}*/
		return listn;
	}
	
void init_randvec(euc_vec **randvec,int L, int k,int W,int size){
	int i,j;
	(*randvec)=malloc(L*k*(sizeof(euc_vec)));
    for(i=0;i<L*k;i++){
        (*randvec)[i].vector=malloc(size*sizeof(float));
    }
    for(i=0;i<L*k;i++){
        (*randvec)[i].t=((float)rand()/(float)(RAND_MAX)*W);
        for(j=0;j<size;j++){
        	(*randvec)[i].vector[j]=marsaglia();                               //initialise array of rand vectors using gaussian distribution
        }	
    }
}

void init_array(Node ***array,List_nodes *listn, int items){
	int i;
	(*array)=malloc(items*(sizeof(Node*)));
	i=items-1;
	List_nodes *pointer=listn;
	while(pointer!=NULL){
		(*array)[i]=&pointer->point;
		(*array)[i]->center=-1;
	//	printf("%s,%d\n",(*array)[i]->name,i);
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

void init_euc_cl(euc_cluster **clusters,int no_cl){
	(*clusters)=malloc(no_cl*sizeof(euc_cluster));
	printf("Clusters initialized!\n");
}

void euc_init_parkjun(Node **array,int items,int size,euc_cluster **clusters,int no_cl){
	int i,j,z;
	float **distances;
	distances= malloc(items*sizeof(float*));
	for(i=0;i<items;i++){
		distances[i]=malloc(items*sizeof(float));
	}
	float distance;
	for(i=0;i<items;i++){
		for(j=0;j<items;j++){
			distance=euclidean_distance(array[i]->array,array[j]->array,size);
			distances[i][j]=distance;
			//if(j==i+1) 	printf("%f\n",distances[i][j]);
		}
	}
	float *vi;
	Node **temparray;
	temparray=malloc(items*sizeof(Node*));
	for(i=0;i<items;i++){
		temparray[i]=array[i];
	}
	vi=malloc(items*sizeof(float));
	int t;
	float dist;
	for(i=0;i<items;i++){
		vi[i]=0;
		for(j=0;j<items;j++){
			dist=0;
			for(t=0; t<items; t++){
				dist= dist+ distances[j][t];
			}
			distance= distances[i][j];
			vi[i]=vi[i]+ (distance/dist);
		}
	}
	Node *temp;
	float swap;
	for(z=0; z<items-1; z++){
		for(i=0; i<items-z-1; i++){
			if(vi[i]>vi[i+1]){
				temp=temparray[i];
				swap=vi[i];
				vi[i]=vi[i+1];
				temparray[i]=temparray[i+1];
				vi[i+1]=swap;
				temparray[i+1]=temp;
			}
		}
	}
	for(i=0;i<no_cl;i++){
		(*clusters)[i].nodeptr=temparray[i];
		(*clusters)[i].points=NULL;
	}
  /*	for(i=0;i<no_cl;i++){
		printf("%s,%f\n",(*clusters)[i].nodeptr->name,vi[i]);
	}*/
}

void print_clusters(euc_cluster *clusters,int no_cl){
	int i;
	for(i=0;i<no_cl;i++)
		printf("%s\n",clusters[i].nodeptr->name);
	
}

void init_hash(List_pointers ****hashtable,euc_vec *randvec,int size,int k,int L,int hashsize,List_nodes *listn,int **G_h,int W,long int *random_r){
		int i,j;
	*hashtable=malloc(sizeof(List_pointers **)*hashsize);
	for(i=0;i<hashsize;i++){
		(*hashtable)[i]=malloc(sizeof(List_pointers*)*L);
			for(j=0;j<L;j++){
			(*hashtable)[i][j]=NULL;
		}
	}
	printf("Hashtables allocated\n");
	List_nodes *pointer=listn;
	int bucket;
	long int g;
	while(pointer!=NULL){
		for(i=0;i<L;i++){
			g=ID_euclidean(G_h,i, pointer->point, size,random_r,k,randvec,W);  
			bucket= F_Euclidean(g,hashsize);                                    //F returns the bucket of the hashtable where the item must be stored
			List_pointers *temptr;
			temptr=malloc(sizeof(List_pointers));
			temptr->nodeptr=&(pointer->point);
			temptr->id=g;
			temptr->next=(*hashtable)[bucket][i];
			(*hashtable)[bucket][i]=temptr;
		}
		pointer=pointer->next;
	}
	printf("Data stored in hashtables\n");
}

void euc_by_ANN(euc_cluster *clusters,int no_cl,int items, Node **array,int size,List_pointers ***hashtables,int k,int L,int W,euc_vec *randvec,long int *random_r,int hashsize,int **G_h){
	int i,j;
	float min_distance,distance;
	for(i=0; i<no_cl; i++){
		for(j=0; j<no_cl; j++){
			if(j!=i){
				printf("Compare %s with %s\n",clusters[i].nodeptr->name,clusters[j].nodeptr->name);
				distance=euclidean_distance(clusters[i].nodeptr->array,clusters[j].nodeptr->array,size);
				if(i==0 && j==1) min_distance=distance;
				if(distance<min_distance) min_distance=distance;
			}
		}
	}
	min_distance=min_distance/2;
	int bucket,id,flag,flag1;
	do{
		printf("radious is %f\n",min_distance);
		flag=0;
		for(i=0;i<no_cl;i++){
			for(j=0;j<L;j++){
				id=ID_euclidean(G_h,i,*clusters[i].nodeptr, size,random_r,k,randvec,W);
				bucket= F_Euclidean(id,hashsize);
				List_pointers *go=hashtables[bucket][i];
				while(go!=NULL){
					distance=euclidean_distance(go->nodeptr->array,clusters[i].nodeptr->array,size);
					if(distance<=min_distance){
						if(go->nodeptr->center==-1){
							flag=1;
							go->nodeptr->dist_center=distance;
							go->nodeptr->center=j;
						}
						else{
							if(distance < go->nodeptr->dist_center){
								flag=1;
								go->nodeptr->dist_center=distance;
								go->nodeptr->center=j;
							}
						}
					}
				go=go->next;
				}
			}
		}
		min_distance= 2*min_distance;
	}while(flag==1);
	/*for(i=0;i<items;i++){
		if(array[i]->)
	}*/
	for(i=0;i<items;i++){
		printf("%s, %d\n",array[i]->name,array[i]->center);
	}
}

void euc_pam_ass(euc_cluster *clusters,int no_cl,int items, Node **array,int size){
	int i,j;
	float distance;
	for(i=0;i<items;i++){
		distance=euclidean_distance(array[i]->array,clusters[0].nodeptr->array,size);
		array[i]->center2=-1;
		array[i]->dist_center=distance;
		array[i]->center=0;
		for(j=1;j<no_cl;j++){
			distance=euclidean_distance(array[i]->array,clusters[j].nodeptr->array,size);
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
				distance=euclidean_distance(array[i]->array,clusters[j].nodeptr->array,size);
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
/*	for(i=0;i<items;i++){
		printf("%s: %d,%d\n",array[i]->name,array[i]->center,array[i]->center2);
	}*/
}

/*int euc_clarans(euc_cluster **clusters,int no_cl,int items, Node **array,int size,int s, int Q){
	int i,j,flag=0;
	euc_cluster **tempclusters;
	tempclusters=malloc(no_cl*sizeof(euc_cluster*));
	int **m_t;
	m_t=malloc(Q*sizeof(int*));
	for(i=0;i<Q;i++){
		m_t[i]=malloc(2*sizeof(int));
	}
	int random;
	for(i=0;i<Q;i++){
		random=rand()%(no_cl*itmes);
		m_t[i][0]=random%no_cl;
		m_t[i][1]=random/no_cl;
	}
	float J=0;
	for(i=0;i<items;i++){
		J=J+array[i]->dist_center;
	}
	for(i=0;i<Q;i++){
		
	}
	
	
	
}*/

int euc_Loyds(euc_cluster **clusters, int no_cl, int items, Node **array, int size){
	int i,j,flag=0,k;
	float J=-1,distance,sum;
	Node *p;
	for(j=0;j<no_cl;j++){
		J=-1;
		for(i=0;i<items;i++){
			sum=0;
			if(array[i]->center==j){
				for(k=0;k<items;k++){
					if(array[k]->center==j){
						distance=euclidean_distance(array[i]->array,array[j]->array,size);
						sum=sum+distance;
						if(J==-1){
							J=sum;
							p=array[i];
						}
						if(sum<J){
							J=sum;
							p=array[i];
						}
					}
				}
			}
		}
	//	printf("Center changed in cluster: %d. Old was %s new is %s\n",j,(*clusters)[j].nodeptr->name,p->name);
		if(p!=(*clusters)[j].nodeptr){
			flag=1;
			printf("Center changed in cluster: %d. Old was %s new is %s\n",j,(*clusters)[j].nodeptr->name,p->name);
			(*clusters)[j].nodeptr=p;

		}
	}
	return flag;
	
}

void euc_silhouette(euc_cluster *clusters, Node **array,int size,int no_cl,int items){
	int i,j;
	float dista=0, distb=0,s;
	int suma,sumb;
	for(i=0;i<items;i++){
		dista=0;
		distb=0;
		suma=0;
		sumb=0;
		for(j=0;j<items;j++){
			if(array[i]->center==array[j]->center){
				dista=dista+ euclidean_distance(array[i]->array,array[j]->array,size);
				suma++;
			}
			else if(array[i]->center2==array[j]->center){
				distb=distb+ euclidean_distance(array[i]->array,array[j]->array,size);
				sumb++;
			}
		}
		suma--;
		dista=dista/suma;
		distb=distb/sumb;
		if(dista>=distb) s=distb/dista -1;
		else s=distb/dista -1;
		printf("%s, %f\n",array[i]->name,s);
	}
}

void search_euclidean(List_pointers ***hashtables,FILE *input,List_nodes *listn,int k,int L,int size,int W,euc_vec *randvec,long int *random_r,int hashsize,int **G_h,FILE *output){
	int i,j,bucket,id;
    	char radius[20];
	int flag=0;
    	fscanf(input, "Radius: %s\n",radius);
    	double time_spent,time_spent1;
   	float Radius= atof(radius);
    float distance, max_distance=1000;
    List_pointers *neighbor;
    clock_t begin, begin1, end, end1;
    if (Radius==0) flag=1;
    char bloo[12];
    while(!feof(input)){
    	memset(bloo, 0, sizeof(bloo));
    	fscanf(input, "%s",bloo);
    	if (!strcmp(bloo,"")){
            break;
        }
        Node point;
        point.array=malloc(size*sizeof(float));
        for(i=0;i<size;i++)
        {
        	fscanf(input, "%f", &point.array[i]);
        }
        List_nodes *pointer=listn;
        while(pointer!=NULL){
       			
        		pointer->point.visited=0;
        		pointer=pointer->next;
        		
		}
        max_distance=1000;
        begin=clock();
        for(i=0;i<L;i++){
        	id=ID_euclidean(G_h,i, point, size,random_r,k,randvec,W);
			bucket= F_Euclidean(id,hashsize);
			List_pointers *go=hashtables[bucket][i];
			while(go!=NULL){
				if(id==go->id){                                                                    //search for nearest neighbour 
					if(go->nodeptr->visited==0){
						distance=euclidean_distance(point.array,go->nodeptr->array,size);
						if(distance<max_distance && distance!=0){
							max_distance=distance;
							neighbor=go;
						}
						go->nodeptr->visited=1;
					}
				}
				go=go->next;
			}
		}
		end=clock();
    	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    	pointer=listn;
		float max_distance1=1000;
		begin1=clock();
       	while(pointer!=NULL){
       		distance=euclidean_distance(point.array,pointer->point.array,size);
        	if(distance<=max_distance1 && distance!=0){                        //all visited=0 and search for true NN at the same time
        		max_distance1=distance;
        	}	
        	pointer->point.visited=0;
        	pointer=pointer->next;
        }
		end1=clock();
		time_spent1 = (double)(end1 - begin1) / CLOCKS_PER_SEC;
		fflush(output);
        fprintf(output,"\nQuery: %s\n",bloo);
        if (flag==0){
        	fflush(output);
        	fprintf(output,"R-nearest neighbours:\n");
        	for(i=0;i<L;i++){
				id=ID_euclidean(G_h,i, point, size,random_r,k,randvec,W);
				bucket= F_Euclidean(id,hashsize);
				List_pointers *go=hashtables[bucket][i];
				while(go!=NULL){
					if(id==go->id){
						if(go->nodeptr->visited==0){
							distance=euclidean_distance(point.array,go->nodeptr->array,size); //print neighbours in radius
							if(distance<=Radius){
								fflush(output);
								fprintf(output,"%s\n",go->nodeptr->name);
							}
							go->nodeptr->visited=1;
						}
					}
					go=go->next;
				}
			}
		}
		fflush(output);
		fprintf(output,"Nearest neighbor: %s\nDistanceLSH: %f\n",neighbor->nodeptr->name,max_distance);
		fprintf(output,"DistanceTrue: %f\ntLSH: %f\ntTrue:%f\n",max_distance1,time_spent,time_spent1);
		free(point.array);
	}
	printf("File written successfully\n");
}

void free_hash(List_pointers  ****hashtable, int hashsize,int L){
	int i,j;
	List_pointers *temp;
	for(i=0;i<hashsize;i++){
		for(j=0;j<L;j++){
			temp=(*hashtable)[i][j];
			while(temp!=NULL){
				List_pointers *temptemp;
				temptemp=temp;
				temp=temp->next;
				free(temptemp);
			}
		}
		free((*hashtable)[i]);
	}
	free(*hashtable);
	(*hashtable)=NULL;
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

void free_randvec(euc_vec **randvec, int L, int k){
	int i;
	for(i=0;i<L*k;i++){
		free((*randvec)[i].vector);
	}
	free(*randvec);
	(*randvec)=NULL;
}
