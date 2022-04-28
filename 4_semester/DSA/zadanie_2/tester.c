#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "AVLTree.h"
#include "RedBlackTree.h"
#include "LinkedHash.h"
#include "DoubleHash.h"

//enum pre lepsiu citatelnost pri vybere postupnosti cisel
enum typeLoad { continual = 0, alternation = 1, random = 2 };

void testAVL(int* numbers, int count, int type);
void testRB(int* numbers, int count, int type);
void testLinkedHash(int tableSize, int* numbers, int count, int type);
void testDoubleHash(int* numbers, int count, int type);
int* generateNumbers(int count, int type);

//test pre AVLTree
void testAVL(int* numbers, int count, int type) {
    NODEAVL* pointer = NULL;
    char* types[3] = { "Continual","Alternation","Random" };
    char* string = type == 0 ? types[0] : type == 1 ? types[1] : types[2];
    clock_t t;
    t = clock();
    for (int i = 0; i <= count; i++) {
        root = insertAVLTree(root, *(numbers + i));
    }
    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
    printf("%s insert AVL test trval %f sekund\n", string, time_taken);


    t = clock();
    for (int i = 0; i <= count; i++) {
        pointer = searchAVLTree(root, *(numbers + i));
    }
    t = clock() - t;
    time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
    printf("%s search AVL test trval %f sekund\n", string, time_taken);
    freeAVL(&root);
}

//test pre RedBlackTree
void testRB(int* numbers, int count, int type) {
    char* types[3] = { "Continual","Alternation","Random" };
    char* string = type == 0 ? types[0] : type == 1 ? types[1] : types[2];
    clock_t t;
    struct Node* RBT = NULL;
    t = clock();
    for (int i = 0; i <= count; i++) {
        RBT = RB_insert(RBT, *(numbers + i));
    }
    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
    printf("%s insert RB test trval %f sekund\n", string, time_taken);

    struct Node* pointer;
    t = clock();
    for (int i = 0; i <= count; i++) {
        pointer = BST_search(RBT, *(numbers + i));
    }
    t = clock() - t;
    time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
    printf("%s search RB test trval %f sekund\n", string, time_taken);
}

//test pre LinkedHash
void testLinkedHash(int tableSize,int* numbers,int count,int type) {
    char* types[3] = { "Continual","Alternation","Random" };
    char* string = type == 0 ? types[0]: type == 1 ? types[1] : types[2];
    int* founded = NULL;
    initLinkedHash(tableSize);
    clock_t t;
    t = clock();
    for (int i = 0; i <= count; i++) {
        insertLinkedHash(*(numbers + i));
    }
    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
    printf("%s insert LinkedHash test trval %f sekund\n", string, time_taken);



    t = clock();
    for (int i = 0; i <= count; i++) {
        founded = searchLinkedHash(*(numbers + i));
    }
    t = clock() - t;
    time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
    printf("%s search LinkedHash test trval %f sekund\n", string, time_taken);
    freeLinkedHash();
}

//test pre DoubleHash
void testDoubleHash(int* numbers, int count, int type) {
    HASHTABLE* pointer = NULL;
    int* founded = NULL;
    char* types[3] = { "Continual","Alternation","Random" };
    char* string = type == 0 ? types[0] : type == 1 ? types[1] : types[2];

    clock_t t;
    t = clock();
    for (int i = 0; i <= count; i++) {
        insertDoubleHash(&pointer, *(numbers + i));
    }
    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
    printf("%s insert DoubleHash test trval %f sekund\n", string, time_taken);


    t = clock();
    for (int i = 0; i <= count; i++) {
        founded = searchDoubleHash(&pointer, *(numbers + i));
    }
    t = clock() - t;
    time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
    printf("%s search DoubleHash test trval %f sekund\n", string, time_taken);

    deleteDoubleHash(&pointer);
}

//funkcia na generovanie cisel
int* generateNumbers(int count, int type) {
    int* numbers = (int*)malloc(count * sizeof(int));
    if(numbers){
        if (type == continual) {
            for (int i = 0; i < count; i++) {
                *(numbers + i) = i;
            }
        }
        else if (type == alternation) {
            for (int i = 0, j = count, k = 0; i < count; i += 2, j--, k++) {
                *(numbers + i) = k;
                if(i + 1< count){
                    *(numbers + i+1) = j;
                }
            }
        }
        else if (type == random) {
            srand(time(NULL));
            for (int i = 0; i < count; i++) {
                *(numbers + i) = abs((int)(rand()* rand()));
            }

        }
        return numbers;
    }
    return NULL;
}



int main() {
    int count = 100000; //pocet vlkadanych cisel
    int tableSize = 900019;   //pre LInkedHash (najlepsie prvocislo)
    int* numbers; //pointer pre pole na cisla
    
    numbers = generateNumbers(count, continual); //ziskanie adresy vygenerovaneho pola cisiel
    testAVL(numbers, count, continual);
    testRB(numbers, count, continual);
    testLinkedHash(tableSize, numbers, count, continual);
    testDoubleHash(numbers, count, continual);

    free(numbers);
    
    numbers = generateNumbers(count, alternation);
    testAVL(numbers, count, alternation);
    testRB(numbers, count, alternation);
    testLinkedHash(tableSize, numbers, count, alternation);
    testDoubleHash(numbers, count, alternation);

    free(numbers);
    
    numbers = generateNumbers(count,random);
    testAVL(numbers, count, random);
    testRB(numbers, count, random);
    testLinkedHash(tableSize, numbers, count, random);
    testDoubleHash(numbers, count, random);

    free(numbers);
}