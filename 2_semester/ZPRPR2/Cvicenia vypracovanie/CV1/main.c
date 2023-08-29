/*#include <stdio.h>
#include <stdlib.h>

int main()
{
    int n, i, cisla[1000];

    scanf("%d",&n);
    for(i = 0;i < n;i++){
        scanf("%d",&cisla[i]);
    }
    if(cisla[0]<=10 && cisla[0]>=0){
        i=1;
        while(i < n){
            if(cisla[i] < 2*cisla[i-1] && cisla[i] > 2/cisla[i-1])
                i++;
            else {printf("Postupnost nie je spravna\n");return 0;}
            }
        if(i==n){printf("Postupnost je spravna\n");return 0;}

        }
    printf("Postupnost nie je spravna\n");
    return 0;
    }


*/
