#ifndef STRUCTS1_H

#define STRUCTS1_H
#include <stdio.h>
typedef struct Node_cos Node_cos;
typedef struct List_nodes_cos List_nodes_cos;
typedef struct List_pointers_cos List_pointers_cos;
typedef struct cos_vec cos_vec;
typedef struct cos_cluster cos_cluster;

long int G_cosine(int **G_h, int no_G, Node_cos p, int size, int k,cos_vec *vectors);
int H_cosine(cos_vec vector_t, Node_cos p,int size);
float cosine_distance(float *point, float *item, int size);
List_nodes_cos* Cosine_input(FILE *fd,int* final_size, int * item);
void init_randvector(cos_vec **randvec,int L, int k,int size);
void init_hash_cos(List_pointers_cos ****hashtable,cos_vec *randvec,int size,int k,int L,int hashsize,List_nodes_cos *listn,int **G_h);
void search_cosine(List_pointers_cos ***hashtables,FILE *input,List_nodes_cos *listn,int k,int L,int size,cos_vec *randvec,int hashsize,int **G_h,FILE *output);
void free_randvec_cos(cos_vec **randvec, int L, int k);
void free_hash_cos(List_pointers_cos  ****hashtable, int hashsize,int L);
void free_list_nodes_cos(List_nodes_cos **listn, int size);
void cos_init_array(Node_cos ***array,List_nodes_cos *listn, int items);
void cos_print_array(Node_cos **array, int items);
void init_cos_cl(cos_cluster **clusters,int no_cl);
void cos_init_parkjun(Node_cos **array,int items,int size,cos_cluster **clusters,int no_cl);
void cos_print_clusters(cos_cluster *clusters,int no_cl);
void cos_pam_ass(cos_cluster *clusters,int no_cl,int items, Node_cos **array,int size);
int cos_clarans(cos_cluster **clusters,int no_cl,int items, Node_cos **array,int size,int s, int Q);
int cos_Loyds(cos_cluster **clusters, int no_cl, int items, Node_cos **array, int size);
void cos_silhouette(cos_cluster *clusters, Node_cos **array,int size,int no_cl,int items);
void cos_k_medoids(int k,int size,int items,Node_cos ***array,cos_cluster **clusters);
void cos_print_data(FILE *output,cos_cluster *clusters, Node_cos **array, int size, int items, int no_cl,int flag,double time);
void cos_by_ANN(cos_cluster *clusters,int no_cl,int items, Node_cos **array,int size,List_pointers_cos ***hashtables,int k,int L,cos_vec *randvec,int hashsize,int **G_h);
void cos_main(FILE *input,FILE *output,int k,int no_cl, int Q, int s,int L,int com);
#endif
