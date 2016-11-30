#include "structs_ham.h"
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
	int center;               //cluster which belongs to
	int center2;              //second best cluster
	int dist_center;          //distance from center of cluster
	int dist_center2;         //distance of second best center
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
	Node_Ham *nodeptr;                   //pointer to center
	int items;                           //number of items in cluster
	float silhouette;                    //silhouette pointer
};

long int G_hamming(int **G_h, int no_G, int number, int size, int k){
	int i,j,t;
	j=k-1;
	long int sum=0;
	for(i=0;i<k;i++){
		t= H_hamming(number,size,G_h[no_G][i]);
		sum=sum+ t*pow(2,j);
		j--;
	}
	return sum;
}

int H_hamming(unsigned long long int number, int size, int pos){
	int i;
	unsigned long long int k=1,o;      
	k=k<<(size-pos);                 
	o=number&k;                     //AND operator used to find if there is 1 or 0 in position
	if(o==0) return 0;
	else return 1;
	
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

void Ham_init_array(Node_Ham ***array,List_nodes_Ham *listn, int items){
	int i;                                                              //array of items to easy find the items
	(*array)=malloc(items*(sizeof(Node_Ham*)));
	i=items-1;
	List_nodes_Ham *pointer=listn;
	while(pointer!=NULL){
		(*array)[i]=&pointer->point;
		(*array)[i]->center=-1;
		(*array)[i]->center2=-1;
		pointer=pointer->next;
		i--;
	}
}
void init_ham_cl(ham_cluster **clusters,int no_cl){
	(*clusters)=malloc(no_cl*sizeof(ham_cluster));
	printf("Clusters initialized!\n");
}
//initialization concentrate
void ham_init_parkjun(Node_Ham **array,int items,int size,ham_cluster **clusters,int no_cl){
	int i,j,z;
	int **distances;
	distances= malloc(items*sizeof(int*));
	for(i=0;i<items;i++){
		distances[i]=malloc(items*sizeof(int));
	}
	int distance;
	for(i=0;i<items;i++){
		for(j=0;j<items;j++){
			distance=hamming_distance(array[i]->binarystr,array[j]->binarystr,size); 
			distances[i][j]=distance;
		}
	}
	double *vi;
	Node_Ham **temparray;
	temparray=malloc(items*sizeof(Node_Ham*));
	for(i=0;i<items;i++){
		temparray[i]=array[i];
	}
	vi=malloc(items*sizeof(double));
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
			vi[i]=vi[i] + (distance/dist);
		}
	}
	Node_Ham *temp;
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
	free(vi);
	free(temparray);
	free(distances);
}

