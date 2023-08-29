
// zliezanie kopca - najdenie cesty v mape (2D staticke pole)
// najdeme policko s najvyssou vyskou
// budeme hladat cestu (4-mi smermi) tak, aby sa robili co najmensie skoky
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define SUBOR "kopec.txt"

// nacitanie obsahu dvojrozmerneho pola zo suboru
int **nacitaj_mapu(int *n) {
	int **mapa,rows=0 ,colums=0,i=0, j=0, k;
	char znak, riadok[100], *r;
	FILE *f;
	f = fopen(SUBOR, "r");

	while((znak = fgetc(f))!=EOF){
        if(znak == '\n') {
            rows++;
        } else if(znak == ' '){
            continue;
        }else{
            colums++;
        }
	}
	colums/=rows;
	fseek(f,-1,SEEK_END);
	if (fgetc(f)!= '\n')
    {
        rows++;
        colums--;
    }

	mapa = (int **) malloc(rows * sizeof(int *));
	for(k=0;k<rows;k++)
    {
        mapa[k] = (int *) malloc(colums* sizeof(int));
    }

	//printf("%d %d\n",rows,colums);
    rewind(f);
	while(i < rows && fgets(riadok, 100, f)) {
		j = 0;
		r = riadok;
		while(j < colums && r != NULL && sscanf(r, "%d", *(mapa+i)+j) == 1) {
			//printf("%d %d: %d\n", i, j, *(*(mapa+i)+j));
			r = strchr(r, ' ');
			if(r != NULL) r++;
			j++;
		}
		i++;
	}
	if (j != i) *n=0;
	else *n=i;
	return mapa;
}

// najdenie maximalneho prvku: vratenie hodnot cez argumenty
void najdi_kopec(int **mapa, int n, int *ki, int *kj) {
	int i, j;
	*ki = *kj = 0;
	for(i=0; i<n; i++)
		for(j=0; j<n; j++)
			if(mapa[i][j] > mapa[*ki][*kj]) {
				*ki = i;
				*kj = j;
				//printf("i: %d, j: %d, mapa: %d\n", *ki, *kj, mapa[*ki][*kj]);
			}
}

// hladanie v vypis cesty, kde sa robia lokalne o najmensie vyskove rozdiely
void zidi_kopec(int **mapa, int n, int ki, int kj) {
	int i, j, max;
	while (ki != 0 && ki != n-1 && kj != 0 && kj != n-1) {
		max = 0;
		// sever
		if(ki-1 >=0 && mapa[ki-1][kj] > max && mapa[ki-1][kj] < mapa[ki][kj]) {
			i = ki-1;
			j = kj;
			max = mapa[i][j];
		}
		//severo_vychod
        if(ki-1 >=0 && kj+1<n && mapa[ki-1][kj+1] > max && mapa[ki-1][kj+1] < mapa[ki][kj]){
            i=ki-1;
            j=kj+1;
            max = mapa[i][j];
        }
		// vychod
		if(kj+1 <n && mapa[ki][kj+1] > max && mapa[ki][kj+1] < mapa[ki][kj]) {
			i = ki;
			j = kj+1;
			max = mapa[i][j];
		}
        //juho_vychod
        if(ki+1 <n && kj+1<n && mapa[ki+1][kj+1] > max && mapa[ki+1][kj+1] < mapa[ki][kj]){
            i=ki+1;
            j=kj+1;
            max = mapa[i][j];
        }
		// juh
		if(ki+1 <n && mapa[ki+1][kj] > max && mapa[ki+1][kj] < mapa[ki][kj]) {
			i = ki+1;
			j = kj;
			max = mapa[i][j];
		}
        //juho_zapad
        if(ki+1 <n && kj-1>=0 && mapa[ki+1][kj-1] > max && mapa[ki+1][kj-1] < mapa[ki][kj]){
            i=ki+1;
            j=kj-1;
            max = mapa[i][j];
        }
		// zapad
		if(kj-1 >=0 && mapa[ki][kj-1] > max && mapa[ki][kj-1] < mapa[ki][kj]) {
			i = ki;
			j = kj-1;
			max = mapa[i][j];
		}
        //juho_zapad
        if(ki-1 >=0 && kj-1>=0 && mapa[ki-1][kj-1] > max && mapa[ki-1][kj-1] < mapa[ki][kj]){
            i=ki-1;
            j=kj-1;
            max = mapa[i][j];
        }
		//printf("i: %d, j: %d, vyska: %d\n", i, j, max);
		printf("%d ", max);
		ki = i;
		kj = j;
	}
	printf("\n");
}

