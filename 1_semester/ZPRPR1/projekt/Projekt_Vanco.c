#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

void nacitaj(FILE *fr, char *povodny);
void vypis_povodny(char *povodny);
void nacitaj_upraveny(char *povodny, char *upraveny);
void vypis_upraveny(char *upraveny);
void vypis_slov(char *povodny);
void histogram(char *upraveny);
void rozsifruj(char *upraveny);

int main()
{
    char prikaz = '\0', povodny[1000], upraveny[1000];
    FILE *fr;
    int pozicia;

    if  ((fr = fopen("sifra.txt","r")) != NULL)
    {
        for(pozicia = 0;pozicia < 1000;pozicia++)
            povodny[pozicia] = '\0';
        for(pozicia = 0;pozicia < 1000;pozicia++)
            upraveny[pozicia] = '\0';
        while(prikaz != 'k'){
            scanf("%c",&prikaz);
            switch(prikaz){
                case 'n':  nacitaj(fr, povodny);  break;
                case 'v':  vypis_povodny(povodny);  break;
                case 'u':  nacitaj_upraveny(povodny, upraveny);  break;
                case 's':  vypis_upraveny(upraveny);  break;
                case 'd':  vypis_slov(povodny);  break;
                case 'h':  histogram(upraveny);  break;
                case 'c':  rozsifruj(upraveny);  break;
                case 'p':  pomer(povodny);  break;
                case 'z':  zmazanie(upraveny);  break;
            }
        }
        if  ((fclose(fr))== EOF)
            printf("Subor sa nepodarilo zatvorit\n");
        return 0;
    }
    else
    {
        printf("Subor sa nepodarilo otvorit\n");
        return 0;
    }
}


void nacitaj(FILE *fr, char *povodny)
{
    int pozicia = 0;
    char znak;
    while((znak = fgetc(fr)) != EOF)
    {
        povodny[pozicia++]=znak;
        if  (pozicia > 1000)
            break;
    }
    if(povodny[0] == '\0')
        printf("Spravu sa nepodarilo nacitat\n");
}

void vypis_povodny(char *povodny)
{
    int pozicia;
    if(povodny[0] != '\0')
    {
        for(pozicia = 0; povodny[pozicia] != '\0'; pozicia++)
            printf("%c",povodny[pozicia]);
            printf("\n");
    }
    else
    {
        printf("Sprava nie je nacitana\n");
    }
}

void nacitaj_upraveny(char *povodny, char *upraveny)
{
    int pozicia, i=0;
    if(povodny[0] != '\0')
    {
        for(pozicia = 0; povodny[pozicia] != '\0'; pozicia++)
            if  ((povodny[pozicia] >= 'a' && povodny[pozicia] <= 'z') || (povodny[pozicia] >= 'A' && povodny[pozicia] <= 'Z') )
               upraveny[i++] = toupper(povodny[pozicia]);
    }
    else
    {
        printf("Sprava nie je nacitana\n");
    }
}

void vypis_upraveny(char *upraveny)
{
    int pozicia;
    if(upraveny[0] != '\0')
    {
        for(pozicia = 0; upraveny[pozicia] != '\0'; pozicia++)
            printf("%c",upraveny[pozicia]);
            printf("\n");
    }
    else
    {
        printf("Nie je k dispozicii upravena sprava\n");
    }
}

void vypis_slov(char *povodny)
{
    int k, zaciatok = 0, pozicia;
    if(povodny[0] != '\0')
    {
        scanf("%d",&k);
        if (k >= 1 && k<=100)
        {
            for(pozicia = 0; povodny[pozicia] != '\0';pozicia++)
            {
                if(povodny[pozicia] == ' ' || povodny[pozicia] == '\n')
                {
                    if((pozicia-zaciatok) == k)
                    {
                        for(;zaciatok < pozicia;zaciatok++) printf("%c",povodny[zaciatok]);
                            printf("\n");
                    }
                    zaciatok = pozicia + 1;
                }
            }
            if((pozicia-zaciatok) == k)
                    {
                        for(;zaciatok < pozicia;zaciatok++) printf("%c",povodny[zaciatok]);
                            printf("\n");
                    }
        }
    }
    else
    {
        printf("Sprava nie je nacitana\n");
    }
}

void histogram(char *upraveny)
{
    if (upraveny[0] != '\0')
    {
        int i,j,r;
        float hist[26],najcastejsie = 0;

        for(i = 0; i < 26;i++)
            hist[i] = '\0';

        for(i=0;upraveny[i] != '\0'; i++)
        {
            hist[upraveny[i] - 'A']++;
        }
        for (int j = 0; j < 26; j++)
        {
            hist[j]=hist[j]/i*100.f;
            if(hist[j] > najcastejsie)
                najcastejsie = hist[j];
        }

        for(r = 90;r >= 0;r = r-10)
        {
            if(r >= najcastejsie) continue;
            for(j=0;j<26;j++)
            {
                if(hist[j] > r)
                    printf("*");
                else
                    printf(" ");
            }
            printf("\n");
        }
        for(i = 0;i < 26; i++)
            printf("%c",'A'+i);
        printf("\n");
    }
    else
    {
        printf("Nie je k dispozicii upravena sprava\n");
    }
}

void rozsifruj(char *upraveny)
{
    if(upraveny[0] != '\0')
    {
        int n = 0, pozicia;
        scanf("%d",&n);
        if(n >= 1 && n <= 25)
        {
            for(pozicia = 0;upraveny[pozicia] != '\0';pozicia++)
            {
                if((upraveny[pozicia]-n) < 'A')
                   printf("%c",(26-n)+upraveny[pozicia]);
                else
                printf("%c",upraveny[pozicia]-n);
            }
            printf("\n");
        }
    }
    else
    {
        printf("Nie je k dispozicii upravena sprava\n");
    }
}

void pomer(char *povodny)
{
    int velke = 0,male = 0, pozicia;

    if(povodny[0] != '\0')
    {
        for(pozicia = 0; povodny[pozicia] != '\0'; pozicia++)
            if  (povodny[pozicia] >= 'a' && povodny[pozicia] <= 'z')
                male++;
            else if (povodny[pozicia] >= 'A' && povodny[pozicia] <= 'Z')
                velke++;
                printf("%d:%d\n",velke,male);
    }
    else
    {
        printf("Sprava nie je nacitana\n");
    }

}

void zmazanie(char *upraveny)
{
    int z, k , l = strlen(upraveny), i, j=0,x;

    scanf(" %d %d", &z, &k);
    if (upraveny[0] != '\0')
    {
        if(upraveny[z]!='\0')
        {
             for(i=0;i<l;i++)
            {
                if(i<z) upraveny[j++]=upraveny[i];

                else if(i>k) upraveny[j++]=upraveny[i];
            }
            for(x=l;x>=(l-(k-z+1));x--)
                upraveny[x]='\0';
            printf("Retazec sa zmazal\n");
        }
        else{
            printf("Retazec nie je mozne zmazat\n");
            return;
        }
    }
    else
    {
        printf("Nie je k dispozicii upravena sprava\n");
    }

}
