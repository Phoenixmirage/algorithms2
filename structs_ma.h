#ifndef STRUCTS5_H

#define STRUCTS5_H
#include <stdio.h>
typedef struct Node_ma Node_ma;
typedef struct Dist_points Dist_points;
typedef struct List_nodes_ma List_nodes_ma;
typedef struct List_pointers_ma List_pointers_ma;
typedef struct ma_cluster ma_cluster;

List_nodes_ma* matrix_input(FILE *fd,int* final_size, int * item,int ***array);
void rand_x1_x2(int **array,Dist_points **rand_x, int L, int k, int size);
void init_hash_ma(List_pointers_ma ****hashtable,int **array,Dist_points *rand_x,int size,int k,int L,int hashsize,List_nodes_ma* listn,int **G_h);
long int G_matrix(int **array,Dist_points *rand_x, Node_ma point, int **G_h,int k,int no_G);
int H_matrix(int **array,Dist_points rand_x,Node_ma point);
void free_matrix_array(int ***array,int size);
void free_list_nodes_ma(List_nodes_ma **listn, int size);
void free_hash_ma(List_pointers_ma  ****hashtable, int hashsize,int L);
void search_matrix(List_pointers_ma ***hashtable, int **array, Dist_points *rand_x, int **G_h, int k, int L, List_nodes_ma *listn,FILE *input,FILE *output,int size);
void ma_init_array(Node_ma ***array,List_nodes_ma *listn, int items);
void init_ma_cl(ma_cluster **clusters,int no_cl);
void ma_init_parkjun(Node_ma **array,int items,int size,ma_cluster **clusters,int no_cl,int **distances);
void ma_print_clusters(ma_cluster *clusters,int no_cl);
void ma_pam_ass(ma_cluster *clusters,int no_cl,int items, Node_ma **array,int size);
int ma_clarans(ma_cluster **clusters,int no_cl,int items, Node_ma **array,int size,int s, int Q);
int ma_Loyds(ma_cluster **clusters, int no_cl, int items, Node_ma **array, int size);
void ma_silhouette(ma_cluster *clusters, Node_ma **array,int size,int no_cl,int items);
void ma_k_medoids(int k,int size,int items,Node_ma ***array,ma_cluster **clusters,int **distance_matrix);
void ma_print_data(FILE *output,ma_cluster *clusters, Node_ma **array, int size, int items, int no_cl,int flag,double time);
void ma_by_ANN(ma_cluster *clusters,int no_cl,int items, Node_ma **array,int size,List_pointers_ma ***hashtables,int k,int L,Dist_points *rand_x,int hashsize,int **G_h,int **distance_matrix);
void ma_main(FILE *input,FILE *output,int k,int no_cl, int Q, int s,int L);

#endif

