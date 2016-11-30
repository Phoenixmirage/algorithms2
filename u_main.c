#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include "structs.h"
#include "structs_cos.h"
#include "structs_ham.h"
#include "structs_ma.h"
#include "functions.h"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>


int init_suite(void)
{
 return 0;
}

int clean_suite(void)
{
 return 0;
}

void Euclidean_input_test(void){
	List_nodes *list;
	int size, items;
	FILE *fd;
	fd=fopen("file.csv", "w");
	fprintf(fd,"item1 1 2 3 4\n");
	fprintf(fd,"item2 3 1 2 4\n");
	fclose(fd);
	fd=fopen("file.csv", "r");
	fclose(stdout);
	list=Euclidean_input(fd,&size,&items);
	CU_ASSERT_EQUAL(size,4);
	CU_ASSERT_EQUAL(items,2);
	fclose(fd);
	remove("file.csv");
	freopen("/dev/tty","a",stdout);
}

void Cosine_input_test(void){
	List_nodes_cos *list;
	int size, items;
	FILE *fd;
	fd=fopen("file.csv", "w");
	fprintf(fd,"item1 1 2 3 4\n");
	fprintf(fd,"item2 3 1 2 4\n");
	fclose(fd);
	fd=fopen("file.csv", "r");
	fclose(stdout);
	list=Cosine_input(fd,&size,&items);
	CU_ASSERT_EQUAL(size,4);
	CU_ASSERT_EQUAL(items,2);
	fclose(fd);
	remove("file.csv");
	freopen("/dev/tty","a",stdout);
}

void Hamming_input_test(void){
	List_nodes_Ham *list;
	int size, items;
	FILE *fd;
	fd=fopen("file.csv", "w");
	fprintf(fd,"item1 10110010\n");
	fprintf(fd,"item2 01000000\n");
	fprintf(fd,"item3 01010000\n");
	fclose(fd);
	fd=fopen("file.csv", "r");
	fclose(stdout);
	list=Hamming_input(fd,&size,&items);
	CU_ASSERT_EQUAL(size,8);
	CU_ASSERT_EQUAL(items,3);
	fclose(fd);
	remove("file.csv");
	freopen("/dev/tty","a",stdout);
}

void Matrix_input_test(void){
	List_nodes_ma *list;
	int size, items;
	FILE *fd;
	fd=fopen("file.csv", "w");
	fprintf(fd,"@items item1,item2,item3,item4\n");
	fprintf(fd,"0 1 4 5\n");
	fprintf(fd,"1 0 7 3\n");
	fprintf(fd,"4 7 0 6\n");
	fprintf(fd,"5 3 6 0\n");
	fclose(fd);
	fd=fopen("file.csv", "r");
	fclose(stdout);
	int **matrix_array;
	list=matrix_input(fd,&size,&items,&matrix_array);
	CU_ASSERT_EQUAL(size,4);
	CU_ASSERT_EQUAL(items,4);
	fclose(fd);
	remove("file.csv");
	freopen("/dev/tty","a",stdout);
}

void string_int_test(void){
	char str[10]= "00101011";
	unsigned long long int number= string_to_int(str,8);
	CU_ASSERT_EQUAL(number,43);
}

void eu_dist_test(void){
	float point1[4];
	float point2[4];
	point1[0]=1;
	point1[1]=3;
	point1[2]=4;
	point1[3]=5;
	point2[0]=3;
	point2[1]=2;
	point2[2]=2;
	point2[3]=1;
	float number= euclidean_distance(point1,point2,4);
	CU_ASSERT_EQUAL(number,5);
}

void cos_dist_test(void){
	float point1[4];
	float point2[4];
	point1[0]=1;
	point1[1]=3;
	point1[2]=-4;
	point1[3]=5;
	point2[0]=-5;
	point2[1]=4;
	point2[2]=3;
	point2[3]=1;
	float number= cosine_distance(point1,point2,4);
	CU_ASSERT_EQUAL(number,1);
}

