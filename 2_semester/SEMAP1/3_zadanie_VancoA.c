#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

/*
Zadanie planujem riesit nasledovne: Ked som zacinal s malymi retazcami pri provnani 2 pismenoveho retazca som vracial 1 ak sa nerovnali a 0 ak sa rovnali  a potom napr.pre 3 pismenovy retazec som zistil som ked porovnam prve dva znaky,
potom posledne dva a dalej porovnam prvy s poslednym a ziskane cislo (1 alebo 0) zratam s mensim cislom ziskanym z prvych dvoch a poslednych dvoch tak dostanem minimalny pocet pismen potrebnych na upravu retazca na palindrom. A potom pre 4 pismenovy
retazec som musel porovnat aj prve pismeno s predposlednym a druhe pismeno s poslednym teda v podstate kazdy s kazdym. Takze budem potrebovat pole do, ktoreho bude vynulovane a kde budem zapisovat hodnoty.
*/

int min(int a, int b);
int treba_doplnit(char retazec[], int n);

int min(int a, int b) { return a < b ? a : b; } // vraziam mensie cislo

	//funkcia, ktora vracia najmensi mozny pocet znakov, ktory treba do retazca vlozit aby z neho bol palindrom
int treba_doplnit(char retazec[], int n) {
	int** tabulka, l, h, medzera;
	if (strlen(retazec) == 1) return 0; //ak retazec obsahuje iba 1 znak tak vraciam 0 lebo je uz palindrom
	if (strlen(retazec) == 2) return (retazec[0] == retazec[1]) ? 0 : 1;    //ak retazec obsahuje 2 znaky tak ich porovnam a ak su rovnake vraciam 0 a ak nie tak 1

        //ak retazec obsahuje viac znakov
	tabulka = (int**)calloc(n, sizeof(int*));   //alokovanie dynamickeho pola na velkost n*n, calloc vynuluje pole
	if (tabulka) {  //kontrola ci sa alokovali riadky
		for (int i = 0; i < n; i++)
			tabulka[i] = (int*)calloc(n, sizeof(int));
		if (tabulka[0]) //kontrola ci sa alokovali stlpce
		{
			for (medzera = 1; medzera < n; ++medzera){//for slusi na rozsirovanie medzeri medzi znakmi na zaciatku porovnavame postupne z lava doprava znaky,ktore su vedla seba, potom, ktore su od saba vzdialene jeden znak, potom dva znaky a tak dalej... pokial je medzera mensia ako dlzka retazca
				//printf("medzera %d\n", medzera);
				for (l = 0, h = medzera; h < n; ++l, ++h) {     //l a h su indexy, ktore ukazuju na miesta v tabulke a aj na indexy pismen v retazci, postupne sa posuvaju v retazci z prava do lava s rozostupom medzera
					tabulka[l][h] = (retazec[l] == retazec[h]) ? tabulka[l + 1][h - 1] : (min(tabulka[l][h - 1], tabulka[l + 1][h]) + 1);
					/*
                        na zaciatku porovnavame susedne znaky a postupne zvisujem medzeru takze porovnamavam potom ob medzeru(ob jeden znak,ob dva znaky...)
                        v poli sa posuvam od kolmice l==h smerom k pravemu hornemu rohu (l == 0 && h == n-1)
                        jedina taka mozno nevyhoda, ze pod kolmicou všetky suradnice >= tabulka(l+h+2,h), je nevyuzite pole
					*/
                //printf("%d %d %d\n", tabulka[l][h], l, h);
				}
            //printf("\n");
            }
			return tabulka[0][n - 1];   //vraciam najmensi pocet pismen potrebnych na vlozenie do retazca na to aby sa z neho mohol spravit palindrom
		}
		return -1;  //ak sa vyskytla chyba pri alokacii
	}
	return -1;  //ak sa vyskytla chyba pri alokacii
}

int main()
{
	int pocet;
	char retazec[1001]; //pole na retazec max na 1000znakov
	while (scanf("%1000s", retazec) == 1) {
		if (retazec[0] != '\0') {   //podmienka, ktora kontroluje ci sa v poli nachadza aspon jeden znak
            //printf("velkost %d\n", strlen(retazec));
			if ((pocet = treba_doplnit(retazec, strlen(retazec))) == -1) printf("Chyba alokovania\n");
			printf("%d\n", pocet);    //vypis poctu pismen, ktore treba doplnit do retazca aby sa z neho stal palindrom
		}
	}
	return 0;
}
