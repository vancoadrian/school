#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

//ukazovatel pre zapamätanie zaèiatku pamäte
void* memory = NULL;

//deklaracie funkcii
void memory_init(void* ptr, unsigned int size);
void* memory_alloc(unsigned int size);
int memory_free(void* valid_ptr);
int memory_check(void* ptr);
void test1(char* region, int memorySize, int blockSize);
void test2(char* region, int memorySize, int minBlock, int maxBlock);




//slúži na inicializáciu spravovanej vo¾nej pamäte
void memory_init(void* ptr, unsigned int size) {

	//podmienka na ošetrenie nedostatoènej velkosti na správu
	if (size < 13) {
		printf("Too small memory! (MINIMUM 13B)\n");
	}
	else {
		//inicializácia pamäte

		memory = (void*)ptr;        //inicializácia ukazovatela na zaèiatok spravovanej pamäte

		int* memorySize = (int*)memory;     //inicializácia ukazovatela memorySize na prve 4B spravovanej pamäte
		int* memoryHead = (int*)memory + 1;     //inicializácia ukazovatela memoryHead druhe 4B spravovanej pamäte
		int* memoryFooter = ((int*)((char*)memory + size - sizeof(int)));       //inicializácia ukazovatela memoryFooter na posledne 4B spravovanej pamäte

		memset(memory, 0, size);        //vynulovanie spravovanej pamäte pre lepšiu vizualizáciu

		*memorySize = size - (3 * sizeof(int));     //celková ve¾kos pamäte, ktorú možem alokova uložená na prvých 4B
		*memoryHead = size - (3 * sizeof(int));     //hlavièka poèiatoèného bloku druhé 4B (celková ve¾kos pamäte, ktorú možem alokova) 
		*memoryFooter = size - (3 * sizeof(int));       //päta poèiatoèného bloku posledné 4B (celková ve¾kos pamäte, ktorú možem alokova)

	}
}





//slúži na alokáciu èasti spravovanej vo¾nej pamäte
void* memory_alloc(unsigned int size) {

	//ošetrenie ak ešte nebola pridelená pamä na správu alebo ak je ve¾kos 0
	if (memory == NULL || size == 0) {

		return NULL;        //nie je možné alokova

	}

	int* memoryHeader = (int*)memory + 1;       //prvá hlavièka v pamäti
	int* memoryEnd = ((int*)((char*)memory + *((int*)memory) + 2 * sizeof(int)));       //posledná päta v pamäti
	int* memorySize = NULL;      //velkost prechádzaného bloku
	int* memoryForAlloc = NULL;     //ukazovate¾ na hlavièku bloku pre alokáciu

	char* memoryPtr = (char*)memoryHeader;      //ukazovate¾ pre posúvanie sa po bytoch (poèiatoène iba pridelená adresa z pamäte)
	char* memoryFooter = memoryPtr;     //ukazovate¾ na pätu prechádzaného bloku (poèiatoène iba pridelená adresa z pamäte)


	//hladanie best fit
	while (memoryFooter != (char*)memoryEnd) {

		memorySize = (int*)memoryPtr;       //velkost prechádzaného bloku

		//posuvanie
		if (*memorySize < 0) {

			memoryFooter = memoryPtr + (*memorySize) * (-1) + sizeof(int);
			memoryPtr += (*memorySize) * (-1) + 2 * sizeof(int);

		}
		else {

			memoryFooter = memoryPtr + *memorySize + sizeof(int);
			memoryPtr += *memorySize + 2 * sizeof(int);

		}

		//ak sa našiel presne velký blok ako požadovaná velkos uložíme adresu a skonèíme s hladaním
		if (*memorySize > 0 && *memorySize == size) {

			if (memoryForAlloc) {
				memoryForAlloc = memorySize;
			}
			else {
				memoryForAlloc = memorySize;
			}
			break;
		}
		else {
			//ak blok nie je presne velky ako pozadovany hlada sa velkostne väèši ale aby bol èo najbližšie k požadovanej velkosti
				//ak sa už už našiel nejaky vyhovujúci
			if (memoryForAlloc) {
				//porovnanie èi nie je úspornejší aktuálny
				if (*memorySize > 0 && (unsigned)(*memorySize) > size && *memoryForAlloc > *memorySize) {
					memoryForAlloc = memorySize;
				}
			}
			else {
				if (*memorySize > 0 && (unsigned)(*memorySize) > size) {
					memoryForAlloc = memorySize;
				}
			}
		}
	}


	//kontrola èi sa našiel vyhovujúci volný blok
	if (memoryForAlloc == NULL || *memoryForAlloc < 0) {
		return NULL;        //ak sa nenašiel dostatoène velký volný blok 
	}
	else {
		//rozhodovanie èi sa oplati rozdelit blok aby nevznikli fragmenty
		if ((unsigned)(*memoryForAlloc) > size + 2 * sizeof(int)) {

			//rozdelujem       
			memoryPtr = (char*)memoryForAlloc + *memoryForAlloc + sizeof(int);      //nealokovany päta
			*((int*)memoryPtr) = *memoryForAlloc - size - 2 * sizeof(int);

			memset(memoryForAlloc + 1, -1, size);		//memset pre lepšiu vizualizáciu 

			memoryPtr = (char*)memoryForAlloc + size + sizeof(int);     //alokovana päta      
			*((int*)memoryPtr) = -(int)size;


			*((int*)memoryPtr + 1) = *memoryForAlloc - size - 2 * sizeof(int);      //nealokovaná hlavièka

			*memoryForAlloc = -(int)size;       //alokovaná hlavièka
		}
		else {

			//nerozdelujem

			memset(memoryForAlloc + 1, -1, *memoryForAlloc);		//memset pre lepšiu vizualizáciu

			memoryPtr = (char*)memoryForAlloc + *memoryForAlloc + sizeof(int);       //päta
			*((int*)memoryPtr) = -(*memoryForAlloc);        //päta na záporné
			*memoryForAlloc = -(*memoryForAlloc);       //hlavièka na záporné 

		}

		return memoryForAlloc + 1; //vratenie ukazovatela na pridelenú pamä

	}
}





