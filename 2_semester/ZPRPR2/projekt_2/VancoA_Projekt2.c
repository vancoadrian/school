#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAXSTR 101	//definovanie maximalnej velkosti pre polia 100 znakov + ukoncovaci znak

	//struktury
typedef struct {
	char meno[MAXSTR];
	char priezvisko[MAXSTR];
} MENO;

typedef struct herec{
	MENO herec;
	int narodeny;
	struct herec* dalsi;
}HEREC;

typedef struct film{
	char nazov[MAXSTR];
	int rok_vyroby;
	MENO reziser;
	struct herec* herci;
	struct film* dalsi;
}FILM;

	//deklaracie funkcii
FILM* pridajNaKoniec_zoznam(FILM** zac, FILM* vloz);
HEREC* pridajNaKoniec_herci(HEREC* zac, HEREC* vloz);
void nacitaj(FILM** zoznam, FILE* filmy);
void uvolni(FILM** zoznam);
void vypis_zoznam(FILM* zoznam);
void pridaj_film(FILM** zoznam);
void vymaz_film(FILM** zoznam);
void vypis_filmy(FILM* zoznam);
void herci(FILM* zoznam);
void odober_rovnakych_hercov(HEREC* zac_herci);
HEREC* pridajDoUsp_herci(HEREC* zac, HEREC* vloz);
void rok(FILM* zoznam);


	//funkcia, ktora vlozi na koniec zoznamu film a vrati zaciatok zoznamu
FILM* pridajNaKoniec_zoznam(FILM** zac, FILM* vloz) {
	FILM* akt = *zac;									//akt pre prejdenie na koniec zoznamu a aby sme nestraliti zaciatok musime mat tento pointer

	if (*zac == NULL) return vloz;						//ak este nebolo predtym nic vlozene do zoznamu vratim zaciatok
	while (akt->dalsi != NULL)							//prejdem az na koniec zoznamu
		akt = akt->dalsi;
	akt->dalsi = vloz;									//vlozim na koniec
	return *zac;										//vratim zaciatok
}
	//funkcia, ktora vlozi na koniec zoznamu herci a vrati zaciatok zoznamu
HEREC* pridajNaKoniec_herci(HEREC* zac, HEREC* vloz) {
	HEREC* akt = zac;									//akt pre prejdenie na koniec zoznamu a aby sme nestraliti zaciatok musime mat tento pointer

	if (zac == NULL) return vloz;						//ak este nebolo predtym nic vlozene do zoznamu vratim zaciatok
	while (akt->dalsi != NULL)							//prejdem az na koniec zoznamu
		akt = akt->dalsi;
	akt->dalsi = vloz;									//vlozim na koniec
	return zac;											//vratim zaciatok
}

	//funkcia na nacitanie zoznamu zo suboru (prikaz "nacitaj")
