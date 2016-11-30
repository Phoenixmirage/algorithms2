#ifndef STRUCTS_CLARA_H

#define STRUCTS_CLARA_H
#include <stdio.h>

typedef struct Node Node;
typedef struct List_nodes List_nodes;
typedef struct List_pointers List_pointers;
typedef struct euc_cluster euc_cluster;

List_nodes* Euclidean_input(FILE *fd,int* final_size, int * item);
void init_array_clara(Node ***array ,Node ***array1,List_nodes *listn, int items,int rand,int sample);
float euclidean_distance(float *point, float *item, int size);
void print_array(Node **array, int items);
void init_euc_cl(euc_cluster **clusters,int no_cl);
void free_list_nodes(List_nodes **listn, int size);
void print_clusters(euc_cluster *clusters,int no_cl);
void pam_init(int k,int size,int items,Node ***array,euc_cluster **clusters);
void euc_pam_ass(euc_cluster *clusters,int no_cl,int items, Node **array,int size);
float objective_value(int items, Node **array);
float clara_update(euc_cluster **clusters, int no_cl, int items, Node **array, int size,float original_cost);
float final_cost(float *array,int s);
void cmp_clusters(euc_cluster *clusters,euc_cluster **temp,int no_cl);
#endif