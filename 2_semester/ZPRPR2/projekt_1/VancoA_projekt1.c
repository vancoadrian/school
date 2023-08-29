#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define SUBOR "osemsmerovka.txt"
#define N 2
#define POC_PISMEN 26

void vykresli_osemsmerovku(int m, int n, char** pole);
char** nacitaj_osemsmerovku(int rows, int colums, FILE* fr, int* index[], int* vyskyt, int* velkost_index);
int hladaj_smer(char** pole, char slovo[], int m, int n, int* index[], int vyskyt[]);
int hladaj_slovo(int pozY, int pozX, int smerY, int smerX, char slovo[], int dlzka_slova, char** pole);
void uvolni(char** pole, int m, int* index[]);
void vypis_tajnicka(int m, int n, char** pole);

void vykresli_osemsmerovku(int m, int n, char** pole) {
	int pozX, pozY;
	for (pozY = 0; pozY < m; pozY++)
	{
		for (pozX = 0; pozX < n; pozX++)
			printf("%c", *(*(pole + pozY) + pozX));
		printf("\n");
	}
}

void vypis_tajnicka(int m, int n, char** pole) {
	int pozX, pozY, pocet = 0;
	for (pozY = 0; pozY < m; pozY++)
	{
		for (pozX = 0; pozX < n; pozX++)
			if (isupper(*(*(pole + pozY) + pozX))) {
				printf("%c", *(*(pole + pozY) + pozX));
				pocet++;
			}
	}
	if (pocet != 0) printf("\n"); else printf("Tajnicka je prazdna.\n");
}

char** nacitaj_osemsmerovku(int rows, int colums, FILE* fr, int* index[], int* vyskyt, int* velkost_index) {
	char** new_pole, * riadok, * r;
	riadok = (char*)malloc((colums + 2) * sizeof(char));
	//pozX-stlpce   pozY-riadky     pozP-pozicia pismena v abecede
	int pozX, pozY, pozP;

	for (pozY = 0; pozY < POC_PISMEN; pozY++)
		index[pozY] = NULL;

	new_pole = (char**)malloc(rows * sizeof(char*));
	for (pozY = 0; pozY < rows; pozY++)
		new_pole[pozY] = (char*)malloc(colums * sizeof(char));

	pozY = 0;
	while (pozY < rows && fgets(riadok, colums + 2, fr)) {
		pozX = 0;
		r = riadok;
		while (pozX < colums && r != NULL && sscanf(r, "%c", *(new_pole + pozY) + pozX) == 1) {
			pozP = *r - 'A';
			if (index[pozP] == NULL) {
				*(velkost_index + pozP) = N * 2;
				index[pozP] = (int*)malloc(N * 2 * sizeof(int));
			}
			else if (*(velkost_index + pozP) <= 2 * (*(vyskyt + pozP))) {
				index[pozP] = (int*)realloc(index[pozP], (*(velkost_index + pozP) + N * 2) * sizeof(int));
				*(velkost_index + pozP) += N * 2;
			}
			*(*(index + pozP) + 2 * (*(vyskyt + pozP))) = pozY;
			*(*(index + pozP) + 2 * (*(vyskyt + pozP)) + 1) = pozX;
			(*(vyskyt + pozP))++;

			if (r != NULL) r++;
			pozX++;
		}
		pozY++;
	}
	free(riadok);

	for (pozY = 0; pozY < POC_PISMEN; pozY++) {
		if (index[pozY] != NULL) {
			memset(index[pozY] + *(vyskyt + pozY) * 2, -1, (*(velkost_index + pozY) - 2 * *(vyskyt + pozY)) * sizeof(int));
		}
	}
	return new_pole;
}