void nacitaj(FILM **zoznam, FILE *filmy) {
	FILM* vloz = NULL;															//ukazovatel na vkladanie filmu
	HEREC* zac_herci = NULL, * vloz_herca = NULL;								//ukazovatel zac_herci pre ziskanie zaciatku zoznamu hercov a ukazovatel vloz_herca pre vlozenie herca

	char nazov_filmu_src[MAXSTR], nazov_filmu_dest[MAXSTR], znak;				//_src ako pomocna pri nacitavany nazvu	a znak na zistenie aky znak sa nachadza za prvym slovom
		//cyklus na nacitavanie filmov
	while (fscanf(filmy, "%100s", nazov_filmu_dest) == 1 && !feof(filmy)) {
		vloz = (FILM*)malloc(sizeof(FILM));										//alokovanie pre film
			//cyklus na ziskanie navzu filmu aj neboj jednoslovny
		while ((znak = getc(filmy)) != '\n' && znak != EOF) {
			fscanf(filmy, "%100s", nazov_filmu_src);
			if (strlen(nazov_filmu_dest) + 1 + strlen(nazov_filmu_src) < MAXSTR) {
				strcat(nazov_filmu_dest, " ");
				strcat(nazov_filmu_dest, nazov_filmu_src);
			}
		}
			//nacitanie informacii o filme a nastavenie ukazovatela na dalsi film na null
		strcpy(vloz->nazov, nazov_filmu_dest);
		fscanf(filmy, "%d", &vloz->rok_vyroby);
		fscanf(filmy, "%100s", vloz->reziser.meno);
		fscanf(filmy, "%100s", vloz->reziser.priezvisko);
		vloz->dalsi = NULL;
			//cyklus na prejdenie na koniec riadku
		while ((znak = getc(filmy)) != '\n' && znak != EOF);
			//cyklus na nacitavanie hercov
		while (fgetc(filmy) == '*') {
			vloz_herca = (HEREC*)malloc(sizeof(HEREC));							//alokovanie herca a nacitanie udajov
			fscanf(filmy, "%100s", vloz_herca->herec.meno);
			fscanf(filmy, "%100s", vloz_herca->herec.priezvisko);
			fscanf(filmy, "%d", &vloz_herca->narodeny);
			vloz_herca->dalsi = NULL;
			zac_herci = pridajNaKoniec_herci(zac_herci, vloz_herca);		//vlozenie herca a ziskanie zaciatku na spajany zoznam hercov
			while ((znak = getc(filmy)) != '\n' && znak != EOF);
		}

		vloz->herci = zac_herci;												//vlozenie zaciatku hercov vo filme
		fseek(filmy, -1, SEEK_CUR);												//vratenie o znak v subore ak to uz nebola hviezdicka
		*zoznam = pridajNaKoniec_zoznam(zoznam, vloz);							//vlozenie filmu a ziskanie zaciatku na spajany zoznam filmov
		zac_herci = NULL;														//zac_herci nastavujeme na null aby sme zacali novy zoznam hercov
	}
}

	//funkcia na uvolnenie dynamicky akolovanych struktur
void uvolni(FILM** zoznam) {
	FILM* pom_film = NULL;									//pomocne aby som najprv obisiel, to co chcem uvolnit
	HEREC* pom_herec = NULL;
	while (*zoznam != NULL) {
		while ((*zoznam)->herci != NULL) {
			pom_herec = (*zoznam)->herci;
			(*zoznam)->herci = (*zoznam)->herci->dalsi;
			free(pom_herec);
		}
		pom_film = *zoznam;
		*zoznam = (*zoznam)->dalsi;
		free(pom_film);
	}
}

	//funkcia na vypis zoznamu filmov s hercami (prikaz "vypis")
void vypis_zoznam(FILM* zoznam){
	HEREC* pom_herec;			//aby sa mi neprepisali pointre v zozname pre hercov
	if (zoznam == NULL) {
		printf("Najprv nacitaj\n");
		return;
	}
	while (zoznam != NULL) {
		printf("%s (%d) %s %s\n", zoznam->nazov, zoznam->rok_vyroby, zoznam->reziser.meno, zoznam->reziser.priezvisko);
		pom_herec = zoznam->herci;
		if (pom_herec != NULL) {																						//podmienka ci pre film je nejaky zoznam hercov
			printf("Hraju: ");
			while (pom_herec != NULL) {																					//vypisovanie hercov
				printf("%s %s (%d)", pom_herec->herec.meno, pom_herec->herec.priezvisko, pom_herec->narodeny);
				if (pom_herec->dalsi != NULL) {																			//ak aktualny herec nieje este posledny tak , a medzera
					printf(", ");
				}
				pom_herec = pom_herec->dalsi;																			//posuvanie v cykle po hercovi
			}
			printf("\n");
		}
		zoznam = zoznam->dalsi;																							//posuvanie v cykle po filme
	}
}
	//funkcia na pridanie filmu do zoznamu na koniec (prikaz "pridaj")
