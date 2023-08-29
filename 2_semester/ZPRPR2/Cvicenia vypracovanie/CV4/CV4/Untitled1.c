#include <stdio.h>
#include <stdlib.h>
#define N 4

int *vloz( int cislo, int *pole, int *m, int *n);

int *vloz( int cislo, int *pole, int *m, int *n)
{
    int i , *p_m, *p_n;
    p_m=m;
    p_n=n;
    if(*p_m-1==*p_n)
    {
        *p_m+=N;
        pole = (int*)realloc(pole, sizeof(int) **p_m);
    }

    for(i=*p_n-1;i>=0;i--)
    {
       if(*(pole+i)>cislo)
        *(pole+i+1)=*(pole+i);
        else
            break;
    }
    *(pole+i+1)=cislo;
    *p_n=*p_n+1;
    return pole;

}

int main()
{
    int cislo, i, m = 10, n=0;
    char c;
    int *p = (int*)malloc(m * sizeof(int));
    //printf("Zadaj postupnost cisel: ");
    for(i=0; i<m; i++)
    {
        scanf("%d", p+i);
        n++;
        c = getchar();
        if (c == ' ')
            continue;
        if (c == '\n')
            break;
    }
    //printf("Zadaj cisla ktore chces vlozit: ");
    do {
        scanf("%d",&cislo);
        p=vloz(cislo, p, &m, &n);
    }
    while(c=getchar()!='\n');
    for (i=0; i < n; i++)
        printf("%d ",*(p+i));
    printf("\n");
    free(p);
    return 0;
}
/*
#define N 5

#include <stdio.h>
#include <stdlib.h>

int* vloz(int cislo, int* pole, int* m, int* n) {

	int i, * p_m, * p_n;
	p_m = m;
	p_n = n;
	if (*p_m - 1 == *p_n)
	{
		*p_m += N;
		pole = (int*)realloc(pole, sizeof(int) * *p_m);
	}

	for (i = *p_n-1; i >= 0; i--)
	{
		if (*(pole + i) > cislo)
			*(pole + i + 1) = *(pole + i);
		else
			break;
	}
	*(pole + i + 1) = cislo;
	*p_n = *p_n + 1;
	return pole;
}

int main() {

	int* arr, i, m = 2, n = 0;

	arr = (int*)malloc(2 * sizeof(int));

	while (scanf("%d", &i) == 1)
		arr = vloz(i, arr, &m, &n);

	for (i = 0; i < n; i++)
		printf("%d ", arr[i]);
	printf("\n");

	free(arr);

}*/
