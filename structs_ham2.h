#ifndef STRUCTS_HAM2_H

#define STRUCTS_HAM2_H
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

#endif