void pridaj_film(FILM** zoznam){
	FILM* vloz=NULL;
	HEREC* zac_herci = NULL, * vloz_herca = NULL;
	char nazov_filmu_src[MAXSTR], nazov_filmu_dest[MAXSTR], meno_herca[MAXSTR],znak;
		//ziskanie nazvu filmu
	scanf("%100s", nazov_filmu_dest);
	vloz = (FILM*)malloc(sizeof(FILM));
			//cyklus ak nebol nazov jednoslovny
		while ((znak = getchar()) != '\n') {
			scanf("%100s", nazov_filmu_src);
			if (strlen(nazov_filmu_dest) + 1 + strlen(nazov_filmu_src) < MAXSTR) {
				strcat(nazov_filmu_dest, " ");
				strcat(nazov_filmu_dest, nazov_filmu_src);
			}
		}
		strcpy(vloz->nazov, nazov_filmu_dest);											//nacitanie udajov pre film
		scanf("%d", &vloz->rok_vyroby);
		scanf("%100s", vloz->reziser.meno);
		scanf("%100s", vloz->reziser.priezvisko);
		vloz->dalsi = NULL;
			//cyklus pre nacitavanie hercov
		while ((scanf("%100s", meno_herca)) == 1 && strcmp(meno_herca, "*")) {
			vloz_herca = (HEREC*)malloc(sizeof(HEREC));
			strcpy(vloz_herca->herec.meno, meno_herca);									//nacitanie udajov pre herca
			scanf("%100s", vloz_herca->herec.priezvisko);
			scanf("%d", &vloz_herca->narodeny);
			vloz_herca->dalsi = NULL;
			zac_herci = pridajNaKoniec_herci(zac_herci, vloz_herca);					//vlozenie herca do zoznamu hercov a ziskanie zaciatku zoznamu
		}
		vloz->herci = zac_herci;														//vlozenie zaciatku zoznamu hercov do filmu
		*zoznam = pridajNaKoniec_zoznam(zoznam, vloz);									//vlozenie filmu do zoznamu filmov a ziskanie zaciatku
}

		//funkcia na vymazanie filmu aj s hercami zo zoznamu filmov (prikaz "vymaz")
void vymaz_film(FILM** zoznam) {
	char nazov_filmu_dest[MAXSTR], nazov_filmu_src[MAXSTR],znak;
	FILM* vymaz = *zoznam, * pom = NULL;
	HEREC* pom_herec = NULL;
	if ((*zoznam) == NULL) {														//ak je zoznam prazdny tak nemame co vymazavat
		printf("Prazdny zoznam, najprv nacitaj\n");
		return;
	}
		//nacitanie nazvu
	scanf("%100s", nazov_filmu_dest);
		//donacitavanie nazvu ak nazov nebol jednoslovny
	while ((znak = getchar()) != '\n' && znak != EOF) {
		scanf("%100s", nazov_filmu_src);
		if (strlen(nazov_filmu_dest) + 1 + strlen(nazov_filmu_src) < MAXSTR) {		//kontrola ci sa dalsie slovo mesti do nazvu
			strcat(nazov_filmu_dest, " ");
			strcat(nazov_filmu_dest, nazov_filmu_src);
		}
	}

	if (!strcmp((*zoznam)->nazov, nazov_filmu_dest)) {								//podmienka ak vymazavame prvy
		if ((*zoznam)->dalsi != NULL) {
			pom = (*zoznam)->dalsi;
		}
		while ((*zoznam)->herci != NULL) {
			pom_herec = (*zoznam)->herci;
			(*zoznam)->herci = (*zoznam)->herci->dalsi;
			free(pom_herec);
		}
		free(*zoznam);
		*zoznam = pom;
		return;
	}else{																			//ak vymazavanie iny ako prvy
			//prechadzame zoznam pokial nenajdeme zhodny nazov filmu
		while (strcmp(vymaz->nazov, nazov_filmu_dest)) {
			if (vymaz->dalsi != NULL) {												//kontrola aby sme sa posuvali a dobre ukoncili cyklus
				pom = vymaz;
				vymaz = vymaz->dalsi;
			}
			else {
				break;
			}
		}
		if (!strcmp(vymaz->nazov, nazov_filmu_dest)) {								//kontrola ci sa vobec nasiel filmy s rovnakym nazvom
			pom->dalsi = vymaz->dalsi;												//ak sa nasiel tak ho obideme
			while (vymaz->herci != NULL) {											//cyklus na uvolnovanie hercov
				pom_herec = vymaz->herci;
				vymaz->herci = vymaz->herci->dalsi;
			free(pom_herec);
			}
			free(vymaz);
		}
	}
}

	//funkcia na vypis filmov, v ktorych hral zadany herec (prikaz "filmy")
