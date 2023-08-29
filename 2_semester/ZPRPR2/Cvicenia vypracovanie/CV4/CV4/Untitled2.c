#include <stdio.h>
#include <stdlib.h>

#define N 7

int jePrvocislo(int sucet);

int jePrvocislo(int sucet)
{
    int i;
    float odmocnina = sqrt(sucet);

    for(i=2; i<=odmocnina; i++){
        if(sucet % i == 0)
            return 0;
    }
    return 1;
}
int main()
{
    FILE *fr = fopen("subor.txt","r");
    int size=N, *pole,cislo,i=0, j, sucet=0, dopln=1;
    pole = (int *)malloc(size*sizeof(int));
    while(fscanf(fr,"%d",&cislo)!= EOF)
    {
        *(pole + i++)=cislo;
        sucet=sucet+*(pole+i-1);
        if(i == size)
        {
            size = size + N;
            pole = (int *)realloc(pole,size*sizeof(int));
        }
    }
    if(jePrvocislo(sucet)==0)
    {
        while(jePrvocislo(sucet+dopln) != 1){
            dopln++;
        }
        pole[i]=dopln;
        sucet+=dopln;
        i++;
    }

    for(j=0;j<i;j++)
    {
        printf("%d ",pole[j]);
    }
    printf("\n%d\n",sucet);
    free(pole);
    return 0;
}
