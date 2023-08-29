#include <stdio.h>
#include <stdlib.h>

#define N 20

int magicky(int pole[][N],int n);

int magicky(int pole[][N],int n)
{
    int diag1 = 0,diag2=0;

    for (int i = 0; i < n; i++)
        diag1 = diag1 + pole[i][i];

    for (int i = 0; i < n; i++)
        diag2 = diag2 + pole[i][n-1-i];

    if(diag1!=diag2)
        return 0;

    for (int i = 0; i < n; i++) {

        int rowSum = 0;
        for (int j = 0; j < n; j++)
            rowSum += pole[i][j];

        if (rowSum != diag1)
            return 0;
    }

    for (int i = 0; i < n; i++) {

        int colSum = 0;
        for (int j = 0; j < n; j++)
            colSum += pole[j][i];

        if (diag1 != colSum)
        return 0;
    }

    return 1;
}


int main()
{
    int n, i, j, m1[N][N];
    printf("Zadaj rozmer matice (max. 20): ");
    scanf("%d",&n);
    if(n > N || n < 1)
    {
        printf("Zle zadany rozmer matice\n");
        return 0;
    }
    else
    {
        for(i=0; i < n; i++)
            for(j=0; j < n; j++)
            {
                scanf("%d",&m1[i][j]);
            }
        if(magicky(m1, n)) printf("magicky\n");
        else printf("nie magicky\n");
        return 0;

    }
}