//slúži na uvo¾nenie vyhradeného bloku pamäti
int memory_free(void* valid_ptr) {

	int* memorySize = (int*)valid_ptr - 1;      //hlavièka pamäti, ktorá sa má uvolni
	int* memoryBegin = (int*)memory + 1;        //prvá hlavièka v pamäti
	int* memoryEnd = ((int*)((char*)memory + *((int*)memory) + 2 * sizeof(int)));       //posledná päta v pamäti
	int* leftFooter = NULL;     //prvá päta na ¾avo od pamäti na uvolnenie
	int* rightHeader = NULL;       //prvá hlavièka na pravo od pamäti na uvolnenie

	char* memoryPtrEnd = (char*)valid_ptr - (*memorySize);
	char* leftHeader = NULL;        //prvá hlavièka na ¾avo od pamäti na uvolnenie
	char* rightFooter = NULL;       //prvá päta na pravo od pamäti na uvolnenie

	bool left = true;       //premenná èi sa môže pozrie na ¾avo od uvolòovanej pamäte
	bool right = true;      //premenná èi sa môže pozrie na pravo od uvolòovanej pamäte

	//hlavièka uvolnovanej pamäti je prvá hlavièka v pamäti
	if (memoryBegin == memorySize) {
		left = false;       //nemôže sa pozrie do ¾ava
	}
	//päta uvolnovanej pamäti je posledná päta v pamäti
	if (memoryEnd == (int*)memoryPtrEnd) {
		right = false;      //nemôže sa pozrie do prava
	}

	//spájanie
	if (left && right) {

		leftFooter = memorySize - 1;        //prvá päta na ¾avo od pamäti na uvolnenie
		rightHeader = (int*)memoryPtrEnd + 1;       //prvá hlavièka na pravo od pamäti na uvolnenie

		//kontrola èi je blok pamäte na pravo volný 
		if (*rightHeader > 0) {
			//spojenie s blokom na pravo
			rightFooter = (char*)rightHeader + *rightHeader + sizeof(int);      //päta bloku na pravo
			*memorySize = (*memorySize * (-1)) + *rightHeader + 2 * sizeof(int);      //hlavièka uvolnovaného bloku (nová velkost)
			*((int*)rightFooter) = *memorySize;     //päta bloku na pravo (nová velkost)

			memset(memorySize + 1, 0, *memorySize);		//memset pre lepšiu vizualizáciu

		}
		else {
			*memorySize = (*memorySize * (-1));       //hlavièka, zmena na kladné

			memset(valid_ptr, 0, *memorySize);		//memset pre lepšiu vizualizáciu

			*((int*)memoryPtrEnd) = (*((int*)memoryPtrEnd) * (-1));       //päta, zmena na kladné
		}


		//kontrola èi je blok pamäte na ¾avo volný
		if (*leftFooter > 0) {
			//spojenie s blokom na lavo
			leftHeader = (char*)leftFooter - *leftFooter - sizeof(int);     //hlavièka bloku na ¾avo
			//kontrola prehodene velkost na kladnu v pripade spojenia s pravym
			if (*memorySize < 0) {
				*((int*)leftHeader) = (*memorySize * (-1)) + *leftFooter + 2 * sizeof(int);       //hlavièka bloku na ¾avo (nova velkos)
				memoryPtrEnd = (char*)valid_ptr - (*memorySize);		//päta uvolnovaného bloku
				*((int*)memoryPtrEnd) = (*memorySize * (-1)) + *leftFooter + 2 * sizeof(int);		//päta uvolnovaného bloku (nova velkos)
			}
			else {
				*((int*)leftHeader) = *memorySize + *leftFooter + 2 * sizeof(int);		//hlavièka bloku na ¾avo (nova velkos)
				memoryPtrEnd = (char*)valid_ptr + *memorySize;		//päta uvolnovaného bloku
				*((int*)memoryPtrEnd) = *memorySize + *leftFooter + 2 * sizeof(int);		//päta uvolnovaného bloku (nova velkos)
			}

			memset(leftHeader + 4, 0, *((int*)leftHeader));		//memset pre lepšiu vizualizáciu
		}
		return 0;

	}
	else if (right) {

		rightHeader = (int*)memoryPtrEnd + 1;

		//kontrola èi je blok pamäte na pravo volný
		if (*rightHeader > 0) {
			//spojenie s blokom na pravo
			rightFooter = (char*)rightHeader + *rightHeader + sizeof(int);      //päta bloku na pravo
			*memorySize = (*memorySize * (-1)) + *rightHeader + 2 * sizeof(int);      //hlavièka uvolnovaného bloku (nová velkost)
			*((int*)rightFooter) = *memorySize;     //päta bloku na pravo (nová velkost)

			memset(memorySize + 1, 0, *memorySize);		//memset pre lepšiu vizualizáciu

		}
		else {
			*memorySize = (*memorySize * (-1));       //hlavièka, zmena na kladné

			memset(valid_ptr, 0, *memorySize);		//memset pre lepšiu vizualizáciu

			*((int*)memoryPtrEnd) = (*((int*)memoryPtrEnd) * (-1));       //päta, zmena na kladné
		}

		return 0;

	}
	else if (left) {

		leftFooter = memorySize - 1;
		//kontrola èi je blok pamäte na ¾avo volný
		if (*leftFooter > 0) {
			//spojenie s blokom na lavo
			leftHeader = (char*)leftFooter - *leftFooter - sizeof(int);		//hlavièka bloku na ¾avo
			*((int*)leftHeader) = (*memorySize * (-1)) + *leftFooter + 2 * sizeof(int);		//hlavièka bloku na ¾avo (nova velkos)
			memoryPtrEnd = (char*)valid_ptr - (*memorySize);		//päta uvolnovaného bloku
			*((int*)memoryPtrEnd) = (*memorySize * (-1)) + *leftFooter + 2 * sizeof(int);		//päta uvolnovaného bloku (nova velkos)

			memset(leftHeader + 4, 0, *((int*)leftHeader));		//memset pre lepšiu vizualizáciu

		}
		else {
			*memorySize = (*memorySize * (-1));       //hlavièka, zmena na kladné

			memset(valid_ptr, 0, *memorySize);		//memset pre lepšiu vizualizáciu

			*((int*)memoryPtrEnd) = (*((int*)memoryPtrEnd) * (-1));       //päta, zmena na kladné
		}

		return 0;

	}
	else {

		*memorySize = (*memorySize * (-1));       //hlavièka, zmena na kladné

		memset(valid_ptr, 0, *memorySize);		//memset pre lepšiu vizualizáciu

		*((int*)memoryPtrEnd) = (*((int*)memoryPtrEnd) * (-1));       //päta, zmena na kladné

		return 0;

	}

	return 1;
}





