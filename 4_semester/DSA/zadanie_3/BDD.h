//Created by Adrian Vanco FIIT-STU ID: 103171

#ifndef BDD_H
#define BDD_H

//bunka funkcie
typedef struct bf {
	int size;
	char* vector;
}BF;

//bunka BDD
typedef struct bdd {
	int numberOfVariables;
	int numberOfNodes;
	struct node* root;
}BDD;

//bunka v BDD
typedef struct node {
	struct bf* nodeVector;
	struct node* left;
	struct node* right;
	struct node* nextRight;	//ukazovatel na dalsi na pravo v rovnakej urovni
}NODE;

//==========deklaracie funkcii=============

BF* BF_create(int size, char* vector);
NODE* NODE_create(BF* bfunction);
void BDD_fill(NODE** node, BF* bfunction);
int BF_check(BF* bfunction);
BDD* BDD_create(BF* bfunkcia);

void BDD_nextRight_connect_next(NODE* node, int iter);
void BDD_nextRight_connect_over_unwanted(NODE* node, NODE* next);
void BDD_reduce_siblings(NODE* node, int* counter);
void BDD_delete_unwanted(NODE** node, int* counter, NODE* to_free, NODE* root);
void NODE_change_every_parent(NODE* parent, NODE* new_child, NODE* old_child);
void BDD_reduce_level(NODE* node, int* counter, NODE* root);
void BDD_connect_levels(NODE* node);
int BDD_reduce(BDD* bdd);

char BDD_use(BDD* bdd, char* vstupy);

void NODES_free(NODE* node);
void BDD_free(BDD* bdd);
void BDD_free_level(NODE* root, NODE* node);
void BDD_free_nodes(NODE* root, int root_size);

void BDD_print(NODE* node);

//========================================

//=============BDD_create=================

//funkcia na vytvorenie bunky pre vektor
BF* BF_create(int size, char* vector) {

	BF* temp = (BF*)malloc(sizeof(BF));
	if (temp) {
		temp->size = size;
		temp->vector = (char*)calloc(size + 1, sizeof(char));
		if (temp->vector) {
			strcpy(temp->vector, vector);
		}
		else {
			printf("Chyba pri BF_create!");
		}
		return temp;
	}
	else {
		printf("Chyba pri alokacii BF_create!");
		return NULL;
	}
}

//funkcia na vytvorenie bunky v BDD
NODE* NODE_create(BF* bfunction) {

	NODE* temp = (NODE*)malloc(sizeof(NODE));
	if (temp) {
		temp->nodeVector = bfunction;
		temp->left = NULL;
		temp->right = NULL;
		temp->nextRight = NULL;
		return temp;
	}
	else {
		printf("Chyba pri alokacii NODE_create!");
		return NULL;
	}
}

//rekurzivna funkcia na naplnenie BDD
void BDD_fill(NODE** node, BF* bfunction) {

	if (*node == NULL) {
		*node = NODE_create(bfunction);	//pridanie NODU do BDD  funkcia vrati hotovy NODE s danym vektorom
	}

	NODE* temp = *node;
	int nextSize = bfunction->size / 2;	//zistenie velkosti pre nasledujuce vektory je to polovica predchadzajuceho lebo sa vektor rozdeli
	char* leftVector = (char*)calloc(nextSize + 1, sizeof(char));	//alokacia nasledujuceho laveho vektora
	char* rightVector = (char*)calloc(nextSize + 1, sizeof(char));	//alokacia nasledujuceho praveho vektora

	//kontrola alokacie
	if (leftVector) {
		strncpy(leftVector, bfunction->vector, nextSize);	//nacitanie laveho vektora
	}
	else {
		printf("Chyba, nealokovane  v BDD_fill");
	}

	//kontrola alokacie
	if (rightVector) {
		strncpy(rightVector, bfunction->vector + nextSize, nextSize);	//nacitanie praveho vektora
	}
	else {
		printf("Chyba, nealokovane v BDD_fill");
	}

	//vytvorenie vektorov do NODOV
	BF* leftBF = BF_create(nextSize, leftVector);	
	BF* rightBF = BF_create(nextSize, rightVector);	

	//kontrola ci sme uz neprisli na koniec ak ano vyskocime z rekurzie
	if (nextSize != 0) {
		BDD_fill(&temp->left, leftBF);	//vnaranie
		BDD_fill(&temp->right, rightBF);
	}
}

