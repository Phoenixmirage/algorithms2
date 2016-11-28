#ifndef STRUCTS_COS2_H

#define STRUCTS_COS2_H
#include <stdio.h>


typedef struct Node Node;
typedef struct List_nodes List_nodes;
typedef struct List_pointers List_pointers;
typedef struct cos_cluster cos_cluster;

float cosine_distance(float *point, float *item, int size);
List_nodes* Cosine_input(FILE *fd,int* final_size, int * item);
void init_array(Node ***array,List_nodes *listn, int items);
void print_array(Node **array, int items);
void init_cos_cl(cos_cluster **clusters,int no_cl);
void k_medoids(int k,int size,int items,Node ***array,cos_cluster **clusters);
void free_list_nodes(List_nodes **listn, int size);
void print_clusters(cos_cluster *clusters,int no_cl);

#endif