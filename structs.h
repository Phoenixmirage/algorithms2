#ifndef STRUCTS_H

#define STRUCTS_H
#include <stdio.h>
typedef struct Node Node;
typedef struct List_nodes List_nodes;
typedef struct List_pointers List_pointers;
typedef struct euc_vec euc_vec;
typedef struct euc_cluster euc_cluster;

long int H_euclidean(euc_vec vector_t, Node p,int size,int W);
unsigned long int ID_euclidean(int **G_h, int no_G, Node p, int size,long int *random_r, int k,euc_vec *vectors,int W);
int F_Euclidean(unsigned long int ID,int hash_size);
float euclidean_distance(float *point, float *item, int size);
List_nodes* Euclidean_input(FILE *fd,int* final_size, int * item);
void init_randvec(euc_vec **randvec,int L, int k,int W,int size);
void init_hash(List_pointers ****hashtable,euc_vec *randvec,int size,int k,int L,int hashsize,List_nodes *listn,int **G_h,int W,long int *random_r);
void search_euclidean(List_pointers ***hashtables,FILE *input,List_nodes *listn,int k,int L,int size,int W,euc_vec *randvec,long int *random_r,int hashsize,int **G_h,FILE*output);
void free_randvec(euc_vec **randvec, int L, int k);
void free_list_nodes(List_nodes **listn, int size);
void free_hash(List_pointers  ****hashtable, int hashsize,int L);
void init_array(Node ***array,List_nodes *listn, int items);
void print_array(Node **array, int items);
void init_euc_cl(euc_cluster **clusters,int no_cl);
void euc_init_parkjun(Node **array,int items,int size,euc_cluster **clusters,int no_cl);
void print_clusters(euc_cluster *clusters,int no_cl);
void euc_by_ANN(euc_cluster *clusters,int no_cl,int items, Node **array,int size,List_pointers ***hashtables,int k,int L,int W,euc_vec *randvec,long int *random_r,int hashsize,int **G_h);
void euc_pam_ass(euc_cluster *clusters,int no_cl,int items, Node **array,int size);
int euc_Loyds(euc_cluster **clusters, int no_cl, int items, Node **array, int size);
void euc_silhouette(euc_cluster *clusters, Node **array,int size,int no_cl,int items);
int euc_clarans(euc_cluster **clusters,int no_cl,int items, Node **array,int size,int s, int Q);
void k_medoids(int k,int size,int items,Node ***array,euc_cluster **clusters);
void euc_print_data(FILE *output,euc_cluster *clusters, Node **array, int size, int items, int no_cl,int flag,double time);
void euc_main(FILE *input,FILE *output,int k,int no_cl, int Q, int s,int L,int W);
#endif
