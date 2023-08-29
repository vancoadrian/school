#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int cislica(char c);
int male_pismeno(char c);
int velke_pismeno(char c);
int biely(char c);
int zisti(FILE *fr,int (*fun_ptr)(c));


int cislica(char c){ if (isdigit(c)!= 0) return 1; else return 0;}
int male_pismeno(char c){ if (islower(c) != 0) return 1; else return 0;}
int velke_pismeno(char c){ if (isupper(c) != 0) return 1; else return 0;}
int biely(char c){ if(isspace(c) != 0) return 1; else return 0;}

int zisti(FILE *fr,int(*fun_ptr)(c))
{
    int pocet = 0;
    char znak='x';

    while((znak = fgetc(fr)) != EOF)
    {
        pocet+=((*fun_ptr)(znak));
    }
    rewind(fr);

    return pocet;
}
int main()
{
    FILE *fr;

    if((fr=fopen("znaky.txt","r")) == NULL)
    {
        printf("Chyba otvarania");
        return 0;
    }
    else
    {
        int count, i;
        char c,_;

        scanf("%d%c",&count,&_);

        for(i=0; i < count; i++)
        {
            scanf("%c%c",&c,&_);

            switch(c)
            {
                case 'b':   { printf("%d\n",zisti(fr, biely)); break;}
                case 'v':   { printf("%d\n",zisti(fr, velke_pismeno)); break;}
                case 'm':   { printf("%d\n",zisti(fr, male_pismeno)); break;}
                case 'c':   { printf("%d\n",zisti(fr, cislica)); break;}
               // default: continue;
            }
        }

        if(fclose(fr) == EOF)
            printf("Chyba zatvarania");
        return 0;

    }
}