/*
	slúži na skontrolovanie, èi parameter (ukazovate¾) je platný ukazovate¾,
	ktorý bol v nejakom z predchádzajúcich volaní vrátený funkciou memory_alloc
	a zatia¾ nebol uvo¾nený funkciou memory_free.
	Funkcia vráti 0, ak je ukazovate¾ neplatný, inak vráti 1.
*/
int memory_check(void* ptr) {

	//kontrola èi pointer nieje NULL
	if (memory == NULL || ptr == NULL) {
		return 0;
	}

	int* memoryBlockSize = (int*)memory + 1;    //ve¾kos celkovej pamäte pre alokáciu

	void* memoryStart = (void*)((int*)memory + 2);      //zaèiatok celkovej pamäte pre alokáciu
	void* memoryEnd = (void*)((char*)memory + *((int*)memory) + 2 * sizeof(int));       
	void* memoryFooter = NULL;
	
	//kontrola èi je zo spravovanej pamäte
	if (ptr >= memoryStart && ptr < memoryEnd) {
		//cyklus na prechadzanie pamäte po allokovaných blokoch
		do {
			//kontrola zhody
			if (memoryStart == ptr) {
				//kontrola èi je blok alokovaný
				if (*((int*)ptr - 1) < 0) {
					return 1;       //je alokovaný (platný)
				}
				else {
					return 0;       //nie je alokovaný (neplatný)
				}
			}
			//posuvanie sa v pamäti podla toho èi je blok alokovaný alebo nie
			if (*memoryBlockSize > 0) {
				memoryStart = (void*)((char*)memoryStart + *memoryBlockSize + 2 * sizeof(int));
				memoryBlockSize = (int*)memoryStart - 1;
			}
			else {
				memoryStart = (void*)((char*)memoryStart - (*memoryBlockSize) + 8);
				memoryBlockSize = (int*)memoryStart - 1;
			}

		} while ((void*)((char*)memoryStart - 2 * sizeof(int)) != memoryEnd);

		return 0;       //nenašla sa zhoda (neplatný)

	}
	else {

		return 0;       //nie je z pamäte (neplatný)

	}

}





