// scitanie, vypis a vymena obsahu matic
// na cviceniach dorobit nasobenie matic
// staticke dvojrozmerne pole
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define N 3
#define SUBOR "matice.txt"

void nacitaj(int m1[][N], int m2[][N], int m3[][N]);
void vypis(int m1[][N], int m2[][N], int m3[][N]);
void scitaj(int m1[][N], int m2[][N], int m3[][N]);
void nasob(int m1[][N], int m2[][N], int m3[][N]);
void vymen(int m1[][N], int m2[][N]);
int nacitaj_pom(int m[][N], FILE *f);

int main()
{
   	int c, m1[N][N], m2[N][N], m3[N][N];
   	char f_meno[50];

   	nacitaj(m1, m2, m3);
   	do {
		printf("\n*** MATICE *** \nv: vypis \ns: scitanie \n");
   		printf("n: nasobenie \n");
   		printf("m: vymena \nk: koniec\n");
		while ((c = tolower(getchar())) == '\n');

		switch (c) {
			case 'v': vypis(m1, m2, m3); break;
			case 's': scitaj(m1, m2, m3); break;
			case 'n': nasob(m1, m2, m3); break;
			case 'm': vymen(m1, m2); break;
		}
	} while (c != 'k');
	return 0;
}

void vypis_pom(int m[][N], int cislo)
{
	int i, j;

	printf("Matica c.%d:\n", cislo);
	for (i=0; i<N; i++) {
        for (j=0; j<N; j++)
	        printf("%d ", m[i][j]);
        putchar('\n');
    }
    putchar('\n');
}

void vypis(int m1[][N], int m2[][N], int m3[][N])
{
   	vypis_pom(m1, 1);
   	vypis_pom(m2, 2);
   	vypis_pom(m3, 3);
}

int nacitaj_pom(int m[][N], FILE *f)
{
   int i, j;

   for (i=0; i<N; i++)
      for (j=0; j<N; j++)
         if (fscanf(f, "%d", &m[i][j]) != 1)
            return 1;
   return 0;
}

void nacitaj(int m1[][N], int m2[][N], int m3[][N])
{
   FILE *f;

   if ((f = fopen(SUBOR, "r")) == NULL) {
      printf("Subor %s sa nepodarilo otvorit.\n", SUBOR);
      exit(1);
   }

   if(nacitaj_pom(m1, f) || nacitaj_pom(m2, f) ||
      nacitaj_pom(m3, f)) {
      printf("Nepodarilo sa nacitat matice.\n");
	exit(1);
   }

   if (fclose(f) == EOF)
      printf("Subor sa nepodarilo zatvorit.\n");
}

void scitaj(int m1[][N], int m2[][N], int m3[][N])
{
   int i, j;

   for (i=0; i<N; i++)
      for (j=0; j<N; j++)
         m3[i][j] = m1[i][j] + m2[i][j];

   printf("Sucet matic c.1 a 2 je v matici c.3\n");
}

void vymen(int m1[][N], int m2[][N])
{
   int i, j, prvok;

   for (i=0; i<N; i++)
      for (j=0; j<N; j++) {

         prvok = m1[i][j];
         m1[i][j] = m2[i][j];
         m2[i][j] = prvok;
	}

   printf("Matice c.1 a 2 su vymenene.\n");
}

void nasob(int m1[][N], int m2[][N], int m3[][N])
{
    int i, j, k, x=0;
    for (i=0; i<N; i++)
      for (j=0; j<N; j++)
        {
            m3[i][j] = 0;
        }

   for (i=0; i<N; i++)
      for (j=0; j<N; j++)
        for(k=0; k <N; k++)
           m3[i][j]+= m1[i][k] * m2[k][j];

}