void vypis_filmy(FILM* zoznam) {
	HEREC* pom_herec;
	int najdene = 0;
	char meno_herca[MAXSTR], priezvisko_herca[MAXSTR];
	if (zoznam == NULL) {										//ak je zoznam prazdny nemame kde hladat
		printf("Prazdny zoznam\n");
		return;
	}
	scanf("%100s",meno_herca);									//nacitanie inicialov herca
	scanf("%100s",priezvisko_herca);
		//cyklus na prechadzanie zoznamu filmov
	while (zoznam != NULL) {
			pom_herec = zoznam->herci;
			while (pom_herec != NULL) {
				if (!strcmp(meno_herca, pom_herec->herec.meno) && !strcmp(priezvisko_herca, pom_herec->herec.priezvisko)){		//ak sa zhoduje meno a priezvisko herca vo filme tak vypis filmu
					najdene++;
					printf("%s (%d)\n", zoznam->nazov, pom_herec->narodeny);
					break;
				}
				pom_herec = pom_herec->dalsi;
			}
		zoznam = zoznam->dalsi;
	}
	if (najdene == 0) {
		printf("Herec nehral v ziadnom z filmov v zozname\n");
	}
}

	//funkcia na hladanie hercov, ktory hrali v oboch zadanych filmoch
void herci(FILM* zoznam) {
	HEREC* pom_herec, * pom_herec2;
	FILM* pom_film = zoznam, * pom_film2 = zoznam, * ponechaj_film2 = NULL;
	int najdene_herec = 0, najdene_filmy = 0;						//premenna najdene_filmy pre pocet najdenych filmov aby som vypisal, ktory film nebol najdeny a najdene herec, aby som vedel ci sa nasiel aspon jeden herec
	char nazov_filmu_dest[MAXSTR], nazov_filmu_src[MAXSTR], znak;
	char nazov_filmu_dest2[MAXSTR];
	if(zoznam == NULL){
		printf("Prazdny zoznam\n");
		return;
	}
		//ziskanie prveho nazvu filmu
	scanf("%100s", nazov_filmu_dest);
	while ((znak = getchar()) != '\n' && znak != EOF) {
		if (strlen(nazov_filmu_dest) + 1 < MAXSTR) {
			scanf("%100s", nazov_filmu_src);
			strcat(nazov_filmu_dest, " ");
			strcat(nazov_filmu_dest, nazov_filmu_src);
		}
	}
		//ziskanie druheho nazvu filmu
	scanf("%100s", nazov_filmu_dest2);
	while ((znak = getchar()) != '\n' && znak != EOF) {
		if (strlen(nazov_filmu_dest2) + 1 < MAXSTR) {
			scanf("%100s", nazov_filmu_src);
			strcat(nazov_filmu_dest2, " ");
			strcat(nazov_filmu_dest2, nazov_filmu_src);
		}
	}
		//cyklus ktory prechadza zoznam od zaciatku pomocou pom_film az kym nenajde zhodu s prvym nazvom
	while (pom_film != NULL) {
		if (!strcmp(pom_film->nazov, nazov_filmu_dest)) {	//ak sa nasiel prvy film idem do cyklu pre jeho hercov
			najdene_filmy++;
			pom_herec = pom_film->herci;					//priradenie zaciatku hercov pre prvy film
			while (pom_herec != NULL) {						//cyklom prechadzam hercov prveho filmu
				if (ponechaj_film2 != NULL) { pom_film2 = ponechaj_film2; }			//znova zapisanie najdeneho druheho filmu, aby som hercov z druheho filmu porovnal s kazdym hercom v prvom filme
				while (pom_film2 != NULL) {											//cyklus na hladanie druheho filmu
					if (!strcmp(pom_film2->nazov, nazov_filmu_dest2)) {				//ak sa nasiel druhy film
						najdene_filmy++;
						ponechaj_film2 = pom_film2;									//ziskanie zaciatku druheho najdene filmu
						pom_herec2 = pom_film2->herci;								//ziskanie zaciatku hercov z druheho filmu
						while (pom_herec2 != NULL) {								//cyklus na prechadzanie hercov z druheho filmu a nasledne porovnavanie hercov z prveho a druheo filmu
							if (!strcmp(pom_herec->herec.meno, pom_herec2->herec.meno) && !strcmp(pom_herec->herec.priezvisko, pom_herec2->herec.priezvisko) && pom_herec2->narodeny == pom_herec2->narodeny) {
								printf("%s %s (%d)\n", pom_herec2->herec.meno, pom_herec2->herec.priezvisko, pom_herec2->narodeny);		//ak sa nasiel tak ho vypiseme a zvacsime premennu pre detekciu hercov
								najdene_herec++;
							}
							pom_herec2 = pom_herec2->dalsi;
						}
					}
					pom_film2 = pom_film2->dalsi;
				}
				pom_herec = pom_herec->dalsi;
			}
			break;	//nepotrebujeme ist uz dalej tak break
		}
		pom_film = pom_film->dalsi;
	}
		//vypis podla toho, ktory film sa nenasiel, ak sa nenasiel prvy tak prebehneme cyklom druhy a ak sa ten nasiel tak vypiseme ze sa nenasiel len prvy ak nie tak oba sa nenasli
	if (najdene_filmy == 0) {
		while (pom_film2 != NULL) {
			if (!strcmp(pom_film2->nazov, nazov_filmu_dest2)) {
				printf("Prvy film nebol najdeny\n"); break;
			}
			pom_film2 = pom_film2->dalsi;
		}
		if (pom_film2 == NULL) { printf("Oba filmy sa nenasli\n"); }
	}
	else if (najdene_filmy == 1) {
		printf("Druhy film nebol najdeny\n");
	}
		//vypis ak sa nasli filmy ale nenasiel sa herec, ktory by hral v oboch filmoch
	if (najdene_filmy == 2 && najdene_herec == 0) {
		printf("Ziadny z hercov nehral v oboch filmoch\n");
	}
}

