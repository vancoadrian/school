#include <stdlib.h>
#include <stdio.h>

typedef struct NodeAVL {
	int data;
	int height;
	struct NodeAVL* left;
	struct NodeAVL* right;
}NODEAVL;

NODEAVL* createNODEAVL(int data);
int height(NODEAVL* node);
int maximum(int a, int b);
int getBalanceFactor(NODEAVL* node);
NODEAVL* leftRotate(NODEAVL* node);	
NODEAVL* rightRotate(NODEAVL* node);
NODEAVL* insertAVLTree(NODEAVL* node, int data);
NODEAVL* searchAVLTree(NODEAVL* node, int data);
void inOrder(NODEAVL* node);
void freeAVL(NODEAVL** node);

//funkcia vracia adresu vytvorenej struktury s datami
NODEAVL* createNODEAVL(int data) {
	NODEAVL* newNode = (NODEAVL*)malloc(sizeof(NODEAVL));
	if(newNode){
	newNode->data = data;
	newNode->height = 1;
	newNode->left = NULL;
	newNode->right = NULL;
	}
	return newNode;
}

//funkcia na ziskanie vysky z prvku
int height(NODEAVL* node){
	if (node == NULL){
		return 0;
	}
	else {
		return node->height;
	}
}

//funkcia vrati vacsie cislo
int maximum(int a, int b){
	if (a > b) {
		return a;
	}
	else {
		return b;
	} 
}

//ziskanie faktoru na balancovanie
int getBalanceFactor(NODEAVL* node){
	return height(node->right) - height(node->left);
}

NODEAVL* leftRotate(NODEAVL* node) {
	NODEAVL* right = node->right;	//prave dieta prvku
	NODEAVL* rightLeftChild = right->left; //lave dieta praveho dietata

	//vymena
	right->left = node;
	node->right = rightLeftChild;

	//uprava vysiek
	node->height =  1 + maximum(height(node->left), height(node->right));
	right->height = 1 + maximum(height(right->left), height(right->right));

	return right;
}

NODEAVL* rightRotate(NODEAVL* node) {
	NODEAVL* left = node->left;;	//lave dieta prvku
	NODEAVL* leftRightChild = left->right;	//prave dieta laveho dietata

	//vymena
	left->right = node;
	node->left = leftRightChild;

	//uprava vysiek
	node->height = 1 + maximum(height(node->left), height(node->right));
	left->height = 1 + maximum(height(left->left), height(left->right));

	return left;
}

//rekurzívna funkcia na vkladanie do stromu
NODEAVL* insertAVLTree(NODEAVL* node, int data) {

	if (node == NULL) {	//vytvori vrchol 
		return createNODEAVL(data);
	}

	if (data < node->data) {		//vkladanie do laveho podstromu
		node->left = insertAVLTree(node->left, data);
	}
	else if (data > node->data) {	//vkladanie do praveho podstromu
		node->right = insertAVLTree(node->right, data);
	}
	else {	//hodnoty sa nasla, nevklada sa
		return node;
	}

	//zvacsovanie vysky
	node->height = 1 + maximum(height(node->left), height(node->right));

	//faktor balancovania
	int bf = getBalanceFactor(node);	

	//jednoducha rotacia dolava (Right Right)
	if (bf > 1 && data > node->right->data) {
		//printf("balancovanie j dolava\n");
		return leftRotate(node);
	}

	//jednoducha rotacia doprava (Left Left)
	if (bf < -1 && data < node->left->data) {
		//printf("balancovanie j doprava\n");
		return rightRotate(node);
	}
	
	//rotacia doprava a dolava (Right Left)
	if (bf > 1 && data < node->right->data) {
		//printf("balancovanie neviem\n");
		node->right = rightRotate(node->right);
		return leftRotate(node);
	}
	//rotacia dolava a doprava (Left Right)
	if (bf < -1 && data > node->left->data) {
		//printf("balancovanie neviem 2\n");
		node->left = leftRotate(node->left);
		return rightRotate(node);
	}
	return node;
}

//hladanie prvku v strome
NODEAVL* searchAVLTree(NODEAVL* node, int data) {
	while (node != NULL) {
		if (node->data == data) {
			return node;
		}
		else if (node->data > data) {
			node = node->left;
		}
		else {
			node = node->right;
		}
	}
	return NULL;
}

//uvolnenie stromu
void freeAVL(NODEAVL** node){
	if ((*node)->right != NULL) {
		//vymaz pravy podstrom
		freeAVL(&((*node)->right));
		(*node)->right = NULL;
	}
	if ((*node)->left != NULL) {
		//vymaz lavy podstrom
		freeAVL(&((*node)->left));
		(*node)->left = NULL;
	}
	//uvolni aktualny vrchol
	free((*node));
	*node = NULL;
}

//vypis stromu 
void inOrder(NODEAVL* node)
{
	if (node != NULL)
	{
		inOrder(node->left);
		printf("%d %d ", node->data, node->height);
		inOrder(node->right);
	}
}