//test na alokovanie rovnakých blokov
void test1(char* region, int memorySize, int blockSize) {

	if (memorySize == 0 || blockSize == 0) {
		return;
	}

	memory_init(region, memorySize);

	int ideal = memorySize / blockSize;		//idealny pocet alokovanych
	int count = 0;		//pocet alokovanych
	float percent = 0;		//count / ideal

	while (memory_alloc(blockSize) != NULL) {
		count++;
	}

	percent = 100 * count / ideal;

	printf("Test 1: memory size %d, block size %d, ideal %d, allocated %d, count/ideal %.2f %%\n",memorySize,blockSize,ideal,count,percent);

}


void test2(char* region, int memorySize, int minBlock, int maxBlock) {

	if (memorySize == 0 || minBlock == 0 || maxBlock == 0) {
		return;
	}

	memory_init(region, memorySize);
	srand(time(0));
	int blockSize = 0;		//premenna na generovanie velkosti bloku z intervalu <minBlock, maxBlock>
	int ideal = 0;		//idealny pocet alokovanych
	int idealSize = 0;	//idealna pocet alokovanych B
	int countblockSize = 0;
	int count = 0;		//pocet alokovanych
	float percent = 0;		//count/ideal
	
	printf("\nBlocks for test 2: ");

	while (countblockSize <= memorySize) {

		blockSize = (rand() % (maxBlock - minBlock + 1)) + minBlock;	//generovanie velkosti bloku z intervalu <minBlock, maxBlock>
		printf("%d ", blockSize);
		countblockSize += blockSize;

		if (idealSize + blockSize <= memorySize) {
			idealSize += blockSize;
			ideal++;		
		}

		if (memory_alloc(blockSize) != NULL) {
			count++;
		}
	}
	printf("\n");
	if(ideal != 0 ){
		percent = 100 * count / ideal;
	}

	printf("Test 2: memory size %d, ideal size %d, allocated %d, count/ideal %.2f %%\n", memorySize, idealSize, count, percent);

}





