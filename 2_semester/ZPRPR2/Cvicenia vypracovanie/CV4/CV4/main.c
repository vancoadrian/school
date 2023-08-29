#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 1000
char *spojRetazce(char *str1,char *str2);

char* spojRetazce(char* str1, char* str2) {


	int i = 0, j = 0, k = 0;
	char* spojene;
	spojene = (char *)malloc(strlen(str1) + strlen(str2));
    while (i != strlen(str1) + strlen(str2))
    {
        if (str1[j] != '\0' && i % 2 == 0)
            spojene[i] = str1[j++];
        else if(str2[k]=='\0' && str1[j]!='\0')
            spojene[i]=str1[j++];
        else
            spojene[i] = str2[k++];
        i++;
    }
	spojene[i]='\0';
	return spojene;

}

int main() {

	char str1[100], str2[100], * res;

	scanf("%99s%99s", str1, str2);

	res = spojRetazce(str1, str2);

	printf("%s\n", res);

}