void ham_print_clusters(ham_cluster *clusters,int no_cl){
	int i;
	for(i=0;i<no_cl;i++)
		printf("CLUSTER-%d: {size: %d, medoid: %s}\n",i+1,clusters[i].items,clusters[i].nodeptr->name);
}
//pam assignment
void ham_pam_ass(ham_cluster *clusters,int no_cl,int items, Node_Ham **array,int size){
	int i,j;
	int distance;
	for(i=0;i<items;i++){
		distance=hamming_distance(array[i]->binarystr,clusters[0].nodeptr->binarystr,size);
		array[i]->center2=-1;
		array[i]->dist_center=distance;
		array[i]->center=0;
		for(j=1;j<no_cl;j++){
			distance=hamming_distance(array[i]->binarystr,clusters[j].nodeptr->binarystr,size); //best center and some second best
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
						if(array[i]->center2==-1){                                      //find 2nd best if it already doesnt have one
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

int ham_clarans(ham_cluster **clusters,int no_cl,int items, Node_Ham **array,int size,int s, int Q){
	int i,j,flag=0,w;
	ham_cluster *tempclusters;
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
			distance= hamming_distance(array[j]->binarystr,array[m_t[i][1]]->binarystr,size);
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

void ham_silhouette(ham_cluster *clusters, Node_Ham **array,int size,int no_cl,int items){
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
				dista=dista+ hamming_distance(array[i]->binarystr,array[j]->binarystr,size);
				suma++;
			}
			else if(array[i]->center2==array[j]->center){
				distb=distb+ hamming_distance(array[i]->binarystr,array[j]->binarystr,size);
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

int ham_Loyds(ham_cluster **clusters, int no_cl, int items, Node_Ham **array, int size){
	int i,j,flag=0,k;
	float J=-1,distance,sum;
	Node_Ham *p;
	for(j=0;j<no_cl;j++){
		J=-1;
		for(i=0;i<items;i++){
			sum=0;
			if(array[i]->center==j){
				for(k=0;k<items;k++){
					if(array[k]->center==j){
						distance=hamming_distance(array[i]->binarystr,array[j]->binarystr,size);
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

void ham_k_medoids(int k,int size,int items,Node_Ham ***array,ham_cluster **clusters){
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

void ham_print_data(FILE *output,ham_cluster *clusters, Node_Ham **array, int size, int items, int no_cl,int flag,double time){
	int i,j,flag1;
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


void ham_by_ANN(ham_cluster *clusters,int no_cl,int items, Node_Ham **array,int size,List_pointers_Ham ***hashtables,int k,int L,int hashsize,int **G_h){
	int i,j,sum=0;
	float min_distance,min_distance1,distance;
	for(i=0; i<no_cl; i++){
		for(j=0; j<no_cl; j++){
			if(j!=i){
				distance=hamming_distance(clusters[i].nodeptr->binarystr,clusters[j].nodeptr->binarystr,size);
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
				bucket=G_hamming(G_h, j,clusters[i].nodeptr->binarystr ,size,k);
				List_pointers_Ham *go=hashtables[bucket][j];
				while(go!=NULL){
					distance=hamming_distance(go->nodeptr->binarystr,clusters[i].nodeptr->binarystr,size);
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
			dist1= hamming_distance(array[i]->binarystr,clusters[0].nodeptr->binarystr,size);
			array[i]->center=0;
			array[i]->dist_center=dist1;
			for(j=1;j<no_cl;j++){
				dist1= hamming_distance(array[i]->binarystr,clusters[j].nodeptr->binarystr,size);
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
				 dist1= hamming_distance(array[i]->binarystr,clusters[1].nodeptr->binarystr,size);
				 array[i]->center2=1;
				array[i]->dist_center2=dist1;
			}
			else{
				dist1= hamming_distance(array[i]->binarystr,clusters[0].nodeptr->binarystr,size);
				array[i]->center2=0;
				array[i]->dist_center2=dist1;
			}
			for(j=2;j<no_cl;j++){
				dist1= hamming_distance(array[i]->binarystr,clusters[j].nodeptr->binarystr,size);
				if(dist1<array[i]->dist_center2 && array[i]->center!=j){
					array[i]->dist_center2=dist1;
					array[i]->center2=j;
				}
			}
		}
	}
}

void init_hash_Ham(List_pointers_Ham ****hashtable,int size,int k,int L,int hashsize,List_nodes_Ham *listn,int **G_h){
	int i,j;
	*hashtable=malloc(sizeof(List_pointers_Ham **)*hashsize);
	for(i=0;i<hashsize;i++){                                         //initialisation of hashtables
		(*hashtable)[i]=malloc(sizeof(List_pointers_Ham*)*L);
			for(j=0;j<L;j++){
			(*hashtable)[i][j]=NULL;
		}
	}
	printf("Hashtables allocated\n");
	List_nodes_Ham *pointer=listn;
	long int bucket;
	long int g;
	while(pointer!=NULL){
		for(i=0;i<L;i++){
			bucket=G_hamming(G_h,i, pointer->point.binarystr, size,k);        //G returns the bucket of the hashtable where the item must be stored
			List_pointers_Ham *temptr;
			temptr=malloc(sizeof(List_pointers_Ham));						//fill the list of bucket
			temptr->nodeptr=&(pointer->point);
			temptr->next=(*hashtable)[bucket][i];
			(*hashtable)[bucket][i]=temptr;
		}
		pointer=pointer->next;
	}
	printf("Data stored in hashtables\n");
}


void free_hash_ham(List_pointers_Ham  ****hashtable, int hashsize,int L){
	int i,j;
	List_pointers_Ham *temp;
	for(i=0;i<hashsize;i++){
		for(j=0;j<L;j++){
			temp=(*hashtable)[i][j];
			while(temp!=NULL){
				List_pointers_Ham *temptemp;
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

void free_list_nodes_ham(List_nodes_Ham **listn, int size){
	List_nodes_Ham *templist;
	int i;
	while((*listn)!=NULL){
		templist=(*listn);
		(*listn)=(*listn)->next;
		free(templist);
	}
}

void ham_main(FILE *input,FILE *output,int k,int no_cl, int Q, int s,int L,int com){
	clock_t begin, end;
	double time_spent;
	int **G_h;
	int size,o,items,i,j,ok;
	List_nodes_Ham *listn;
	listn=Hamming_input(input,&size,&items);
	if(k>(log(items)/log(2))){
		k=(log(items)/log(2))-1;
		printf("k is to big, k is going to be %d\n",k);
	}
    initG_h(&G_h,k,L,0,size);
    int hashsize=pow(2,k);
    List_pointers_Ham ***hashtables;
    init_hash_Ham(&hashtables,size,k,L,hashsize,listn,G_h);
    fclose(input);
    Node_Ham **objects;
	Ham_init_array(&objects,listn,items);
	ham_cluster *clusters;
	init_ham_cl(&clusters,no_cl);
	printf("Writing in file...\n");
	for(i=1;i<3;i++){
		for(j=0;j<4;j++){
			begin=clock();
			if(i==1 ) ham_k_medoids(no_cl,size,items,&objects,&clusters);
			else if (i==2 ) ham_init_parkjun(objects,items,size,&clusters,no_cl);
			if (j==0){
				fprintf(output,"Algorithm: I%dA1U1\n",i);
				do{
					ham_pam_ass(clusters,no_cl,items,objects,size);
					ok=ham_Loyds(&clusters,no_cl,items,objects,size);
				}while(ok==1);
				end=clock();
    			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
				ham_silhouette(clusters,objects,size,no_cl,items);
				ham_print_data(output,clusters,objects,size,items,no_cl,com,time_spent);
			}
			else if(j==1){
				fprintf(output,"Algorithm: I%dA1U2\n",i);
				for(o=0;o<s;o++){
					ham_pam_ass(clusters,no_cl,items,objects,size);
					ok=ham_clarans(&clusters,no_cl,items,objects,size,1,Q);
				}
				ham_pam_ass(clusters,no_cl,items,objects,size);
				end=clock();
    			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
				ham_silhouette(clusters,objects,size,no_cl,items);
				ham_print_data(output,clusters,objects,size,items,no_cl,com,time_spent);
			}
			else if(j==2){
				fprintf(output,"Algorithm: I%dA2U1\n",i);
				do{
					ham_by_ANN(clusters,no_cl,items,objects,size,hashtables,k,L,hashsize,G_h);
					ok=ham_Loyds(&clusters,no_cl,items,objects,size);
				}while(ok==1);
				end=clock();
    			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
				ham_silhouette(clusters,objects,size,no_cl,items);
				ham_print_data(output,clusters,objects,size,items,no_cl,com,time_spent);
			}
			else if(j==3){
				fprintf(output,"Algorithm: I%dA2U2\n",i);
				for(o=0;o<s;o++)
				{
					ham_by_ANN(clusters,no_cl,items,objects,size,hashtables,k,L,hashsize,G_h);
					ok=ham_clarans(&clusters,no_cl,items,objects,size,1,Q);
				}
				ham_by_ANN(clusters,no_cl,items,objects,size,hashtables,k,L,hashsize,G_h);
				end=clock();
    			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
				ham_silhouette(clusters,objects,size,no_cl,items);
				ham_print_data(output,clusters,objects,size,items,no_cl,com,time_spent);
			}
		}
	}
	free(clusters);
	free(objects);
	free_hash_ham(&hashtables,hashsize,L);
	free_list_nodes_ham(&listn,size);
	freeG_h(&G_h,L);
}
