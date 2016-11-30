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
	int center;        //cluster which belongs to
	int center2;       //second best cluster
	float dist_center;     //distance from center of cluster
	float dist_center2;		//distance of second best center								 
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
	Node *nodeptr;              //pointer to center
	int items;					//number of items in cluster
	float silhouette;          //silhouette pointer
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
	int i;                                                       //array of items to easy find the items
	(*array)=malloc(items*(sizeof(Node*)));
	i=items-1;
	List_nodes *pointer=listn;
	while(pointer!=NULL){
		(*array)[i]=&pointer->point;
		(*array)[i]->center=-1;
		(*array)[i]->center2=-1;
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

void euc_init_parkjun(Node **array,int items,int size,euc_cluster **clusters,int no_cl){  //initialization concentrate
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
		(*clusters)[i].items=0;
		(*clusters)[i].silhouette=0;
	}
	for(i=0;i<items;i++){
		free(distances[i]);
	}
	free(distances);
	free(temparray);
	free(vi);
}

void print_clusters(euc_cluster *clusters,int no_cl){                               //we dont use this function. it is just for check
	int i;

	for(i=0;i<no_cl;i++)
		printf("CLUSTER-%d: {size: %d, medoid: %s}\n",i+1,clusters[i].items,clusters[i].nodeptr->name);
		
	
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
//assignment by ANN
void euc_by_ANN(euc_cluster *clusters,int no_cl,int items, Node **array,int size,List_pointers ***hashtables,int k,int L,int W,euc_vec *randvec,long int *random_r,int hashsize,int **G_h){
	int i,j,sum=0;
	float min_distance,min_distance1,distance;
	for(i=0; i<no_cl; i++){
		for(j=0; j<no_cl; j++){
			if(j!=i){
				distance=euclidean_distance(clusters[i].nodeptr->array,clusters[j].nodeptr->array,size);
				if(i==0 && j==1) min_distance=distance;
				if(distance<min_distance) min_distance=distance;
			}
		}
	}
	min_distance1=min_distance;
	min_distance=min_distance/2;
	int bucket,id,flag,flag1;
	do{
		flag=0;
		for(i=0;i<no_cl;i++){
			for(j=0;j<L;j++){
				id=ID_euclidean(G_h,j,*clusters[i].nodeptr, size,random_r,k,randvec,W);
				bucket= F_Euclidean(id,hashsize);
				List_pointers *go=hashtables[bucket][j];
				while(go!=NULL){
					distance=euclidean_distance(go->nodeptr->array,clusters[i].nodeptr->array,size);
					if(distance<=min_distance){
						if(go->nodeptr->center==-1){
							flag=1;
							sum++;
							go->nodeptr->dist_center=distance;
							go->nodeptr->center=i;
						}
						else{
							if(distance < go->nodeptr->dist_center){
								flag=1;
								go->nodeptr->dist_center2=go->nodeptr->dist_center;
								go->nodeptr->center2=go->nodeptr->center;
								go->nodeptr->dist_center=distance;
								go->nodeptr->center=i;
							}
						}
					}
				go=go->next;
				}
			}
		}
		min_distance= 2*min_distance;         //double the radius
	}while(flag==1);                         //stops when none of the centers has new item
	for(i=0;i<items;i++){                                          //check for items left without center and add 2nd best center
		float dist1;
		if(array[i]->center==-1){
			dist1= euclidean_distance(array[i]->array,clusters[0].nodeptr->array,size);
			array[i]->center=0;
			array[i]->dist_center=dist1;
			for(j=1;j<no_cl;j++){
				dist1= euclidean_distance(array[i]->array,clusters[j].nodeptr->array,size);
				if(dist1<array[i]->dist_center){
					array[i]->center2=array[i]->center;
					array[i]->dist_center2=array[i]->dist_center;
					array[i]->center=j;
					array[i]->dist_center=dist1;
				}
			}
		}
		if(array[i]->center2==-1){
			if(array[i]->center==0){
				 dist1= euclidean_distance(array[i]->array,clusters[1].nodeptr->array,size);
				 array[i]->center2=1;
				array[i]->dist_center2=dist1;
			}
			else{
				dist1= euclidean_distance(array[i]->array,clusters[0].nodeptr->array,size);
				array[i]->center2=0;
				array[i]->dist_center2=dist1;
			}
			for(j=2;j<no_cl;j++){
				dist1= euclidean_distance(array[i]->array,clusters[j].nodeptr->array,size);
				if(dist1<array[i]->dist_center2 && array[i]->center!=j){
					array[i]->dist_center2=dist1;
					array[i]->center2=j;
				}
			}
		}
	}
}
//pam assignment
void euc_pam_ass(euc_cluster *clusters,int no_cl,int items, Node **array,int size){
	int i,j;
	float distance;
	for(i=0;i<items;i++){
		distance=euclidean_distance(array[i]->array,clusters[0].nodeptr->array,size);
		array[i]->center2=-1;
		array[i]->dist_center=distance;                                                   //finds first best
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
			for(j=1;j<no_cl;j++){                                    //check for second best (it might already exist from the first "for")
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
}
//assignment by clarans
int euc_clarans(euc_cluster **clusters,int no_cl,int items, Node **array,int size,int s, int Q){
	int i,j,flag=0,w;
	euc_cluster *tempclusters;
	int **m_t;
	m_t=malloc(Q*sizeof(int*));
	for(i=0;i<Q;i++){
		m_t[i]=malloc(2*sizeof(int));
	}
	int random;
	for(i=0;i<Q;i++){
		random=rand()%(no_cl*items);
		m_t[i][0]=random%no_cl;
		m_t[i][1]=random/no_cl;
	}
	float J=0;
	for(i=0;i<items;i++){
		J=J+array[i]->dist_center;
	}
	flag=0;
	float best_J=J;
	float dJ,distance,J_new;
	for(w=0;w<s;w++){
		for(i=0;i<Q;i++){
			dJ=0;
			for(j=0;j<items;j++){
				distance= euclidean_distance(array[j]->array,array[m_t[i][1]]->array,size);
				if(array[j]->center==m_t[i][0]){
					if(array[j]->dist_center2>=distance){
						dJ=dJ+distance- array[j]->dist_center;            //as written in theory
					}
					else{
						dJ=dJ+array[j]->dist_center2 - array[j]->dist_center;
					}
				}
				else{
					if(array[j]->dist_center>distance){
						dJ=dJ+distance- array[j]->dist_center;
					}
				}
			}
			J_new=J+dJ;
			if(J_new<J && J_new<best_J){ 
				(*clusters)[m_t[i][0]].nodeptr=array[m_t[i][1]];
		 		best_J=J_new;
		 		flag=1;
			}
		}
	}
	for(i=0;i<items;i++){
		array[i]->center=-1;
		array[i]->center2=-1;
	}
	for(i=0;i<Q;i++){
		free(m_t[i]);
	}
	free(m_t);
	return flag;
}

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
						distance=euclidean_distance(array[i]->array,array[j]->array,size);  //calculate medoids
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
		if(p!=(*clusters)[j].nodeptr){
			flag=1;
			(*clusters)[j].nodeptr=p;
		}
	}
	if(flag==1){
		for(i=0;i<items;i++){
			array[i]->center=-1;
			array[i]->center2=-1;
		}
	}
	return flag;
}

void k_medoids(int k,int size,int items,Node ***array,euc_cluster **clusters){
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
			for(l=0;l<centers_count-1;l++){
				if(i==cent_pos[l])
					flag=1;
			}
			if(flag==0){
				min=euclidean_distance((*array)[i]->array,(*clusters)[0].nodeptr->array,size);
				for(j=1;j<centers_count-1;j++){
					temp=euclidean_distance((*array)[i]->array,(*clusters)[j].nodeptr->array,size);
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
			for(j=0;j<centers_count-2;j++){
				if(rand_sum==previous_rands[j])
					flag1=0;
			}
			if(flag1==1){
				previous_rands[centers_count-2]=rand_sum;
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
			for(l=0;l<centers_count-1;l++){
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
		centers_count++;
	}
	free(cent_pos);
	free(prob);
	free(previous_rands);
}



void euc_silhouette(euc_cluster *clusters, Node **array,int size,int no_cl,int items){
	int i,j;
	float dista=0, distb=0,s;
	int suma,sumb;
	for(i=0;i<no_cl;i++){
		clusters[i].items=0;
		clusters[i].silhouette=0;
	}
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
		dista=dista/suma;
		distb=distb/sumb;
		if(dista>=distb) s= distb/dista -1;
		else s=1- dista/distb;
		clusters[array[i]->center].items++;
		clusters[array[i]->center].silhouette+=s;
	}
	for(i=0;i<no_cl;i++){
		clusters[i].silhouette=clusters[i].silhouette/clusters[i].items;
	}
}

void euc_print_data(FILE *output,euc_cluster *clusters, Node **array, int size, int items, int no_cl,int flag,double time){
	int i,j,flag1=0;
	for(i=0; i<no_cl; i++){
		fprintf(output,"CLUSTER-%d: {size: %d, medoid: %s}\n",i+1,clusters[i].items,clusters[i].nodeptr->name);
	}
	fprintf(output,"clustering time:%f\n",time);
	fprintf(output,"Silhouette: [");
	for(i=0;i<no_cl;i++){
		if(i!=0) fprintf(output,",");
		fprintf(output,"%f",clusters[i].silhouette);
		clusters[i].silhouette=0;
		clusters[i].items=0;
		clusters[i].nodeptr=NULL;
	}
	fprintf(output,"]\n");
	if(flag==1){
		for(i=0;i<no_cl;i++){
			flag1=1;
			fprintf(output,"CLUSTER-%d: {",i+1);
			for(j=0;j<items;j++){
				if(array[j]->center==i){
					if(flag1==1){
						fprintf(output,"%s",array[j]->name);
						flag1=0;
					}
					else fprintf(output,",%s",array[j]->name);
				}
			}
			fprintf(output,"}\n");
		}
	}
	for(i=0;i<items;i++){
		array[i]->center=-1;
		array[i]->center2=-1;
	}
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

void euc_main(FILE *input,FILE *output,int k,int no_cl, int Q, int s,int L,int W,int com){
	clock_t begin, end;
	double time_spent;
	int **G_h;
	int size,o,items,i,j,ok;
	List_nodes *listn;       //list of elements read from file
	listn=Euclidean_input(input,&size, &items); //store elements from file in list
	if(k>(log(items)/log(2))){
		k=(log(items)/log(2))-1;
		printf("k is to big, k is going to be %d\n",k);
	}
	euc_vec *randvec;             // random vectors
	init_randvec(&randvec,L,k,W,size);   
	initG_h(&G_h,k,L,1,size);  
	int hashsize=items/8;       
	List_pointers ***hashtables;   
	long int *random_r;    //1d array with random r
	random_r=malloc(k*sizeof(long int));
	for(i=0;i<k;i++){
			random_r[i]=(long int)rand();
	}
	init_hash(&hashtables,randvec,size,k,L,hashsize,listn,G_h,W,random_r); //store itmes from list to hashtables with euclidean method
	fclose(input);
	Node **objects;
	init_array(&objects,listn,items);
	euc_cluster *clusters;
	init_euc_cl(&clusters,no_cl);
	printf("Writing in file...\n");
	for(i=1;i<3;i++){                 //here starts the combination of the algorithms
		for(j=0;j<4;j++){
			begin=clock();
			if(i==1) k_medoids(no_cl,size,items,&objects,&clusters);
			else if(i==2) euc_init_parkjun(objects,items,size,&clusters,no_cl);
			if (j==0){
				fprintf(output,"Algorithm: I%dA1U1\n",i);
				do{
					euc_pam_ass(clusters,no_cl,items,objects,size);
					ok=euc_Loyds(&clusters,no_cl,items,objects,size);
				}while(ok==1);
				end=clock();
    			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
				euc_silhouette(clusters,objects,size,no_cl,items);
				euc_print_data(output,clusters,objects,size,items,no_cl,com,time_spent);
						
			}
			else if(j==1){
				fprintf(output,"Algorithm: I%dA1U2\n",i);
				for(o=0;o<s;o++){
					euc_pam_ass(clusters,no_cl,items,objects,size);
					ok=euc_clarans(&clusters,no_cl,items,objects,size,1,Q);
				}
				euc_pam_ass(clusters,no_cl,items,objects,size);
				end=clock();
    			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
				euc_silhouette(clusters,objects,size,no_cl,items);
				euc_print_data(output,clusters,objects,size,items,no_cl,com,time_spent);
			}
			else if(j==2){
				fprintf(output,"Algorithm: I%dA2U1\n",i);
				do{
					euc_by_ANN(clusters,no_cl,items,objects,size,hashtables,k,L,W,randvec,random_r,hashsize,G_h);
					ok=euc_Loyds(&clusters,no_cl,items,objects,size);
				}while(ok==1);
				end=clock();
    			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
				euc_silhouette(clusters,objects,size,no_cl,items);
				euc_print_data(output,clusters,objects,size,items,no_cl,com,time_spent);
			}
			else if(j==3){
				fprintf(output,"Algorithm: I%dA2U2\n",i);
				for(o=0;o<s;o++){
					euc_by_ANN(clusters,no_cl,items,objects,size,hashtables,k,L,W,randvec,random_r,hashsize,G_h);
					ok=euc_clarans(&clusters,no_cl,items,objects,size,1,Q);
				}
				euc_by_ANN(clusters,no_cl,items,objects,size,hashtables,k,L,W,randvec,random_r,hashsize,G_h);
				end=clock();
    			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
				euc_silhouette(clusters,objects,size,no_cl,items);
				euc_print_data(output,clusters,objects,size,items,no_cl,com,time_spent);
			}
		}
	}
	fclose(output);
	free(clusters);
	free(objects);
	free_randvec(&randvec,L,k);
	free_hash(&hashtables,hashsize,L);
	free_list_nodes(&listn,size);
	freeG_h(&G_h,L);
	free (random_r);
}