//funkcia na overenie ci sa vstupna funkcia sklada len z 0 a 1; 
int BF_check(BF* bfunction) {
	for (int i = 0; i < bfunction->size; i++) {
		if (bfunction->vector[i] != '1' && bfunction->vector[i] != '0') {
			return 0;
		}
	}
	return 1;
}

//funkcia na vytvorenie BDD
BDD* BDD_create(BF* bfunkcia) {

	BDD* temp = (BDD*)malloc(sizeof(BDD));	//alokuje sa BDD
	//osetrenie èi je vektor správny 
	if (bfunkcia == NULL || temp == NULL || (bfunkcia->size) == 0 || (bfunkcia->size) % 2 != 0 || BF_check(bfunkcia) == 0) {
		printf("Zle zadany vektor alebo chyba alokacie!\n");
		return NULL;
	}

	BF* bfunction = BF_create(bfunkcia->size, bfunkcia->vector);	//vytvara sa kopia vstupnej funkcie pretoze, pri teste nechcem aby som uvolnil tu, ktoru pouzivam v cykle

	temp->numberOfVariables = ((int)log2(bfunction->size));	//poèet premenných
	temp->numberOfNodes = (bfunction->size * 2) - 1;	//poèet prvkov
	temp->root = NULL;
	
	BDD_fill(&temp->root, bfunction);	//naplnenie BDD

	return temp;	//vratenie BDD
}
//========================================


//==============BDD_reduce================

//funkcia na obidenie stromu na pravo
void BDD_nextRight_connect_next(NODE* node, int iter) {
	if (node != NULL) {
		int i = 0;
		NODE* temp = node->nextRight;
		while (temp != NULL && i < iter) {
			temp = temp->nextRight;
			i++;
		}
		node->nextRight = temp;
		BDD_nextRight_connect_next(node->right, iter * 2);
	}
}

//funkcia na obidenie NODU v urovni, ktory sa ide uvolnovat
void BDD_nextRight_connect_over_unwanted(NODE* node, NODE* next) {
	if (node != NULL) {
		if (node->nextRight == next) {
			//printf("prepajam  %s %s \n", node->nodeVector->vector,next->nodeVector->vector);
			node->nextRight = next->nextRight;
		}
		else {
			BDD_nextRight_connect_over_unwanted(node->left, next);
			BDD_nextRight_connect_over_unwanted(node->right, next);
		}
	}
}

//funkcia na redukciu vektorovo rovnakych potomkov ide sa od vrchola a postupne sa vnara
void BDD_reduce_siblings(NODE* node, int* counter) {
	NODE* temp;
	if (node != NULL) {
		if (node->left != NULL && node->right != NULL) {
			//ak sme nasli zhodu
			if (!strcmp(node->left->nodeVector->vector, node->right->nodeVector->vector)) {
				BDD_nextRight_connect_next(node->left, 1);	//premostime
				temp = node->right;
				node->right = node->left;	//dame rovnakého potomka
				//uvolni cely strom potomka
				NODES_free(temp);
				(*counter) = (*counter) + node->nodeVector->size - 1;	//priratame kolko sme uvolnili
				BDD_reduce_siblings(node->left, counter);	//a dalej sa vnarame
			}
			else {
				BDD_reduce_siblings(node->left, counter);	//vnaranie
				BDD_reduce_siblings(node->right, counter);
			}
		}
	}
}

//funkcia na odstranovanie rodicov, ktori maju laveho a praveho potomka toho sameho
void BDD_delete_unwanted(NODE** node, int* counter, NODE* to_free, NODE* root) {
	NODE* temp = *node;

	if (temp != NULL) {
		if (temp->left != NULL && temp->right != NULL) {
			if (temp->left == temp->right) {
				BDD_nextRight_connect_over_unwanted(root, temp);
				//prepisem v ukazovatelovi adresu
				*node = (*node)->left;

				BDD_delete_unwanted(node, counter, temp, root);
				
			}
			else {
				//vnaranie
				BDD_delete_unwanted(&(*node)->left, counter, NULL, root);
				BDD_delete_unwanted(&(*node)->right, counter, NULL, root);
			}
		}
		//uvolnovanie prvku v nasledujucom vnoreni
		if (to_free) {
			free(to_free->nodeVector);
			free(to_free);
			(*counter)++;
		}
	}
}

