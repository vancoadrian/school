#define _CRT_SECURE_NO_WARNINGS
// nacitanie najviac N prvych roznych slov do N-prvkoveho pola ukazovatelov na retazce
// v N-prvkovom poli celych cisel si pamatame pocty vyskytov
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 20		// maximalna dlzka slovnika
#define NSLOVA 100 	// maximalna dlzka slova
#define SUBOR "slova.txt"

int nacitaj(char *slovnik[], int pocet[], int *pozicie[]) {
	int i, n = 0, poz=0;
	char *p, slovo[NSLOVA];
	FILE *f;

	for(i=0; i<N; i++) {
		pocet[i] = 0;
		slovnik[i] = NULL;
		pozicie[i] = NULL;
	}

	if((f = fopen(SUBOR, "r")) == NULL) {
		printf("Nepodarilo sa otvorit subor\n");
		return 0;
	}
	while(fscanf(f, "%s", slovo) == 1) {
		//printf("%s\n", slovo);
		poz++;
		for(i=0; i<n; i++)
			if(!strcmp(slovnik[i], slovo))
				break;
		if(i < n) // slovo sa v slovniku nachadza
			{
			    pocet[i]++;
			    pozicie[i] = (int *) realloc(pozicie[i],pocet[i] * sizeof(int));
                *(pozicie[i]+pocet[i]-1)=poz;
			}
		else if(n<N){
			int len = strlen(slovo);
			p = (char *) malloc(len+1);
			pozicie[n] = (int *) malloc(1 * sizeof(int));
			*(pozicie[n]) = poz;
			strcpy(p, slovo);
			slovnik[n] = p;
			pocet[n] = 1;
			n++;
		}
	}
	fclose(f);
	for(i=n; i<N; i++) {
		slovnik[i] = NULL;
		pocet[i] = 0;
	}
 	return n;
}

void uvolni(char *slovnik[],int *pozicie[]) {
	int i;
	for (i=0; i<N; i++)
    {
        if (slovnik[i] != NULL)
        {
            free(slovnik[i]);
            free(pozicie[i]);
        }
    }
}

int main() {
	char *slovnik[N];
	int pocet[N],*pozicie[N];
	int i, j, n;
	n = nacitaj(slovnik, pocet, pozicie);
	for(i=0; i<n; i++)
    {
        printf("%s (%d):", slovnik[i], pocet[i]);
        for(j=0;j<pocet[i];j++)
            printf(" %d",pozicie[i][j]);
        printf("\n");
    }
	uvolni(slovnik, pozicie);
	return 0;
}
