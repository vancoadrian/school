#ifndef AVL_TREE_H
#define AVL_TREE_H

typedef struct NodeAVL {
	int data;	//hodnota, ktoru chcem ulozit
	int height;	//vyska prvku
	struct NodeAVL* left;
	struct NodeAVL* right;
}NODEAVL;

NODEAVL* root = NULL;

NODEAVL* createNODEAVL(int data);	//vrati vytvoreny prvok
int height(NODEAVL* node);	//vrati vysku prvku
int maximum(int a, int b);	//vrati vacsie cislo
int getBalanceFactor(NODEAVL* node);	//vrati hodnotu faktoru balancovania
NODEAVL* leftRotate(NODEAVL* node);	
NODEAVL* rightRotate(NODEAVL* node);
NODEAVL* insertAVLTree(NODEAVL* node, int data);	//vlozi do avl stromu
NODEAVL* searchAVLTree(NODEAVL* node, int data);	//vyhlada v avl strome
void inOrder(NODEAVL* node);	//vypis stromu
void freeAVL(NODEAVL** node);	//uvolnenie stromu
#endif