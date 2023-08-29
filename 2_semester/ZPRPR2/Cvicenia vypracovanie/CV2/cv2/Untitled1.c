#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main()
{
    srand((unsigned)time(NULL));
    FILE *fr

    fr=fopen("hadanka.txt","r");

    for(int i = 0; i<5; i++)
        printf(" %d ", rand());


    printf("hadanie po pismenkach (p) alebo po slovach (s)?\n");
    printf("zadajte pismeno:\n");
    printf("zadajte slovo:\n");
    printf("Bingo!\n");

    return 0;
}
