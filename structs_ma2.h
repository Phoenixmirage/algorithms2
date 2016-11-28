#ifndef STRUCTS_MA2_H

#define STRUCTS_MA2_H
#include <stdio.h>

typedef struct Node_ma Node_ma;
typedef struct Dist_points Dist_points;
typedef struct List_nodes_ma List_nodes_ma;
typedef struct List_pointers_ma List_pointers_ma;
typedef struct ma_cluster ma_cluster;

List_nodes_ma* matrix_input(FILE *fd,int* final_size, int * item,int ***array);
void free_matrix_array(int ***array,int size);
void free_list_nodes_ma(List_nodes_ma **listn, int size);
void init_array(Node_ma ***array,List_nodes_ma *listn, int items);
void print_array(Node_ma **array, int items);
void init_ma_cl(ma_cluster **clusters,int no_cl);
void k_medoids(int k,int size,int items,Node_ma ***array,ma_cluster **clusters,int **distance_matrix);
void print_clusters(ma_cluster *clusters,int no_cl);
#endif