int main()
{
	//char region[100000];

	//testovanie pridelovania rovnakých 8-24B bloky
	/*
	for (int i = 8; i <= 24; i++) {
		test1(region, 25, i);
		test1(region, 50, i);
		test1(region, 75, i);
		test1(region, 100, i);
		test1(region, 150, i);
		test1(region, 200, i);
		printf("\n");
	}
	*/

	//testovanie roznej velkosti blokov
	
	//for (int i = 0; i < 10; i++) {

		//nerovnake bloky 8-24B do 200B pamäte
		/*
		test2(region, 25, 8, 24);
		test2(region, 50, 8, 24);
		test2(region, 75, 8, 24);
		test2(region, 100, 8, 24);
		test2(region, 150, 8, 24);
		test2(region, 200, 8, 24);
		printf("\n");
		*/

		//nerovnake bloky 500-5000B do 100000B pamäte
		/*
		test2(region, 500, 500, 5000);
		test2(region, 1000, 500, 5000);
		test2(region, 2000, 500, 5000);
		test2(region, 5000, 500, 5000);
		test2(region, 7500, 500, 5000);
		test2(region, 10000, 500, 5000);
		test2(region, 25000, 500, 5000);
		test2(region, 50000, 500, 5000);
		test2(region, 75000, 500, 5000);
		test2(region, 100000, 500, 5000);
		printf("\n");
		*/

		//nerovnake bloky 8-50000B do 100000B pamäte
		/*
		test2(region, 500, 8, 50000);
		test2(region, 1000, 8, 50000);
		test2(region, 2000, 8, 50000);
		test2(region, 5000, 8, 50000);
		test2(region, 7500, 8, 50000);
		test2(region, 10000, 8, 50000);
		test2(region, 25000, 8, 50000);
		test2(region, 50000, 8, 50000);
		test2(region, 75000, 8, 50000);
		test2(region, 100000, 8, 50000);
		printf("\n");
		*/
	//}




	//testovanie memory free

	//memory_init(region, 64);
	/*
	char* pointer0 = (char*)memory_alloc(52);
	char* pointer1 = (char*)memory_alloc(4);
	char* pointer2 = (char*)memory_alloc(24);
	char* pointer3 = (char*)memory_alloc(4);
	char* pointer4 = (char*)memory_alloc(40);
	*/

	//testovanie check
	/*
	printf("memory check %d \n", memory_check(pointer0));
	printf("memory check %d \n", memory_check(pointer1));
	printf("memory check %d \n", memory_check(pointer2));
	printf("memory check %d \n", memory_check(pointer3));
	printf("memory check %d \n", memory_check(pointer4));
	printf("memory check %d \n", memory_check(NULL));
	*/

	//test spajania najpr s ¾avým blokom potom s pravým blokom
	/*
	memory_free(pointer1);		
	memory_free(pointer2);
	memory_free(pointer3);
	*/


	//test spajania najpr s pravým blokom potom s ¾avým blokom
	/*
	memory_free(pointer3);
	memory_free(pointer2);
	memory_free(pointer1);
	*/


	//test spajania najpr s pravým a ¾avým blokom
	/*
	memory_free(pointer3);
	memory_free(pointer2);
	memory_free(pointer1);
	*/

	//testovanie check
	/*
	printf("\nmemory check %d \n", memory_check(pointer0));
	printf("memory check %d \n", memory_check(pointer1));
	printf("memory check %d \n", memory_check(pointer2));
	printf("memory check %d \n", memory_check(pointer3));
	printf("memory check %d \n", memory_check(pointer4));
	printf("memory check %d \n", memory_check(NULL));
	*/

	return 0;
}