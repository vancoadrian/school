#include <stdio.h>
#include <stdlib.h>
#define N 1000
/* Autor: Adrian Vanco */

void prehod(int *pole, char *poleY, char druh, int pozicia, int pocet);

void prehod(int *poleX, char *poleY, char druh, int pozicia, int pocet)
{
    int pom,j;  //pom=pomocna pre prehodenie hodnot v poli
    pom=poleX[pozicia]; //ulozenie hodnoty do pomocnej
       for(j=pozicia+1;j<pocet;j++) //hladanie vojaka rovnakeho druhu s menej mesiacmi ako aktualny vojak
       {
           if(poleY[j]==druh && poleX[j]<pom)
           {
               poleX[pozicia]=poleX[j]; //prehodenie
               poleX[j]=pom;
               break;
           }
       }
}

int main()
{
    int i,pocet=0, pozicia, x[N]; //pocet=pocet prvkov v poli-1
    char y[N],c;

    for(i=0;i<N;i++)    //Vynulovanie poli
    {
        y[i]='\0';
        x[i]='\0';
    }

    while((c=getchar())!='\n') //Nacitavanie do poli az po enter
    {
        if (pocet==N-1) break; //Nacitanie maximalne 1000 vojakov
        if(c=='A'||c=='B')
        {
            y[pocet]=c; //y[]=pole pre druh vojaka
            scanf("%d",&x[pocet]); //x[]=pole pre mesiace vojaka
            pocet++;
        }
    }
    /*
        Sort, ktory funguje ako Bubble sort ide od prveho prvku a zistuje, ci
        je v poli A alebo B a nasledne volá funkciu, prehod.
    */
    for(i=0;i<pocet;i++)
    {
        for(pozicia=0;pozicia<pocet-1;pozicia++)
        {
            if(y[pozicia]=='A')
            {
                prehod(x,y,'A',pozicia,pocet);//Posielam polia, znak pre kontretné pismeno, poziciu a pocet
            }
            if(y[pozicia]=='B')
            {
                prehod(x,y,'B',pozicia,pocet);
            }
        }
    }
    pozicia=0;  //Vynulovanie pozicie na nulu pre pouzitie vo vypise
    while(y[pozicia]!='\0') //vypis uz usporiadanych vojakov
    {
        printf("%c%d ",y[pozicia],x[pozicia]);
        pozicia++;
    }
    return 0;
}
