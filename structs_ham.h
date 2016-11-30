#ifndef STRUCTS2_H

#define STRUCTS2_H
#include <stdio.h>
typedef struct Node_Ham Node_Ham;
typedef struct List_nodes_Ham List_nodes_Ham;
typedef struct List_pointers_Ham List_pointers_Ham;
typedef struct ham_cluster ham_cluster;

int H_hamming(unsigned long long int number, int size, int pos);
long int G_hamming(int **G_h, int no_G, int number, int size, int k);
int hamming_distance(unsigned long long int number1, unsigned long long int number2, int size);
List_nodes_Ham* Hamming_input(FILE *fd,int* final_size, int * item);
void init_hash_Ham(List_pointers_Ham ****hashtable,int size,int k,int L,int hashsize,List_nodes_Ham *listn,int **G_h);
void search_Ham(List_pointers_Ham ***hashtables,FILE *input,List_nodes_Ham *listn,int k,int L,int size,int hashsize,int **G_h,FILE *output);
void free_list_nodes_ham(List_nodes_Ham **listn, int size);
void free_hash_ham(List_pointers_Ham  ****hashtable, int hashsize,int L);
void Ham_init_array(Node_Ham ***array,List_nodes_Ham *listn, int items);
void init_ham_cl(ham_cluster **clusters,int no_cl);
void ham_init_parkjun(Node_Ham **array,int items,int size,ham_cluster **clusters,int no_cl);
void ham_pam_ass(ham_cluster *clusters,int no_cl,int items, Node_Ham **array,int size);
void ham_print_clusters(ham_cluster *clusters,int no_cl);
int ham_clarans(ham_cluster **clusters,int no_cl,int items, Node_Ham **array,int size,int s, int Q);
void ham_silhouette(ham_cluster *clusters, Node_Ham **array,int size,int no_cl,int items);
int ham_Loyds(ham_cluster **clusters, int no_cl, int items, Node_Ham **array, int size);
void ham_k_medoids(int k,int size,int items,Node_Ham ***array,ham_cluster **clusters);
void ham_print_data(FILE *output,ham_cluster *clusters, Node_Ham **array, int size, int items, int no_cl,int flag,double time);
void ham_by_ANN(ham_cluster *clusters,int no_cl,int items, Node_Ham **array,int size,List_pointers_Ham ***hashtables,int k,int L,int hashsize,int **G_h);
void ham_main(FILE *input,FILE *output,int k,int no_cl, int Q, int s,int L,int com);
#endif
