#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 100

int main()
{
    char zasif[N], sprava[N], heslo[N];
   scanf("%s",sprava);
   scanf("%s",heslo);
    if(sprava[0] != '\0' && heslo[0] != '\0')
    {
        int pozicia;
            for(pozicia = 0;sprava[pozicia] != '\0';pozicia++)
            {
                zasif[pozicia]=sprava[pozicia]+heslo[pozicia%strlen(heslo)]-2*'A';
                zasif[pozicia]=zasif[pozicia]%26;
                printf("%c",zasif[pozicia]+'A');
            }
            printf("\n");
    }
    return 0;
}
