//Created by Adrian Vanco FIIT-STU ID: 103171

#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "BDD.h"

char* rand_string(char* str, int size);
void Decimal_to_binary_string(int number, int array_size, char* variables);
void test_all(int numberOfVariables, int iter, int use);



//generator stringov o pozadovanych rozmeroch
char* rand_string(char* str, int size)
{
	const char charset[] = "01101010101000000111111";
	if (size) {
		--size;
		for (int i = 0; i < size; i++) {
			int key = rand() % (int)(sizeof charset - 1);
			str[i] = charset[key];
		}
		str[size] = '\0';
	}
	return str;
}

//do pola variables o presnej velkosti, ktore je vo funkcii test_all vlozi binarne dane cislo
void Decimal_to_binary_string(int number, int array_size, char* variables) {
	int bit;
	int index = array_size - 2;	//posledny index kde sa bude vkladat '1' alebo '0'
	if (variables != NULL) {
		for (int i = 0; i < array_size - 1 ; i++) {
			bit = number >> i;	//binarny posun
			if (bit & 1) {	//ak je po posune na kraji jednotka ulozime '1' inak '0'
				*(variables + index) = '1';
			}
			else {
				*(variables + index) = '0';
			}
			index--;
		}
		*(variables + array_size - 1) = '\0';
	}
	else {
		printf("Chyba pri decimal to binary string");
	}
}


void test_all(int numberOfVariables,int iter,int use) {
	int numberOfNodes = pow(2, numberOfVariables);
	//printf("totok %d\n", numberOfNodes);
	char* f = (char*)malloc(numberOfNodes + 1);
	char* variables = (char*)malloc(numberOfVariables + 1);
	BDD* bdd;
	clock_t t;
	double time_taken;
	BF* funkcia = (BF*)malloc(sizeof(BF));
	for(int i = 0;i < iter; i++){
		rand_string(f, numberOfNodes + 1);

		if (funkcia && f) {
			funkcia->vector = f;
			funkcia->size = strlen(funkcia->vector);

			printf("==================================\n");
			//printf("%s\n", f);

	//========================BDD_create==========================

			t = clock();
			bdd = BDD_create(funkcia);
			t = clock() - t;
			time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
			printf("Create pre %d premennych trval %f sekund\n", numberOfVariables, time_taken);
			//printf("Vysledok : %c\n", BDD_use(bdd, "111"));
			printf("Pocet buniek %d \n", bdd->numberOfNodes);


	//========================BDD_use=============================
			
			if (use) {	//ak cheme pouzit use
				t = clock();
				for (int j = 0; j < numberOfNodes; j++) {	//cyklus pouzije vsetky kombinacie premennych 
					Decimal_to_binary_string(j, numberOfVariables + 1, variables);	
					//printf("using %s\n", variables);
					BDD_use(bdd, variables);
				}
				t = clock() - t;
				time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
				printf("Use %d krat, pred reduce trval %f sekund\n", numberOfNodes, time_taken);
			}


	//========================BDD_reduce==========================

			t = clock();
			BDD_reduce(bdd);
			t = clock() - t;
			time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
			printf("Reduce pre %d premennych trval %f sekund\n", numberOfVariables, time_taken);
			printf("Pocet buniek po %d \n", bdd->numberOfNodes);


	//========================BDD_use=============================

			if (use) {	//ak cheme pouzit use
				t = clock();
				for (int j = 0; j < numberOfNodes; j++) {	//cyklus pouzije vsetky kombinacie premennych 
					Decimal_to_binary_string(j, numberOfVariables + 1, variables);
					//printf("using %s\n", variables);
					BDD_use(bdd, variables);
				}
				t = clock() - t;
				time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
				printf("Use %d krat, po reduce trval %f sekund\n", numberOfNodes, time_taken);
			}
			
			printf("==================================\n\n");



			BDD_free(bdd);
		}
	}
}


int main() {

	int numberOfVariables = 13;
	int iterations = 2000;
	int use = 1;
	test_all(numberOfVariables, iterations, use);

	/*
	for (int i = 0; i < 2000; i++) {
		test_all(i%17,1,0);
	}
	*/

	//BDD_print(bdd->root);

	return 0;
}