//funkcia na uvolnenie dynamicky akolovanych struktur
void uvolni_hercov(HEREC** zoznam) {
	HEREC* pom_film = NULL;									//pomocna aby som najprv obisiel, to co chcem uvolnit
	while (*zoznam != NULL) {
		pom_film = *zoznam;
		*zoznam = (*zoznam)->dalsi;
		free(pom_film);
	}
}

	//funkcia pre prikaz rok na odobratie roznakych hercov zo zoznamu pre rok
void odober_rovnakych_hercov(HEREC* zac_herci) {
	HEREC* pom_herec2, * pom_herec, * pom,* vymaz = zac_herci;
	int move = 0, move2 = 0, vymazat = 0;							//moves pre to aby ked sa ukazuje na rovnaky tak aby to nevymazalo a vymaz na to aby sme uvolnili po priradeni
	pom_herec2 = zac_herci;
	pom_herec = zac_herci;
	pom = zac_herci;
	while (pom_herec != NULL) {
		pom_herec2 = zac_herci;
		while (pom_herec2 != NULL) {
			if(move != move2 && !strcmp(pom_herec->herec.meno, pom_herec2->herec.meno) && !strcmp(pom_herec->herec.priezvisko, pom_herec2->herec.priezvisko) && pom_herec->narodeny == pom_herec2->narodeny){
				//printf("%s %s (%d)\n", pom_herec2->herec.meno, pom_herec2->herec.priezvisko, pom_herec2->narodeny);
				pom->dalsi = pom_herec2->dalsi;
				vymaz = pom_herec2;
				vymazat = 1;
			}
			pom = pom_herec2;
			pom_herec2 = pom_herec2->dalsi;
			if (vymazat == 1) {
				free(vymaz);
				vymazat = 0;
			}
			move2++;
		}
		move2 = 0;
		move++;
		pom_herec = pom_herec->dalsi;
	}
}

