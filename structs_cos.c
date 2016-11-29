#include "structs_cos.h"
#include "functions.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
struct cos_vec{
	float *vector;               //array of random coordinates
};

struct Node_cos{
	char name[12];
	int visited;
	float* array; 
	int center;
	int center2;
	float dist_center;
	float dist_center2;           //array of coordinates of item 
};

struct List_nodes_cos{
	Node_cos point;        //list of nodes (each node represents an item)
	List_nodes_cos *next;
};

struct List_pointers_cos{
	Node_cos *nodeptr;               //list of pointers to nodes (each pointer points to a specific node)
	List_pointers_cos *next;
};

struct cos_cluster{
	Node_cos *nodeptr;
	int items;
	double silhouette;
};

long int G_cosine(int **G_h, int no_G, Node_cos p, int size, int k,cos_vec *vectors){
	int i,j;
	j=k-1;
	long int sum=0;
	float t;
	for(i=0;i<k;i++){
		t= H_cosine(vectors[G_h[no_G][i]],p,size);
		sum=sum+ t*pow(2,j);
		j--;
	}
	return sum;
}
 int H_cosine(cos_vec vector_t, Node_cos p,int size){
	int i;
	float sum=0;
	for(i=0; i<size; i++){
		sum=sum+ (vector_t.vector[i]*p.array[i]);                       // inner product of random vector and item p
	}
	if (sum>=0){
		return 1;
	}
	else{
		return 0;
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

List_nodes_cos* Cosine_input(FILE *fd,int* final_size, int * item){
	int items=0;
	List_nodes_cos *listn=NULL;
    int size=0;
    int tempsize=2;
    float * array;
    char bla[12];
    char c;
    c='t';
    array=malloc(tempsize*sizeof(float));
    fscanf(fd, "%s",bla);
    items++;
	while(c!='\n')
    {
    	fscanf(fd, "%f%c", &(array[size]), &c);
        size++;
        if (size==tempsize-1){                            //finds the dimension of vector
            tempsize*=2;
            array=realloc(array, tempsize*sizeof(float));
        }
    }
    array=realloc(array, size*sizeof(float));
    List_nodes_cos *tempnod;
    tempnod=malloc(sizeof(List_nodes_cos));
    strcpy(tempnod->point.name,bla);
    memset(bla, 0, sizeof(bla));
    tempnod->point.array=array;
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
       				
       	tempnod=malloc(sizeof(List_nodes_cos));
       	strcpy(tempnod->point.name,bla);
       	memset(bla, 0, sizeof(bla));                              //fill list of items
       	tempnod->point.array=malloc(size*sizeof(float));
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
	return listn;
}

void init_randvector(cos_vec **randvec,int L, int k,int size){
	int i,j;
	(*randvec)=malloc(L*k*(sizeof(cos_vec)));
    for(i=0;i<L*k;i++){
        (*randvec)[i].vector=malloc(size*sizeof(float));
    }
    for(i=0;i<L*k;i++){
        for(j=0;j<size;j++){
        	(*randvec)[i].vector[j]=marsaglia();         //initialise array of rand vectors using gaussian distribution
        }	
    }

}

void cos_init_array(Node_cos ***array,List_nodes_cos *listn, int items){
	int i;
	(*array)=malloc(items*(sizeof(Node_cos*)));
	i=items-1;
	List_nodes_cos *pointer=listn;
	while(pointer!=NULL){
		(*array)[i]=&pointer->point;
		(*array)[i]->center=-1;
		pointer=pointer->next;
		i--;
	}
}

void cos_print_array(Node_cos **array, int items){
	int i;
	for(i=0; i<items ;i++){
		printf("%s,%f\n",array[i]->name,array[i]->array[0]);
	}
}

void init_cos_cl(cos_cluster **clusters,int no_cl){
	(*clusters)=malloc(no_cl*sizeof(cos_cluster));
	printf("Clusters initialized!\n");
}

void cos_init_parkjun(Node_cos **array,int items,int size,cos_cluster **clusters,int no_cl){
	int i,j,z;
	float **distances;
	distances= malloc(items*sizeof(float*));
	for(i=0;i<items;i++){
		distances[i]=malloc(items*sizeof(float));
	}
	float distance;
	for(i=0;i<items;i++){
		for(j=0;j<items;j++){
			distance=cosine_distance(array[i]->array,array[j]->array,size);
			distances[i][j]=distance;
		}
	}
	float *vi;
	Node_cos **temparray;
	temparray=malloc(items*sizeof(Node_cos*));
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
	Node_cos *temp;
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


void cos_print_clusters(cos_cluster *clusters,int no_cl){
	int i;
	for(i=0;i<no_cl;i++)
		printf("CLUSTER-%d: {size: %d, medoid: %s}\n",i+1,clusters[i].items,clusters[i].nodeptr->name);
}

void cos_pam_ass(cos_cluster *clusters,int no_cl,int items, Node_cos **array,int size){
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

int cos_clarans(cos_cluster **clusters,int no_cl,int items, Node_cos **array,int size,int s, int Q){
	int i,j,flag=0,w;
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
			distance= cosine_distance(array[j]->array,array[m_t[i][1]]->array,size);
			if(array[j]->center==m_t[i][0]){
				if(array[j]->dist_center2>=distance){
					dJ=dJ+distance- array[j]->dist_center;
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

int cos_Loyds(cos_cluster **clusters, int no_cl, int items, Node_cos **array, int size){
	int i,j,flag=0,k;
	float J=-1,distance,sum;
	Node_cos *p;
	for(j=0;j<no_cl;j++){
		J=-1;
		for(i=0;i<items;i++){
			sum=0;
			if(array[i]->center==j){
				for(k=0;k<items;k++){
					if(array[k]->center==j){
						distance=cosine_distance(array[i]->array,array[j]->array,size);
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

void cos_k_medoids(int k,int size,int items,Node_cos ***array,cos_cluster **clusters){
	int centers_count=1;
	int random,*cent_pos,flag,flag1,i,l,j;
	srand(time(NULL));
	float *prob,sum,sumtemp,rand_sum,temp,min;
	float *previous_rands;
	random=rand() % items;
	cent_pos=malloc(k*sizeof(int));
	prob=malloc((items+1)*sizeof(float));
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

void cos_silhouette(cos_cluster *clusters, Node_cos **array,int size,int no_cl,int items){
	int i,j;
	double dista=0, distb=0,s;
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
				dista=dista+ cosine_distance(array[i]->array,array[j]->array,size);
				suma++;
			}
			else if(array[i]->center2==array[j]->center){
				distb=distb+ cosine_distance(array[i]->array,array[j]->array,size);
				sumb++;
			}
		}
		suma--;
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

void cos_by_ANN(cos_cluster *clusters,int no_cl,int items, Node_cos **array,int size,List_pointers_cos ***hashtables,int k,int L,cos_vec *randvec,int hashsize,int **G_h){
	int i,j,sum=0;
	float min_distance,min_distance1,distance;
	for(i=0; i<no_cl; i++){
		for(j=0; j<no_cl; j++){
			if(j!=i){
				distance=cosine_distance(clusters[i].nodeptr->array,clusters[j].nodeptr->array,size);
				if(i==0 && j==1) min_distance=distance;
				if(distance<min_distance) min_distance=distance;
			}
		}
	}
	min_distance1=min_distance;
	min_distance=min_distance/2;
	int bucket,flag,flag1;
	do{
		flag=0;
		for(i=0;i<no_cl;i++){
			for(j=0;j<L;j++){
				bucket= G_cosine(G_h,j,*clusters[i].nodeptr,size,k,randvec);
				List_pointers_cos *go=hashtables[bucket][j];
				while(go!=NULL){
					distance=cosine_distance(go->nodeptr->array,clusters[i].nodeptr->array,size);
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
		min_distance= 2*min_distance;
	}while(flag==1);
	for(i=0;i<items;i++){
		float dist1;
		if(array[i]->center==-1){
			dist1= cosine_distance(array[i]->array,clusters[0].nodeptr->array,size);
			array[i]->center=0;
			array[i]->dist_center=dist1;
			for(j=1;j<no_cl;j++){
				dist1= cosine_distance(array[i]->array,clusters[j].nodeptr->array,size);
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
				 dist1= cosine_distance(array[i]->array,clusters[1].nodeptr->array,size);
				 array[i]->center2=1;
				array[i]->dist_center2=dist1;
			}
			else{
				dist1= cosine_distance(array[i]->array,clusters[0].nodeptr->array,size);
				array[i]->center2=0;
				array[i]->dist_center2=dist1;
			}
			for(j=2;j<no_cl;j++){
				dist1= cosine_distance(array[i]->array,clusters[j].nodeptr->array,size);
				if(dist1<array[i]->dist_center2 && array[i]->center!=j){
					array[i]->dist_center2=dist1;
					array[i]->center2=j;
				}
			}
		}
	}
}

void cos_print_data(FILE *output,cos_cluster *clusters, Node_cos **array, int size, int items, int no_cl,int flag,double time){
	int i,j;
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
	for(i=0;i<items;i++){
		array[i]->center=-1;
		array[i]->center2=-1;
	}
}


void init_hash_cos(List_pointers_cos ****hashtable,cos_vec *randvec,int size,int k,int L,int hashsize,List_nodes_cos *listn,int **G_h){
	int i,j;
	*hashtable=malloc(sizeof(List_pointers_cos **)*hashsize);
	for(i=0;i<hashsize;i++){
		(*hashtable)[i]=malloc(sizeof(List_pointers_cos*)*L);
			for(j=0;j<L;j++){
			(*hashtable)[i][j]=NULL;
		}
	}
	printf("Hashtables allocated\n");
	List_nodes_cos *pointer=listn;
	long int bucket;
	long int g;
	while(pointer!=NULL){
		for(i=0;i<L;i++){
			bucket=G_cosine(G_h,i, pointer->point, size,k,randvec);      //G returns the bucket of the hashtable where the item must be stored
			List_pointers_cos *temptr;
			temptr=malloc(sizeof(List_pointers_cos));
			temptr->nodeptr=&(pointer->point);
			temptr->next=(*hashtable)[bucket][i];
			(*hashtable)[bucket][i]=temptr;
		}
		pointer=pointer->next;
	}
	printf("Data stored in hashtables\n");
}

void cos_main(FILE *input,FILE *output,int k,int no_cl, int Q, int s,int L){
	clock_t begin, end;
	double time_spent;
	int **G_h;
	int size,o,items,i,j,ok;
	List_nodes_cos *listn;
	listn=Cosine_input(input,&size,&items);
	if(k>(log(items)/log(2))){
		k=(log(items)/log(2))-1;
		printf("k is to big, k is going to be %d\n",k);
	}
	cos_vec *randvec;
	init_randvector(&randvec,L,k,size);
	initG_h(&G_h,k,L,1,size);
	int hashsize=pow(2,k);
	List_pointers_cos ***hashtables;
	init_hash_cos(&hashtables,randvec,size,k,L,hashsize,listn,G_h);
	fclose(input);
	Node_cos **objects;
	cos_init_array(&objects,listn,items);
	cos_cluster *clusters;
	init_cos_cl(&clusters,no_cl);
	printf("Writing in file...\n");
	for(i=1;i<3;i++){
		for(j=0;j<4;j++){
			begin=clock();
			if(i==1) cos_k_medoids(no_cl,size,items,&objects,&clusters);
			else cos_init_parkjun(objects,items,size,&clusters,no_cl);
			if (j==0){
				fprintf(output,"Algorithm: I%dA1U1\n",i);
				do{
					cos_pam_ass(clusters,no_cl,items,objects,size);
					ok=cos_Loyds(&clusters,no_cl,items,objects,size);
				}while(ok==1);
				end=clock();
				time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
				cos_silhouette(clusters,objects,size,no_cl,items);
				cos_print_data(output,clusters,objects,size,items,no_cl,0,time_spent);
			}
			else if(j==1){
				fprintf(output,"Algorithm: I%dA1U2\n",i);
				for(o=0;o<s;o++){
					cos_pam_ass(clusters,no_cl,items,objects,size);
					ok=cos_clarans(&clusters,no_cl,items,objects,size,1,Q);
				}
				cos_pam_ass(clusters,no_cl,items,objects,size);
				end=clock();
    			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
				cos_silhouette(clusters,objects,size,no_cl,items);
				cos_print_data(output,clusters,objects,size,items,no_cl,0,time_spent);
			}
			else if(j==2){
				fprintf(output,"Algorithm: I%dA2U1\n",i);
				do{
					cos_by_ANN(clusters,no_cl,items,objects,size,hashtables,k,L,randvec,hashsize,G_h);
					ok=cos_Loyds(&clusters,no_cl,items,objects,size);
				}while(ok==1);
				end=clock();
    			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
				cos_silhouette(clusters,objects,size,no_cl,items);
				cos_print_data(output,clusters,objects,size,items,no_cl,0,time_spent);
			}
			else if(j==3){
				fprintf(output,"Algorithm: I%dA2U2\n",i);
				for(o=0;o<s;o++){
					cos_by_ANN(clusters,no_cl,items,objects,size,hashtables,k,L,randvec,hashsize,G_h);
					ok=cos_clarans(&clusters,no_cl,items,objects,size,1,Q);
				}
				cos_by_ANN(clusters,no_cl,items,objects,size,hashtables,k,L,randvec,hashsize,G_h);
				end=clock();
    			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
				cos_silhouette(clusters,objects,size,no_cl,items);
				cos_print_data(output,clusters,objects,size,items,no_cl,0,time_spent);
			}
		}
	}
	free(clusters);
	free(objects);
	free_randvec_cos(&randvec,L,k);
	free_hash_cos(&hashtables,hashsize,L);
	free_list_nodes_cos(&listn,size);
	freeG_h(&G_h,L);
}


void free_hash_cos(List_pointers_cos  ****hashtable, int hashsize,int L){
	int i,j;
	List_pointers_cos *temp;
	for(i=0;i<hashsize;i++){
		for(j=0;j<L;j++){
			temp=(*hashtable)[i][j];
			while(temp!=NULL){
				List_pointers_cos *temptemp;
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

void free_list_nodes_cos(List_nodes_cos **listn, int size){
	List_nodes_cos *templist;
	int i;
	while((*listn)!=NULL){
		templist=(*listn);
		(*listn)=(*listn)->next;
		free(templist->point.array);
		free(templist);
	}
}

void free_randvec_cos(cos_vec **randvec, int L, int k){
	int i;
	for(i=0;i<L*k;i++){
		free((*randvec)[i].vector);
	}
	free(*randvec);
	(*randvec)=NULL;
}
