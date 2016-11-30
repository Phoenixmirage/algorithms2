#ifndef STRUCTS_CLARA_HAM_H

#define STRUCTS_CLARA_HAM_H
#include <stdio.h>

typedef struct Node_Ham Node_Ham;
typedef struct List_nodes_Ham List_nodes_Ham;
typedef struct List_pointers_Ham List_pointers_Ham;
typedef struct ham_cluster ham_cluster;

int hamming_distance(unsigned long long int number1, unsigned long long int number2, int size);
List_nodes_Ham* Hamming_input(FILE *fd,int* final_size, int * item);
void init_array_clara(Node_Ham ***array ,Node_Ham ***array1,List_nodes_Ham *listn, int items,int rand,int sample);
void free_list_nodes_ham(List_nodes_Ham **listn, int size);
void print_clusters(ham_cluster *clusters,int no_cl);
void print_array(Node_Ham **array, int items);
void init_ham_cl(ham_cluster **clusters,int no_cl);
void pam_init(int k,int size,int items,Node_Ham ***array,ham_cluster **clusters);
void ham_pam_ass(ham_cluster *clusters,int no_cl,int items, Node_Ham **array,int size);
float objective_value(int items, Node_Ham **array);
float clara_update(ham_cluster **clusters, int no_cl, int items, Node_Ham **array, int size,float original_cost);
float final_cost(float *array,int s);
void cmp_clusters(ham_cluster *clusters,ham_cluster **temp,int no_cl);


#endif
