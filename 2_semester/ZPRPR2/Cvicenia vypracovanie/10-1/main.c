// uloha10-1.c -- Adrián Vanèo, 7.5.2020 20:18

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct clovek {
    char meno[30];
    int rocnik;
    struct clovek *dalsi, *pred;
} CLOVEK;

int rovnakiSusedia(CLOVEK *akt) {
   int i= 0;
   if(akt==NULL) return 0;
   if(akt->dalsi == NULL && akt->pred == NULL) return akt->rocnik;
   CLOVEK* pom = akt;

  do{
     if(pom->pred == NULL && pom->dalsi!=NULL && pom->rocnik == pom->dalsi->rocnik) i++;
     if(pom->pred != NULL && pom->dalsi == NULL && pom->rocnik == pom->pred->rocnik) i++;
    if(pom->pred!=NULL && pom->dalsi!=NULL && pom->pred->rocnik == pom->dalsi->rocnik) i++;
    pom = pom->dalsi;
  }
   while(pom != NULL);


   return i;
	// sem napis svoje riesenie
}

// vlozenie zaznamu
CLOVEK *vlozDoUsp(CLOVEK *zac, char meno[], int rocnik){
   CLOVEK *vloz = (CLOVEK *) malloc(sizeof(CLOVEK));
   strcpy(vloz->meno, meno);
   vloz->rocnik = rocnik;
   vloz->dalsi = vloz->pred = NULL;

   // prazdny zoznam: jedinym prvkom zoznamu sa stane vkladany prvok
   if (zac == NULL) return vloz;

   // vkladanie na zaciatok zoznamu
   if (strcmp(meno, zac->meno) < 0) {
      vloz->dalsi = zac;
      zac->pred = vloz;
      return vloz;
   }

   // vkladanie na 2. - poslednu poziciu
   CLOVEK *akt = zac;
   while (akt->dalsi != NULL && strcmp(meno, akt->dalsi->meno) > 0) {
      akt = akt->dalsi;
   }
   vloz->dalsi = akt->dalsi;
   akt->dalsi = vloz;
   vloz->pred = akt;
   if (vloz->dalsi != NULL) vloz->dalsi->pred = vloz;
   return zac;
}

int main() {

	CLOVEK *akt = NULL;
	int r;
	char meno[100];

	while (scanf("%s %d", meno, &r) == 2) {
		akt = vlozDoUsp(akt, meno, r);

	}

	printf("%d\n", rovnakiSusedia(akt));
	return 0;
}