// hladanie v vypis cesty, kde sa robia lokalne o najmensie vyskove rozdiely
// rekurzivna verzia
void zidi_kopec_rek(int **mapa, int n, int ki, int kj) {
	int i, j, max;
	if (ki != 0 && ki != n-1 && kj != 0 && kj != n-1) {
		max = 0;
				// sever
		if(ki-1 >=0 && mapa[ki-1][kj] > max && mapa[ki-1][kj] < mapa[ki][kj]) {
			i = ki-1;
			j = kj;
			max = mapa[i][j];
		}
		//severo_vychod
        if(ki-1 >=0 && kj+1<n && mapa[ki-1][kj+1] > max && mapa[ki-1][kj+1] < mapa[ki][kj]){
            i=ki-1;
            j=kj+1;
            max = mapa[i][j];
        }
		// vychod
		if(kj+1 <n && mapa[ki][kj+1] > max && mapa[ki][kj+1] < mapa[ki][kj]) {
			i = ki;
			j = kj+1;
			max = mapa[i][j];
		}
        //juho_vychod
        if(ki+1 <n && kj+1<n && mapa[ki+1][kj+1] > max && mapa[ki+1][kj+1] < mapa[ki][kj]){
            i=ki+1;
            j=kj+1;
            max = mapa[i][j];
        }
		// juh
		if(ki+1 <n && mapa[ki+1][kj] > max && mapa[ki+1][kj] < mapa[ki][kj]) {
			i = ki+1;
			j = kj;
			max = mapa[i][j];
		}
        //juho_zapad
        if(ki+1 <n && kj-1>=0 && mapa[ki+1][kj-1] > max && mapa[ki+1][kj-1] < mapa[ki][kj]){
            i=ki+1;
            j=kj-1;
            max = mapa[i][j];
        }
		// zapad
		if(kj-1 >=0 && mapa[ki][kj-1] > max && mapa[ki][kj-1] < mapa[ki][kj]) {
			i = ki;
			j = kj-1;
			max = mapa[i][j];
		}
        //juho_zapad
        if(ki-1 >=0 && kj-1>=0 && mapa[ki-1][kj-1] > max && mapa[ki-1][kj-1] < mapa[ki][kj]){
            i=ki-1;
            j=kj-1;
            max = mapa[i][j];
        }
		//printf("i: %d, j: %d, vyska: %d\n", i, j, max);
		printf("%d ", max);
		zidi_kopec_rek(mapa, n, i, j);
	}
}

int main() {

    int **mapa, n;
	int ki, kj;

	mapa = nacitaj_mapu(&n);
	//printf("rozmer stvorcovej mapy: %d\n", n);
	najdi_kopec(mapa, n, &ki, &kj);
	//printf("suradnice kopca: %d, %d\n", ki, kj);
	//printf("%d\n", *(*(mapa+ki)+kj));
	//printf("%d\n", mapa[ki][kj]);
	zidi_kopec(mapa, n, ki, kj);
	printf("rekurzivne:\n");
	zidi_kopec_rek(mapa, n, ki, kj);
    //printf("\n");
    for(;n>0;n--)
        free(mapa[n-1]);
    free(mapa);
	return 0;
}

