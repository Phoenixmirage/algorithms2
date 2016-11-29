#ifndef STRUCTS4_H

#define STRUCTS4_H
#include <stdio.h>

float marsaglia(void);
unsigned long long int string_to_int(char *string,int size);
void initG_h(int ***G_h, int k, int L, int w, int size);
void freeG_h(int ***G_h,int L);
int configuration(FILE *config,int *Q,int *k,int *L,int *no_cl,int *s);

#endif
