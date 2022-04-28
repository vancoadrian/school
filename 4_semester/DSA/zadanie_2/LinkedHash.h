#ifndef LINKED_HASH_H
#define LINKED_HASH_H

//bunka listu
typedef struct NodeData {
	int data;	//hodnota, ktoru chcem ulozit
	struct NodeData* next;
}NODEDATA;

//bunka tabulky
typedef struct ArrayNode {
	int count;		//pocet prvkov v liste
	struct NodeData* list;
}ARRAYNODE;

ARRAYNODE* array;	//ukazovatel na tabulku
int arraySize;	//velkost tabulky

void initLinkedHash(int size);		//vytvori tabulku
int sizeOfArrayLinkedHash();		//vrati velkosti tabulky
void insertLinkedHash(int data);	//vlozi do tabulky
void freeOldLinkedHash(ARRAYNODE** array, int size);	//uvolni staru tabulku
void freeLinkedHash();		//uvolni tabulku
int* searchLinkedHash(int data);	//vyhlada data v tabulke
bool isPrimeLinkedHash(int number);	//zisti ci cislo je prvocislo
int getNextPrimeLinkedHash(int number);	//vrati najblizsie prvocislo na pravo
void rehashLinkedHash();	//zvacsi tabulku
int hashLinkedHash(int x);	//zahašuje hodnotu a vrati poziciu v tabulke
void printLinkedHash();	//vypise celu tabulku

#endif