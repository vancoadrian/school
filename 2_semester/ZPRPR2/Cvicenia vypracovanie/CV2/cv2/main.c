#include <stdio.h>
#include <stdlib.h>


void vloz(int cislo, int pole[],int *pocet);


void vloz(int cislo, int pole[],int *pocet)
{

    int i;

    for(i=*pocet-1; i >= 0; i--)
    {
        if(cislo>pole[i])
            break;
        pole[i+1] = pole[i];

    }
    pole[i+1] = cislo;
    (*pocet)++;
}


int main() {

	int pocet, * pole, i, cislo;
	scanf("%d", &pocet);
	pole = (int*)malloc((pocet + 1) * sizeof(int));
	for (i = 0; i < pocet; i++) {
		scanf("%d", pole + i);
	}
	scanf("%d", &cislo);
	vloz(cislo, pole, &pocet);
	for (i = 0; i < pocet; i++) {
		printf("%d ", pole[i]);
	}
	printf("\n");
	return 0;

}
