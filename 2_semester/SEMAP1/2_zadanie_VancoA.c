#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//declarations
int* subPalindromes(char* s,int *najdlhsi,int *velkost);
void longestPalindrome(char* s, int najdlhsi, int* dlzky, int velkost);

int* subPalindromes(char* s, int *najdlhsi,int *velkost) {

    int* len = NULL;    //vynulovanie pointrov
    char* tmp = NULL;
    int len1 = strlen(s);   //ulozenie velkosti stringu do len1
    int len2 = 2 * len1;    //dvojnasabok velkosti dlzky pola len1 pre nové pole, ktore bude obsahovat povodny retazec a medzi kazde pismeno sa vlozi specialny znak aby funkcia fungovala aj pre parny palindrom
    *velkost = len2;    // ulozenie aj do premennej v maine
    tmp = (char*)malloc(sizeof(char) * len2 + 3); // +3 lebo adding '@'、'$'(as boundary) and '\0'(as end)
            //krajne znaky musia byt rozdielne a aj voci specialnemu znaku, ktory vkladame medzi pismena retazca musia byt ine aby sa pri porovnavani pravej a lavej strany palindromu nemohlo zvecsit viac ako je spravne
            //keby by boli krajne body rovnake palindrom s zaciatkom v strede retazca by mohol mat dlzku vecsiu o 1. A to aj keby boli podobne so specialnym znakom, dlzka palindromu na hoci, ktorej pozici by sa mohla zvecsit o 1 viac
    if (!tmp) {printf("Chyba alokovania (char* tmp)\n"); return 0;} //kontrolovanie ci sa dinamicke pole alokovalo.
    len = (int*)malloc(sizeof(int) * len2 + 3); //alokovanie pola pre dlzky palindromov podla indexov pola
    if (!len) {printf("Chyba alokovania (int* len)\n"); return 0;}  //kontrolovanie ci sa dinamicke pole alokovalo.

    //init
    tmp[0] = '@'; //prvy krajny znak
    for (int i = 1; i <= len1; i++) { //nacitavanie do dinamickeho pola a vkladanie specialneho znaku medzi pismena
        int d = 2 * i;
        tmp[d - 1] = '#';
        tmp[d] = s[i - 1];
    }
    tmp[len2 + 1] = '#';
    tmp[len2 + 2] = '$';    //druhy krajny znak

    // loop for finding the substring
    int max = 0, max_len = 0, max_id = 0;
    for (int i = 1; i <= len2; i++) {   //ideme od indexu 1 pretoze na nultom indexe mame hranicny znak

        if (max > i) {
                //Prvá podmienka prideluje dĺžku podľa toho čí sub-palindróm na pravej strane vytŕča z palindrómu alebo nie ak vytŕča nastaví sa dĺžka max-i
                //ak nie tak sa nastaví dĺžka pre sub-palindrom na pravej strane na dĺžku od zrkadlového sub-palindrómu na pravej strane palindrómu.
            len[i] = len[2 * max_id - i] > (max - i) ? (max - i) : len[2 * max_id - i];
        }
        else {
            len[i] = 1;
        }

        for (; tmp[i + len[i]] == tmp[i - len[i]]; len[i]++); //A od nastavejen dlzky z podmienky ide kontrolovanie pravej a lavej strany od stredu palindrom ide pokial su zrkadlove znaky rovnake

        //update max
        if (i + len[i] > max) { //max ukazuje na znak za hranicou palindrómu na pravej strane a max_id na znak ktorý bol max v predchádzajúcom palindrome
            max = len[i] + i;
            max_id = i;
        }

        //update max_len ked najde vacsi palindrom
		if (len[i] > max_len) {
			max_len = len[i];
		}
    }

    // the result is begin with s[(final_id +1)/2 - max_len/2 - 1], the len is max_len - 1
    max_len -= 1;   //skratenie dlzky o dlzku, ktoru palindrom nadobudol vdaka specialnemu znaku
    *najdlhsi= max_len; //ulozenie dlzky do main premennej najdlhsi
    free(tmp);  //ulovnenie pola na ktore ukazuje tmp
    return len; //vraciam ukazovatel na pole s dlzkami
}

void longestPalindrome(char* s,int najdlhsi,int *dlzky, int velkost){
    char* result = NULL;
    int final_id;
    printf("%d ",najdlhsi); //vypis velkosti najdlhsieho palindromu/ov
    for(int i=1;i<=velkost;i++){    //hladanie po indexoch pola, v ktorom su ulozene velkosti palindromov. Indexe je stredom palindromu
        if (dlzky[i] == najdlhsi+1){    //ak sa velkosti najvecsieho palindromu zhoduje s velkostou v poli, tak vypiseme palindrom (+1 lebo sme ho zmensovali v predchadzajucej funkcii o dlzku, ktoru nadobuda vdaka #)
            final_id = (i + 1) / 2; //stred palindromu vypocitaneho pre povodne pole bez specialnych znakov
            result = (char*)realloc(result,sizeof(char) * (najdlhsi + 1));  //alokovanie pola na velkost najvacsieho palindromu + ukoncovaci znak
            if (!result) {printf("Chyba alokovania (int* len)\n"); break;}   //kontrola
            memcpy(result, s + final_id - najdlhsi / 2 - 1, najdlhsi);  //ulozenie palindromu do pola na ktore ukazuje result
            result[najdlhsi] = '\0';    //ukoncovaci znak;
            printf("%s ",result);   //vypis palindromu
        }
    }
    printf("\n");
    free(result); //uvolnenie pamate
}


int main()
{
    int najdlhsi, *dlzky, velkost; //premenne
    char s[501];    //pole pre 500 znakov
    while(scanf("%500s",s)==1){ //cyklus, ktory nacita maximalne 500 znakovy retazec, ak je retacez dlhsi zoberie len prvych 500 znakov a dalsie znaky budu v dalsom retazci
        //funkcia, ktora hlada najvecsi palindrom v retazci, jeho velkost dostaneme do premennej najdlhsi,
        //dalej nam funkcia vracia ukazovatel na pole s dlzkami palindromov nachadzajucich sa v stringu a budeme potrebovat vediet aj velkost pola, tak si ju rovno ziskame a ulozime do premennej velkost
    dlzky = subPalindromes(s, &najdlhsi, &velkost);
        //funkcia na vypis velkosti palindromu a uz konkretnych palindromov do ktorej posielame povodne pole bez rozsirenia o specialne znaky, a vsetko potrebne pre vypis.
    longestPalindrome(s,najdlhsi,dlzky,velkost);
    }
    return 0;
}