//funkcia na zmenu rodicov, ktore ukazuju na stareho potomka, ktory sa pojde uvolnovat
void NODE_change_every_parent(NODE* parent, NODE* new_child, NODE* old_child) {
	if (parent != NULL) {
		if (parent->left == old_child) {	//ak bol vlavo prepiseme a este musime prezriet pravu stranu
			parent->left = new_child;
			NODE_change_every_parent(parent->right, new_child, old_child);
		}
		else if (parent->right == old_child) {	//tak isto ale naopak ako pri lavej strane
			parent->right = new_child;
			NODE_change_every_parent(parent->left, new_child, old_child);
		}
		else {
			NODE_change_every_parent(parent->left, new_child, old_child);
			NODE_change_every_parent(parent->right, new_child, old_child);
		}
	}
}

//funkcia na vratenie najlavejsieho prvku v urovni
NODE* search_first_from_left(NODE* node, int size) {
	if (node != NULL) {
		if (node->nodeVector->size == size) {
			return node;	//ak sme nasli vynarame sa
		}
		else {
			NODE* temp = search_first_from_left(node->left, size);	//najprv sa vnaram vlavo 
			if (temp == NULL) {	//ak sa nic nenaslo vlavo
				temp = search_first_from_left(node->right, size);	//vnorenie sa vpravo
			}
			return temp;
		}
	}
	else {
		return NULL;	//ak sme prisli na koniec a nenasli sme
	}
}

//funkcia na redukciu rovnakych NODOV v danej urovni
void BDD_reduce_level(NODE* node, int* counter, NODE* root) {
	NODE* temp;
	NODE* temp_prev;
	NODE* temp_holding;
	NODE* for_free;
	if (node != NULL) {
		temp_holding = node;
		temp_prev = temp_holding;
		//prechadzanie urovne
		while (temp_holding != NULL) {
			temp = temp_holding;
			while (temp != NULL) {
				//hladanie rovnakeho prvku
				if (temp_holding != temp && !strcmp(temp_holding->nodeVector->vector, temp->nodeVector->vector)) {

					temp_prev->nextRight = temp->nextRight;
					for_free = temp;	//prvok, ktory sa bude uvolnovat
					temp = temp->nextRight; //posuvanie v druhom cykle

					NODE_change_every_parent(root, temp_holding, for_free);	//zmenenie v rodicoch, ktory ukazuju na potomka, ktory sa ide uvolnit
					(*counter) = (*counter) + 1;	//pripocitanie uvolneneho k poctu
					free(for_free);	//uvolnenie
				}
				else {
					temp_prev = temp;
					temp = temp->nextRight; //posuvanie v druhom cykle
				}
			}
			temp_holding = temp_holding->nextRight;	//posuvanie v prvom cykle
		}
	}
}

//rekurzivna funkcia na prepojenie NODOV v rovnakej urovni. resp aby ukazoval prvok na prvok na pravo od neho v jeho urovni
void BDD_connect_levels(NODE* node) {
	if (node == NULL) { return; }
	if (node->left) {
		node->left->nextRight = node->right;
	}
	if (node->right) {
		if (node->nextRight) {
			node->right->nextRight = node->nextRight->left;
		}
		else {
			node->right->nextRight = NULL;
		}
	}
	//vnaranie
	BDD_connect_levels(node->left);
	BDD_connect_levels(node->right);
}

