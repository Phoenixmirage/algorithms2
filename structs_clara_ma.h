#ifndef STRUCTS_CLARA_MA_H

#define STRUCTS_CLARA_MA_H
#include <stdio.h>

typedef struct Node_ma Node_ma;
typedef struct Dist_points Dist_points;
typedef struct List_nodes_ma List_nodes_ma;
typedef struct List_pointers_ma List_pointers_ma;
typedef struct ma_cluster ma_cluster;

List_nodes_ma* matrix_input(FILE *fd,int* final_size, int * item,int ***array);
void free_matrix_array(int ***array,int size);
void free_list_nodes_ma(List_nodes_ma **listn, int size);
void print_array(Node_ma **array, int items);
void init_ma_cl(ma_cluster **clusters,int no_cl);
void print_clusters(ma_cluster *clusters,int no_cl);

void init_array_clara(Node_ma ***array ,Node_ma ***array1,List_nodes_ma *listn, int items,int rand,int sample);
void pam_init(int k,int size,int items,Node_ma ***array,ma_cluster **clusters);
void ma_pam_ass(ma_cluster *clusters,int no_cl,int items, Node_ma **array,int size,int **distance_matrix);
float objective_value(int items, Node_ma **array);
float clara_update(ma_cluster **clusters, int no_cl, int items, Node_ma **array, int size,float original_cost,int **distance_matrix);
float final_cost(float *array,int s);
void cmp_clusters(ma_cluster *clusters,ma_cluster **temp,int no_cl);
#endif