HEREC* pridajDoUsp_herci(HEREC* zac, HEREC* vloz) {
	HEREC* akt = zac;														//akt pre prechadzanie zoznamu a aby sme nestraliti zaciatok musime mat tento pointer
	if (zac == NULL) return vloz;											//ak este nebolo predtym nic vlozene do zoznamu vratim zaciatok
	if (strcmp(zac->herec.priezvisko, vloz->herec.priezvisko) == 1){		//podmienky pre porovnavanie ak su herci rovnaky tak ich vlozime, rovnakych hercov odfiltrujeme potom
		vloz->dalsi = zac;
		return vloz;
	}
	else if (strcmp(zac->herec.priezvisko, vloz->herec.priezvisko) == 0) {
		if (strcmp(zac->herec.meno, vloz->herec.meno) == 1) {
				vloz->dalsi = zac;
				return vloz;
		}
		else if (strcmp(zac->herec.meno, vloz->herec.meno) == 0) {
			if (zac->narodeny > vloz->narodeny) {
				vloz->dalsi = zac;
				return vloz;
			}
			else if (zac->narodeny == vloz->narodeny) {
				if(zac->dalsi !=NULL){
					vloz->dalsi = zac->dalsi->dalsi;
				}
				zac->dalsi = vloz;
				return zac;
			}
		}
	}
		//cyklus na prechadzanie zoznamu a vkladanie do zoznamu podla podmienok aby sme ich zoradili
	while (akt->dalsi != NULL && strcmp(akt->dalsi->herec.priezvisko, vloz->herec.priezvisko) < 1) {	//ak akt->dalsi->herec.priezvisko je uz v abecede dalej tak zastavime
		if (strcmp(akt->dalsi->herec.priezvisko, vloz->herec.priezvisko) == 0) {						//ak priezviska su rovnake tak sa pozrieme ako su na tom mena hercov
			if (strcmp(akt->dalsi->herec.meno, vloz->herec.meno) < 1) {									//ak akt->dalsi meno je mensie v abecede tak pozrieme ci sa nerovna
				if (strcmp(akt->dalsi->herec.meno, vloz->herec.meno) == 0) {							//ak sa akt->dalsi meno rovna pozrieme sa na roky narodenia hercov
					if (akt->dalsi->narodeny > vloz->narodeny) { break; }								//ak akt->dalsi je narodeny neskor tak zastavime
				}
			}
			else { break; }																				//else pre meno ak akt->dalsi meno je uz dalej v abecede tak zastavime
		}
		akt = akt->dalsi;					//posuvanie sa v zozname
	}
	vloz->dalsi = akt->dalsi;				//vkladanie do zoznamu za akt
	akt->dalsi = vloz;
	return zac;
}

	//funkcia na vypis hercov, ktory hrali v niektorom z filmov, ktore vysli v zadanom roku (prikaz "rok")
