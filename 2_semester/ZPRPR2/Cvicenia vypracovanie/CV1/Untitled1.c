#include <stdio.h>
#include <stdlib.h>

int main()
{
    /*
    FILE *cisla1,*cisla2,*vysledok;
    int cislo1,cislo2;
    cisla1 = fopen ("cisla1.txt", "r");
    cisla2 = fopen ("cisla2.txt", "r");
    vysledok = fopen ("vysledok.txt", "w");

    if((cisla1 = fopen ("cisla1.txt", "r"))!=NULL && (cisla2 = fopen ("cisla2.txt", "r"))!=NULL && (vysledok = fopen ("vysledok.txt", "w"))!=NULL)
    {
        while ((fscanf(cisla1,"%d",&cislo1))!=EOF || (fscanf(cisla2,"%d",&cislo2))!=EOF)
            {
                if(cislo1!=EOF)
                printf(" %d ",cislo1);
                if(cislo2!=EOF)
                printf(" %d ",cislo2);
            }
        if((fclose(cisla1))==EOF || (fclose(cisla2))==EOF || (fclose(vysledok))==EOF)
        {
            printf("Chyba zatvarania\n");
            return 0;
        }

    }
    else
    {
        printf("Chyba otvarania\n");
        return 0;
    }*/
    printf("%d",sizeof(int));
    return 0;
}