//funkcia kde sa vykona cela redukcia BDD
int BDD_reduce(BDD* bdd) {
	//kontrola vstupu
	if (bdd == NULL || bdd->root == NULL) {
		printf("Chyba BDD_reduce\n");
		return -1;
	}

	BDD_connect_levels(bdd->root);	//pospajanie urovny v BDD

	int count = 0;	//pocet NODOV, ktore sa odstranili
	int iter = 1;
	NODE* temp = NULL;

	//redukuje v BDD potomkov s rovnakym vektorom
	BDD_reduce_siblings(bdd->root, &count);

	//redukuje v BDD NODE, ktory ma potomkov jeden a ten isty NODE
	BDD_delete_unwanted(&bdd->root, &count, NULL, bdd->root);

	//cyklus na zlucovanie v urovniach, zacina sa od spodku postupujuc k vrcholu
	while (iter != bdd->root->nodeVector->size) {
		temp = search_first_from_left(bdd->root, iter);	//najdem zaciatocny NODE pre uroven
		//zakomentovana vizualna kontrola ci ostalo zachovanie prepojenie urovni
		/*printf("%d \n", iter);
		
		while (temp != NULL) {
			printf("%s ", temp->nodeVector->vector);
			temp = temp->nextRight;
		}
		printf("\n\n");
		*/
		BDD_reduce_level(temp, &count, bdd->root);	//funkcia na uvolnenie
		iter = iter * 2;	//zvacsuje sa * 2 lebo aj vektory sa zvacsuju dvojnasobne
	}

	//printf("count %d\n",  count);
	bdd->numberOfNodes = bdd->numberOfNodes - count;

	return count;
}

//========================================


//===============BDD_use==================

//funkcia na vratenie vyslednej hodnoty pre zadany vektor premennych
char BDD_use(BDD* bdd, char* vstupy) {

	//kontrola ci BDD je vytvoreny a ci je vstup spravnej dlzky ako pocet premennych 
	if (bdd == NULL) {
		printf("Prazdny bdd!");
		return -1;
	}
	else if (bdd->numberOfVariables != strlen(vstupy)) {
		printf("Zle zadany vstup!");
		return -1;
	}

	NODE* temp = bdd->root;
	int index = 0;	//ukazovatel na index vektora premennych

	//cyklus na najdenie vyslednej hodnoty pre zadany vektor premennych
	//podla hodnoty na danom indexe vo vektore premennych sa vnori
	while (temp->nodeVector->size != 1) {
		if (vstupy[index] == '0') {
			temp = temp->left;
			index++;
		}
		else if (vstupy[index] == '1') {
			temp = temp->right;
			index++;
		}
		else {
			printf("Zle zadany vstup");
			return -1;
		}
	}

	return temp->nodeVector->vector[0];	//vratenie vyslednej hodnoty
}

//========================================

//==============BDD_print=================

//rekurzivna funkcia na vypisanie stromu
void BDD_print(NODE* node) {
	if (node != NULL) {
		printf("%s %d [%p] vector {%p} \n", node->nodeVector->vector, node->nodeVector->size, node, node->nodeVector);
		BDD_print(node->left);
		BDD_print(node->right);
	}
}

//========================================

//==============BDD_free==================

//rekurzivna funkcia na uvolnenie stromu NODOV
void NODES_free(NODE* node) {
	if(node != NULL){
		if (node->right != NULL) {
			//vymaz pravy podstrom
			NODES_free(node->right);
		}
		if (node->left != NULL) {
			//vymaz lavy podstrom
			NODES_free(node->left);
		}
		//uvolni aktualny vrchol
		free(node->nodeVector);
		free(node);
	}
}

//funkcia na uvolnenie BDD
void BDD_free(BDD* bdd) {
	if (bdd != NULL) {
		BDD_free_nodes(bdd->root, bdd->root->nodeVector->size);
		free(bdd);
	}
}

//rekurzivna funkcia na uvolnenie urovne
void BDD_free_level(NODE* root, NODE* node) {
	if (node != NULL) {
		NODE_change_every_parent(root, NULL, node);
		BDD_free_level(root, node->nextRight);
		free(node->nodeVector);
		free(node);
	}
}

//funkcia na uvolnenie stromu od spodu po urovniach
void BDD_free_nodes(NODE* root, int root_size) {
	NODE* temp;
	int iter = 1;
	while (iter != root_size) {
		temp = search_first_from_left(root, iter);	//najdem zaciatocny NODE pre uroven
		if (temp != NULL)
			BDD_free_level(root,temp);	//funkcia na uvolnenie
		iter = iter * 2;	//zvacsuje sa * 2 lebo aj vektory sa zvacsuju dvojnasobne
	}
	free(root);
}

//========================================
#endif