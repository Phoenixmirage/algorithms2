#include <stdio.h>
#include "functions.h"
#include "structs_ma.h"
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <math.h>

struct Node_ma{
	char name[12]; //name of item
	int *array;    // array of distances from all other items
	int pos;		//position in DistanceMatrix
	int visited;    //checks if node is visited to avoid duplicates
	int center;           //cluster which belongs to
	int center2;          //2nd best cluster
	int dist_center;      //distance from center
	int dist_center2; 	//distance from 2nd best center
};

struct List_nodes_ma{        
	Node_ma point;           //list of nodes (each node represents an item)
	List_nodes_ma *next;
};

struct List_pointers_ma{
	Node_ma *nodeptr;         //list of pointers to nodes (each pointer points to a specific node)
	List_pointers_ma *next;
};

struct Dist_points{
	int x1;                        
	int x2;
	float t1;
};

struct ma_cluster{
	Node_ma *nodeptr;
	int items;
	float silhouette;
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

void rand_x1_x2(int **array,Dist_points **rand_x, int L, int k, int size){
	(*rand_x)=malloc(sizeof(Dist_points)*L*k);                      
	int i,j;
	for(i=0;i<L*k;i++){
		(*rand_x)[i].x1=rand()%size;				
		do{
			(*rand_x)[i].x2=rand()%size;
		}while( (*rand_x)[i].x1==(*rand_x)[i].x2); //x1 and x2 must be different to avoid division by 0
	}
	int d1,d2,d3,d4;
	float sum;
	for(i=0;i<L*k;i++){
		sum=0;
		for(j=0;j<size;j++){
			d1=pow(array[j][(*rand_x)[i].x1],2);       //(d(x,x1)^2 + d(x,x2)^2 -d(x1,x2)^2)/2d(x1,x2)   
			d2=pow(array[j][(*rand_x)[i].x2],2);
			d3=array[(*rand_x)[i].x1][(*rand_x)[i].x2];
			d4=((d1+d2-pow(d3,2))/(2*d3));               
		    sum=sum + d4;
		}
		(*rand_x)[i].t1=sum/size;             //t1 median        
	}
}

void init_hash_ma(List_pointers_ma ****hashtable,int **array,Dist_points *rand_x,int size,int k,int L,int hashsize,List_nodes_ma* listn,int **G_h){
	int i,j;
	*hashtable=malloc(sizeof(List_pointers_ma **)*hashsize);
	for(i=0;i<hashsize;i++){
		(*hashtable)[i]=malloc(sizeof(List_pointers_ma*)*L);
	
	
		for(j=0;j<L;j++){
			(*hashtable)[i][j]=NULL;
		}
	}
	printf("Hashtables allocated\n");
	List_nodes_ma *pointer=listn;
	long int bucket;
	while(pointer!=NULL){
		for(i=0;i<L;i++){
			bucket=G_matrix(array,rand_x, pointer->point, G_h,k,i);          //G returns the bucket of the hashtable where the item must be stored
			List_pointers_ma *temptr;
			temptr=malloc(sizeof(List_pointers_ma));                        
			temptr->nodeptr=&(pointer->point);
			temptr->next=(*hashtable)[bucket][i];
			(*hashtable)[bucket][i]=temptr;
		}
		pointer=pointer->next;
	}
	printf("Data stored in hashtables\n");	
}

int H_matrix(int **array,Dist_points rand_x,Node_ma point){
	float d1,d2,d3,d4,sum=0;
	d1=pow(point.array[rand_x.x1],2);          
	d2=pow(point.array[rand_x.x2],2);
	d3=array[rand_x.x1][rand_x.x2];
	sum=((d1+d2-pow(d3,2))/(2*d3));
	if (sum>=rand_x.t1) return 1;
	else return 0;
}

long int G_matrix(int **array,Dist_points *rand_x, Node_ma point, int **G_h,int k,int no_G){
	int i,j,t;
	j=k-1;
	long int sum=0;
	for(i=0;i<k;i++){
		t= H_matrix(array,rand_x[G_h[no_G][i]],point);
		sum=sum+ t*pow(2,j);
		j--;
	}
	return sum;
}

void ma_init_array(Node_ma ***array,List_nodes_ma *listn, int items){
	int i;
	(*array)=malloc(items*(sizeof(Node_ma*)));
	i=items-1;
	List_nodes_ma *pointer=listn;
	while(pointer!=NULL){
		(*array)[i]=&pointer->point;
		(*array)[i]->center=-1;
		pointer=pointer->next;
		i--;
	}
}

void init_ma_cl(ma_cluster **clusters,int no_cl){
	(*clusters)=malloc(no_cl*sizeof(ma_cluster));
	printf("Clusters initialized!\n");
}

void ma_init_parkjun(Node_ma **array,int items,int size,ma_cluster **clusters,int no_cl,int **distances){
	int i,j,z;
	float *vi;
	Node_ma **temparray;
	float distance;
	temparray=malloc(items*sizeof(Node_ma*));
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
	Node_ma *temp;
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
	free(temparray);
	free(vi);
}

void ma_print_clusters(ma_cluster *clusters,int no_cl){
	int i;
	for(i=0;i<no_cl;i++)
		printf("CLUSTER-%d: {size: %d, medoid: %s}\n",i+1,clusters[i].items,clusters[i].nodeptr->name);
}

void ma_pam_ass(ma_cluster *clusters,int no_cl,int items, Node_ma **array,int size){
	int i,j;
	float distance;
	for(i=0;i<items;i++){
		distance=array[i]->array[clusters[0].nodeptr->pos];
		array[i]->center2=-1;
		array[i]->dist_center=distance;
		array[i]->center=0;
		for(j=1;j<no_cl;j++){
			distance=array[i]->array[clusters[j].nodeptr->pos];
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
				distance=array[i]->array[clusters[j].nodeptr->pos];
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

int ma_clarans(ma_cluster **clusters,int no_cl,int items, Node_ma **array,int size,int s, int Q){
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
			distance=array[j]->array[array[m_t[i][1]]->pos];
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

int ma_Loyds(ma_cluster **clusters, int no_cl, int items, Node_ma **array, int size){
	int i,j,flag=0,k;
	float J=-1,distance,sum;
	Node_ma *p;
	for(j=0;j<no_cl;j++){
		J=-1;
		for(i=0;i<items;i++){
			sum=0;
			if(array[i]->center==j){
				for(k=0;k<items;k++){
					if(array[k]->center==j){
						distance=array[i]->array[array[j]->pos];
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

void ma_silhouette(ma_cluster *clusters, Node_ma **array,int size,int no_cl,int items){
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
				dista=dista+ array[i]->array[array[j]->pos];
				suma++;
			}
			else if(array[i]->center2==array[j]->center){
				distb=distb+ array[i]->array[array[j]->pos];
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

void ma_k_medoids(int k,int size,int items,Node_ma ***array,ma_cluster **clusters,int **distance_matrix){
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

void ma_print_data(FILE *output,ma_cluster *clusters, Node_ma **array, int size, int items, int no_cl,int flag,double time){
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

void ma_by_ANN(ma_cluster *clusters,int no_cl,int items, Node_ma **array,int size,List_pointers_ma ***hashtables,int k,int L,Dist_points *rand_x,int hashsize,int **G_h,int **distance_matrix){
	int i,j,sum=0;
	float min_distance,min_distance1,distance;
	for(i=0; i<no_cl; i++){
		for(j=0; j<no_cl; j++){
			if(j!=i){
				distance=clusters[i].nodeptr->array[clusters[j].nodeptr->pos];
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
				bucket=G_matrix(distance_matrix,rand_x, *clusters[i].nodeptr, G_h,k,j); 
				List_pointers_ma *go=hashtables[bucket][j];
				while(go!=NULL){
					distance=go->nodeptr->array[clusters[i].nodeptr->pos];
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
			dist1= array[i]->array[clusters[0].nodeptr->pos];
			array[i]->center=0;
			array[i]->dist_center=dist1;
			for(j=1;j<no_cl;j++){
				dist1= array[i]->array[clusters[j].nodeptr->pos];
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
				 dist1= array[i]->array[clusters[1].nodeptr->pos];
				 array[i]->center2=1;
				array[i]->dist_center2=dist1;
			}
			else{
				dist1= array[i]->array[clusters[0].nodeptr->pos];
				array[i]->center2=0;
				array[i]->dist_center2=dist1;
			}
			for(j=2;j<no_cl;j++){
				dist1= array[i]->array[clusters[j].nodeptr->pos];
				if(dist1<array[i]->dist_center2 && array[i]->center!=j){
					array[i]->dist_center2=dist1;
					array[i]->center2=j;
				}
			}
		}
	}
}

void free_hash_ma(List_pointers_ma  ****hashtable, int hashsize,int L){
	int i,j;
	List_pointers_ma *temp;
	for(i=0;i<hashsize;i++){
		for(j=0;j<L;j++){
			temp=(*hashtable)[i][j];
			while(temp!=NULL){
				List_pointers_ma *temptemp;
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

void free_list_nodes_ma(List_nodes_ma **listn, int size){
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

void ma_main(FILE *input,FILE *output,int k,int no_cl, int Q, int s,int L,int com){
	clock_t begin, end;
	double time_spent;
	int **G_h;
	int size,o,items,i,j,ok;
	List_nodes_ma *listn;
    int **matrix_array;
	listn=matrix_input(input,&size,&items,&matrix_array);
	if(k>(log(items)/log(2))){
		k=(log(items)/log(2))-1;
		printf("k is to big, k is going to be %d\n",k);
	}
    initG_h(&G_h,k,L,1,size);
    Dist_points *rand_x;       //array of struct with x1, x2 random variables from matrix and median t1
    rand_x1_x2(matrix_array,&rand_x,L,k,items);
	int hashsize=pow(2,k);
    List_pointers_ma ***hashtables;
    init_hash_ma(&hashtables,matrix_array,rand_x,size,k,L,hashsize,listn,G_h);
    fclose(input);
    Node_ma **objects;
	ma_init_array(&objects,listn,items);
	ma_cluster *clusters;
	init_ma_cl(&clusters,no_cl);
	printf("Writing in file...\n");
	for(i=1;i<3;i++){
		for(j=0;j<4;j++){
			begin=clock();
			if(i==1 ) ma_k_medoids(no_cl,size,items,&objects,&clusters,matrix_array);
			else if (i==2) ma_init_parkjun(objects,items,size,&clusters,no_cl,matrix_array);
			if (j==0){
				fprintf(output,"Algorithm: I%dA1U1\n",i);
				do{
					ma_pam_ass(clusters,no_cl,items,objects,size);
					ok=ma_Loyds(&clusters,no_cl,items,objects,size);
				}while(ok==1);
				end=clock();
    			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
				ma_silhouette(clusters,objects,size,no_cl,items);
				ma_print_data(output,clusters,objects,size,items,no_cl,com,time_spent);
			}
			else if(j==1){
				fprintf(output,"Algorithm: I%dA1U2\n",i);
				for(o=0;o<s;o++){
					ma_pam_ass(clusters,no_cl,items,objects,size);
					ok=ma_clarans(&clusters,no_cl,items,objects,size,1,Q);
				}
				ma_pam_ass(clusters,no_cl,items,objects,size);
				end=clock();
    			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
				ma_silhouette(clusters,objects,size,no_cl,items);
				ma_print_data(output,clusters,objects,size,items,no_cl,com,time_spent);
			}
			else if(j==2){
				fprintf(output,"Algorithm: I%dA2U1\n",i);
				do{
					ma_by_ANN(clusters,no_cl,items,objects,size,hashtables,k,L,rand_x,hashsize,G_h,matrix_array);
					ok=ma_Loyds(&clusters,no_cl,items,objects,size);
				}while(ok==1);
				end=clock();
    			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
				ma_silhouette(clusters,objects,size,no_cl,items);
				ma_print_data(output,clusters,objects,size,items,no_cl,com,time_spent);
			}
			else if(j==3){
				fprintf(output,"Algorithm: I%dA2U2\n",i);
				for(o=0;o<s;o++){
					ma_by_ANN(clusters,no_cl,items,objects,size,hashtables,k,L,rand_x,hashsize,G_h,matrix_array);
					ok=ma_clarans(&clusters,no_cl,items,objects,size,1,Q);
				}
				ma_by_ANN(clusters,no_cl,items,objects,size,hashtables,k,L,rand_x,hashsize,G_h,matrix_array);
				end=clock();
    			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
				ma_silhouette(clusters,objects,size,no_cl,items);
				ma_print_data(output,clusters,objects,size,items,no_cl,com,time_spent);
			}
		}
	}
	free(clusters);
	free(objects);
	free_hash_ma(&hashtables,hashsize,L);
	free_list_nodes_ma(&listn,size);
	free_matrix_array(&matrix_array,size);
	freeG_h(&G_h,L);
	free(rand_x);
}
