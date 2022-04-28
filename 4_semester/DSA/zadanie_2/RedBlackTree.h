//Created by Adrian Vanco

#ifndef ZADANIE2_REDBLACKTREE_H
#define ZADANIE2_REDBLACKTREE_H



#include <stdio.h>
#include <stdlib.h>

enum type { RED, BLACK };

struct Node
{
    int data;
    struct Node* left;
    struct Node* right;
    struct Node* parent;
    enum type color;
};

struct Queue
{
    struct Node* data;
    struct Queue* next;
};

extern struct Queue* front;
extern struct Queue* rear;


void levelorder(struct Node* root);     //vypis stromu
struct Node* RB_insert(struct Node* T, int data);   //vkladanie do stromu
void preorder(struct Node* root);   //vypis stromu
struct Node* RB_delete(struct Node* T, struct Node* z); //vymazanie zo stromu
struct Node* BST_search(struct Node* root, int x);  //vyhladavanie v strome
#endif //ZADANIE2_REDBLACKTREE_H