#ifndef STRUCTS_CLARA_COS_H

#define STRUCTS_CLARA_COS_H
#include <stdio.h>

typedef struct Node Node;
typedef struct List_nodes List_nodes;
typedef struct List_pointers List_pointers;
typedef struct cos_cluster cos_cluster;

float cosine_distance(float *point, float *item, int size);
List_nodes* Cosine_input(FILE *fd,int* final_size, int * item);
void init_array_clara(Node ***array ,Node ***array1,List_nodes *listn, int items,int rand,int sample);
void print_array(Node **array, int items);
void init_cos_cl(cos_cluster **clusters,int no_cl);
void free_list_nodes(List_nodes **listn, int size);
void print_clusters(cos_cluster *clusters,int no_cl);
void pam_init(int k,int size,int items,Node ***array,cos_cluster **clusters);
void euc_pam_ass(cos_cluster *clusters,int no_cl,int items, Node **array,int size);
float objective_value(int items, Node **array);
float clara_update(cos_cluster **clusters, int no_cl, int items, Node **array, int size,float original_cost);
float final_cost(float *array,int s);
void cmp_clusters(cos_cluster *clusters,cos_cluster **temp,int no_cl);

#endif