#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "LinkedHash.h"

//deklaracie funkcii
void initLinkedHash(int size);
int sizeOfArrayLinkedHash();
void insertLinkedHash(int data);
void freeOldLinkedHash(ARRAYNODE** array, int size);
void freeLinkedHash();
int* searchLinkedHash(int data);
bool isPrimeLinkedHash(int number);
int getNextPrimeLinkedHash(int number);
void rehashLinkedHash();
int hashLinkedHash(int x);
void printLinkedHash();

//inicializacia tabulky
void initLinkedHash(int size) {
	arraySize = getNextPrimeLinkedHash(size);		//nastavenie velkosti tabulky na najblizsie prvocislo na pravo od zadanej velkosti
	array = (ARRAYNODE*)malloc(arraySize * sizeof(ARRAYNODE));	//alokovanie tabulky
	if(array)
	for (int i = 0; i < arraySize; i++) {		//vynulovanie tabulky
		(array + i)->count = 0;
		(array + i)->list = NULL;
	}
}

//ziskanie velkosti tabulky
int sizeOfArrayLinkedHash() {
	return arraySize;
}

//vkladanie prvku do tabulky
void insertLinkedHash(int data) {
	float alfa = 0;		//faktor naplnenia
	int index = hashLinkedHash(data);	//ziskanie indexu v tabulke
	NODEDATA* list = (array + index)->list;		//ziskanie listnu z indexu v tabulke

	//kontrola zoznamu pre danu hash hodnotu
	if (list == NULL) {
		//ak je zoznam prazdny vlozi prvok
		NODEDATA* item = (NODEDATA*)malloc(sizeof(NODEDATA));		//alokovanie prvku
		if (item) {
			item->data = data;		//ulozenie vkladanej hodnoty
			item->next = NULL;

			(array + index)->list = item;		//vlozenie prvku do listu
			(array + index)->count = 1;		//nastavi pocet v prvkov v liste na jeden  
		}
	}
	else {
		//kontrola ci su data v tabulke
		int *inTable = searchLinkedHash(data);
		if (inTable == NULL) {
			//data neboli v tabulke tak vlozi prvok na zaciatok
			NODEDATA* item = (NODEDATA*)malloc(sizeof(NODEDATA));
			if(item){
				item->data = data;
				item->next = NULL;

				item->next = list;
				(array + index)->list = item;
				(array + index)->count += 1;		//zvysi pocet prvok v liste

				//kontrola ci netreba zvacsit tabulku
				alfa = (float)(1.0 * (array + index)->count / sizeOfArrayLinkedHash());
				if (alfa >= 0.1) {
					//printf("\nrehash!\n");
					rehashLinkedHash();
				}
			}
		}
		else {
			return;		//data su uz v tabulke
		}
	}
}

//uvolnenie starej tabulky
void freeOldLinkedHash(ARRAYNODE** array,int size) {
	NODEDATA* list = NULL;
	NODEDATA* temp = NULL;
	if (*array == NULL) return;
	for (int i = 0; i < size; i++) {

		list = (*array + i)->list;
		if (list != NULL) {
			while (list->next != NULL) {
				temp = list;
				list = list->next;
				free(temp);
			}
			free(list);
		}
	}
	free(*array);
}

//uvolnenie tabulky
void freeLinkedHash() {
	NODEDATA* list = NULL;
	NODEDATA* temp = NULL;
	for (int i = 0; i < sizeOfArrayLinkedHash(); i++) {
		list = (array + i)->list;
		if (list != NULL) {
			while (list->next != NULL) {
				temp = list;
				list = list->next;
				free(temp);
			}
			free(list);
		}
	}
	free(array);
}

//hladanie prvku v tabulke
int* searchLinkedHash(int data) {
	int index = hashLinkedHash(data);		//ziskanie indexu v tabulke
	NODEDATA* temp = (array + index)->list;		//ziskanie listnu z indexu v tabulke
	int count = (array + index)->count;		//ziskanie poctu prvkov v liste

	//prechadzanie listu
	for (int i = 0; i < count; i++) {
		if (temp->data == data) {
			return &temp->data;
		}
		temp = temp->next;
	}
	return NULL;
}

//zisti ci cislo je prvocislo
bool isPrimeLinkedHash(int number){
	for (int i = 2; i <= number / 2; ++i) {
		// condition for non-prime
		if (number % i == 0) {
			return false;
		}
	}
	return true;
}

//vrati nablizsie prvocislo na pravo od zadaneho cisla
int getNextPrimeLinkedHash(int number) {
	while(!isPrimeLinkedHash(++number)){}
	return number;
}

//zvacsenie tabulky
void rehashLinkedHash() {
	ARRAYNODE* temp = array;
	NODEDATA* list = NULL;
	int oldSize = sizeOfArrayLinkedHash();		//ziskanie velkosti aktualnej tabulky
	int newSize = getNextPrimeLinkedHash(2 * oldSize);	//ziskanie novej velkosti (velkost bude vzdy prvocislo)
	initLinkedHash(newSize);	//inicializovanie novej tabulky

	//presuvanie hodnot zo starej tabulky do novej vacsej tabulky 
	for (int i = 0; i < oldSize; i++) {

		list = (temp + i)->list;

		if (list == NULL) {
			continue;
		}
		else {
			while (list != NULL) {
				insertLinkedHash(list->data);
				list = list->next;
			}
		}
	}

	freeOldLinkedHash(&temp,oldSize);	//uvolnenie starej tabulky
}

//vracia index, kam sa ma prvok vlozit
int hashLinkedHash(int x) {
	int a = 7;
	int b = 3;
	return(abs(a*x + b) % arraySize);
}

//vypis tabulky
void printLinkedHash() {
	NODEDATA* temp = NULL;
	for (int i = 0; i < sizeOfArrayLinkedHash(); i++) {

		temp = (array + i)->list;
		printf("%d: ", i);

		while (temp != NULL) {
			printf("%d ->", temp->data);
			temp = temp->next;
		}

		printf("\n");
	}
	printf("\n");
}