int hladaj_smer(char** pole, char slovo[], int m, int n, int* index[], int vyskyt[]) {
	int pozP = slovo[0] - 'A', pozX, pozY, pismeno = 0, dlzka_slova = 0;
	for (; *(slovo + dlzka_slova) != '\0'; dlzka_slova++);
	int najdene = 0;
		for (int i = 0; i < vyskyt[pozP]; i++) {
			pozY = *(*(index + pozP) + 2 * i);
			pozX = *(*(index + pozP) + 2 * i + 1);
			//sever
			if (pozY - 1 - (dlzka_slova - 2) >= 0 && *(slovo + pismeno + 1) == toupper(*(*(pole + pozY - 1) + pozX))) {
				najdene += hladaj_slovo(pozY, pozX, -1, 0, slovo, dlzka_slova, pole);
			}
			//severo_vychod
			if (pozY - 1 - (dlzka_slova - 2) >= 0 && pozX + 1 + (dlzka_slova - 2) < n && *(slovo + pismeno + 1) == toupper(*(*(pole + pozY - 1) + pozX + 1))) {
				najdene += hladaj_slovo(pozY, pozX, -1, 1, slovo, dlzka_slova, pole);
			}
			//vychod
			if (pozX + 1 + (dlzka_slova - 2) < n && *(slovo + pismeno + 1) == toupper(*(*(pole + pozY) + pozX + 1))) {
				najdene += hladaj_slovo(pozY, pozX, 0, 1, slovo, dlzka_slova, pole);
			}
			//juho_vychod
			if (pozY + 1 + (dlzka_slova - 2) < m && pozX + 1 + (dlzka_slova - 2) < n && *(slovo + pismeno + 1) == toupper(*(*(pole + pozY + 1) + pozX + 1))) {
				najdene += hladaj_slovo(pozY, pozX, 1, 1, slovo, dlzka_slova, pole);
			}
			//juh
			if (pozY + 1 + (dlzka_slova - 2) < m && *(slovo + pismeno + 1) == toupper(*(*(pole + pozY + 1) + pozX))) {
				najdene += hladaj_slovo(pozY, pozX, 1, 0, slovo, dlzka_slova, pole);
			}
			//juho_zapad
			if (pozY + 1 + (dlzka_slova - 2) < m && pozX - 1 - (dlzka_slova - 2) >= 0 && *(slovo + pismeno + 1) == toupper(*(*(pole + pozY + 1) + pozX - 1))) {
				najdene += hladaj_slovo(pozY, pozX, 1, -1, slovo, dlzka_slova, pole);
			}
			//zapad
			if (pozX - 1 - (dlzka_slova - 2) >= 0 && *(slovo + pismeno + 1) == toupper(*(*(pole + pozY) + pozX - 1))) {
				najdene += hladaj_slovo(pozY, pozX, 0, -1, slovo, dlzka_slova, pole);
			}
			//severo_zapad
			if (pozY - 1 - (dlzka_slova - 2) >= 0 && pozX - 1 - (dlzka_slova - 2) >= 0 && *(slovo + pismeno + 1) == toupper(*(*(pole + pozY - 1) + pozX - 1))) {
				najdene += hladaj_slovo(pozY, pozX, -1, -1, slovo, dlzka_slova, pole);
			}
		}
	if (najdene == 0) return 0; else return 1;
}

int hladaj_slovo(int pozY, int pozX, int smerY, int smerX, char slovo[], int dlzka_slova, char** pole) {
	int* na_vymazanie, pozP = 0, posunY = 0, posunX = 0;

	na_vymazanie = (int*)malloc(dlzka_slova * 2 * sizeof(int));
	if(na_vymazanie){
		while (*(slovo + pozP) != '\0' && *(slovo + pozP) == toupper(*(*(pole + pozY + posunY) + pozX + posunX))) {
			na_vymazanie[pozP * 2] = pozY + posunY;
			na_vymazanie[pozP * 2 + 1] = pozX + posunX;
			pozP++;
			posunY += smerY;
			posunX += smerX;
		}
		if (*(slovo + pozP) == '\0') {
			for (int j = 0; j < dlzka_slova; j++) {
				*(*(pole + *(na_vymazanie + j * 2)) + *(na_vymazanie + j * 2 + 1)) = tolower(*(*(pole + *(na_vymazanie + j * 2)) + *(na_vymazanie + j * 2 + 1)));
			}
			free(na_vymazanie);
			return 1;
		}
		free(na_vymazanie);
	}
	return 0;
}


void uvolni(char** pole, int m, int* index[]) {
	int i, j;
	for (i = 0; i < m; i++)
		free(pole[i]);
	free(pole);

	for (j = 0; j < POC_PISMEN; j++) {
			free(index[j]);
	}
}

int main()
{
	int m, n, * index[POC_PISMEN], velkost_index[POC_PISMEN], vyskyt[POC_PISMEN];
	char** pole;
	FILE* fr;
	if ((fr = fopen(SUBOR, "r")) == NULL) {
		printf("Subor sa nepodarilo otvorit.\n");
		return 0;
	}
	else {
		for (int i = 0; i < POC_PISMEN; i++) {
			vyskyt[i] = 0;
			velkost_index[i] = 0;
		}

		if ((fscanf(fr, "%d %d", &m, &n)) == 0) { return 0; }
		char slovo[101];
		while (fgetc(fr) != '\n');
		pole = nacitaj_osemsmerovku(m, n, fr, index, vyskyt, velkost_index);
		while ((fscanf(fr, " %100s", slovo)) == 1) {
            if(slovo[0]=='\0' || slovo[1]=='\0'){continue;}
			if (hladaj_smer(pole, slovo, m, n, index, vyskyt)) vykresli_osemsmerovku(m, n, pole); else printf("Slovo %s sa nepodarilo najst\n", slovo);
			printf("\n");
		}
		vypis_tajnicka(m, n, pole);
		uvolni(pole, m, index);
		return 0;
	}
}