void ham_dist_test(void){
	unsigned long long int number1=43;
	unsigned long long int number2=42;
	unsigned long long int dist=hamming_distance(number1,number2,8); 
	CU_ASSERT_EQUAL(dist,1);
}

void ham_H_test(void){
	unsigned long long int number1=43;
	int dist=H_hamming(number1,8,5); 
	CU_ASSERT_EQUAL(dist,1);
}

void ham_G_test(void){
	int **G_h;
	G_h=malloc(2*sizeof(int*));
	int i;
	for (i=0;i<2;i++){
		G_h[i]=malloc(3*sizeof(int));
	}
	G_h[0][0]=3;
	G_h[0][1]=4;
	G_h[0][2]=1;
	G_h[1][0]=7;
	G_h[1][1]=1;
	G_h[1][2]=6;
	unsigned long long int number1=43;
	long int dist=G_hamming(G_h,1,number1,8,3); 
	CU_ASSERT_EQUAL(dist,4);
	for (i=0;i<2;i++){
		free(G_h[i]);
	}
	free(G_h);
}

void configuration_test(void){
	int k,L,s,Q,no_cl;
	FILE *fd;
	fd=fopen("file.csv", "w");
	fprintf(fd,"number_of_clusters: 5\n");
	fprintf(fd,"number_of_hash_tables: 6\n");
	fprintf(fd,"clarans_set_fraction: 300\n");
	fclose(fd);
	fd=fopen("file.csv", "r");
	int ok=configuration(fd,&Q,&k,&L,&no_cl,&s);
	CU_ASSERT_EQUAL(no_cl,5);
	CU_ASSERT_EQUAL(L,6);
	CU_ASSERT_EQUAL(Q,300);
	CU_ASSERT_EQUAL(k,4);
	CU_ASSERT_EQUAL(s,2);
	fclose(fd);
	remove("file.csv");
}

	

int main(int argc, char** argv)
{

 /* UNIT TESTING STARTS */

 CU_pSuite pSuite = NULL;

 /* initialize the CUnit test registry */

 if(CUE_SUCCESS != CU_initialize_registry())
  return CU_get_error();

 /* add a suite to the registry */

 pSuite = CU_add_suite("test_suite", init_suite, clean_suite);
 if(NULL == pSuite)
 {
  CU_cleanup_registry();
  return CU_get_error();
 }

/* Here you add the tests to the suite */

if((NULL == CU_add_test(pSuite,"Euclidean_input_test", Euclidean_input_test))||
(NULL == CU_add_test(pSuite,"Cosine_input_test", Cosine_input_test))||
(NULL == CU_add_test(pSuite,"Hamming_input_test", Hamming_input_test))||
(NULL == CU_add_test(pSuite,"Matrix_input_test", Matrix_input_test))||
(NULL == CU_add_test(pSuite,"string_int_test", string_int_test))||
(NULL == CU_add_test(pSuite,"eu_dist_test", eu_dist_test))||
(NULL == CU_add_test(pSuite,"cos_dist_test", cos_dist_test))||
(NULL == CU_add_test(pSuite,"ham_dist_test", ham_dist_test))||
(NULL == CU_add_test(pSuite,"ham_H_test", ham_H_test))||
(NULL == CU_add_test(pSuite,"ham_G_test", ham_G_test))||
(NULL == CU_add_test(pSuite,"configuration_test", configuration_test)))
{
  CU_cleanup_registry();
  return CU_get_error();
}


 /* Run all tests using the basic interface */

 CU_basic_set_mode(CU_BRM_VERBOSE);
 CU_basic_run_tests();
 printf("\n");
 CU_basic_show_failures(CU_get_failure_list());
 printf("\n\n");

 CU_cleanup_registry();


 /* UNIT TESTING ENDS */

 return CU_get_error();


}