void rok(FILM* zoznam) {
	HEREC* pom_herec, * zac_herci = NULL, * vloz_herec;
	int rok, najdene = 0;
	if (zoznam == NULL) {														//kontrola ci je zoznam prazdny
		printf("Prazdny zoznam, najprv nacitaj\n");
		return;
	}
	scanf("%d", &rok);															//nacitanie roku

	while (zoznam != NULL) {													//prechadzanie zoznamu filmov
		if (zoznam->rok_vyroby == rok) {										//ak sa rok filmu zhoduje so zadanym rokom prejdem do zoznamu hercov
			najdene++;
			pom_herec = zoznam->herci;
			while (pom_herec != NULL) {											//cyklus na prechadzanie hercov a vytvarenie noveho zoznamu hercov
				vloz_herec = (HEREC*)malloc(sizeof(HEREC));
				vloz_herec->narodeny = pom_herec->narodeny;
				vloz_herec->herec = pom_herec->herec;
				vloz_herec->dalsi = NULL;
				zac_herci = pridajDoUsp_herci(zac_herci, vloz_herec);			//funkcia, ktora vracia zaciatok na zoznam hercov pre funkciu rok, posielam do nej zaciatok a herca na vlozenie
				pom_herec = pom_herec->dalsi;									//posun v zozname hercov
			}
		}
		zoznam = zoznam->dalsi;													//posun v zozname filmov
	}
	if (najdene == 0) {															//ak sa nezasiel film so zadanym rokom
		printf("Z daneho roku nieje ziadny film v zozname\n");
		return;
	}
	if(zac_herci != NULL){
		if(zac_herci->dalsi != NULL){											//ak novy zoznam pre prikaz rok obsahuje aspon dvoch hercov tak ich skontrolujem ci niesu rovnaky
			odober_rovnakych_hercov(zac_herci);
		}
			//cyklus na vypis zoznamu hercov, ktory hrali vo filmoch daneho roku
		while (zac_herci != NULL) {
			printf("%s %s (%d)", zac_herci->herec.meno, zac_herci->herec.priezvisko, zac_herci->narodeny);
			if (zac_herci->dalsi != NULL) {
				printf(", ");
			}
			zac_herci = zac_herci->dalsi;										//posun v cykle
		}
		printf("\n");
		uvolni_hercov(&zac_herci);
	}
}

int main()
{
	FILE* filmy;									//ukazovatel na subor s filmami
	FILM* zoznam = NULL;							//ukazovatel na zaciatok zoznamu s filmami
	char prikaz[MAXSTR];							//pole na zadany prikaz
	filmy = fopen("filmy.txt", "r+");				//otvorenie suboru
	if(filmy != NULL){								//osetrenie otvarania suboru
		while (scanf("%100s", prikaz) == 1) {		//nacitavam prikazi pokial nieje zadany prikaz koniec
			if (!strcmp(prikaz, "nacitaj")) {		//kontrola prikazov pomocou podmienok, ak sa prikaz zhoduje tak sa zavola dana funkcia
				if (zoznam == NULL){				//kontrola ci je zoznam prazdny, ak je tak sa nacita
					nacitaj(&zoznam, filmy);
				}
				else {
					uvolni(&zoznam);				// a ak nie je tak sa najprv uvolni nacitany zoznam, ukazovatel na subor sa nastavi na zaciatok a az potom sa nacitaj zoznam
					rewind(filmy);
					nacitaj(&zoznam, filmy);
				}
			}
			else if (!strcmp(prikaz, "vypis")) {
				vypis_zoznam(zoznam);
			}
			else if (!strcmp(prikaz, "pridaj")) {
				pridaj_film(&zoznam);
			}
			else if (!strcmp(prikaz, "vymaz")) {
				vymaz_film(&zoznam);
			}
			else if (!strcmp(prikaz, "filmy")) {
				vypis_filmy(zoznam);
			}
			else if (!strcmp(prikaz, "herci")) {
				herci(zoznam);
			}
			else if (!strcmp(prikaz, "rok")) {
				rok(zoznam);
			}
			else if (!strcmp(prikaz, "koniec")) {
				uvolni(&zoznam);
				if (fclose(filmy) == EOF)			//osetrenie zatvarania
				{
					printf("Chyba: zatvaranie suboru\n");
				}
				break;
			}
		}
		return 0;
	}
	else {
		printf("Chyba: otvaranie suboru\n");
		return 0;
	}
}
