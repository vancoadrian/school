#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

//ukazovatel pre zapam�tanie za�iatku pam�te
void* memory = NULL;

//deklaracie funkcii
void memory_init(void* ptr, unsigned int size);
void* memory_alloc(unsigned int size);
int memory_free(void* valid_ptr);
int memory_check(void* ptr);
void test1(char* region, int memorySize, int blockSize);
void test2(char* region, int memorySize, int minBlock, int maxBlock);




//sl��i na inicializ�ciu spravovanej vo�nej pam�te
void memory_init(void* ptr, unsigned int size) {

	//podmienka na o�etrenie nedostato�nej velkosti na spr�vu
	if (size < 13) {
		printf("Too small memory! (MINIMUM 13B)\n");
	}
	else {
		//inicializ�cia pam�te

		memory = (void*)ptr;        //inicializ�cia ukazovatela na za�iatok spravovanej pam�te

		int* memorySize = (int*)memory;     //inicializ�cia ukazovatela memorySize na prve 4B spravovanej pam�te
		int* memoryHead = (int*)memory + 1;     //inicializ�cia ukazovatela memoryHead druhe 4B spravovanej pam�te
		int* memoryFooter = ((int*)((char*)memory + size - sizeof(int)));       //inicializ�cia ukazovatela memoryFooter na posledne 4B spravovanej pam�te

		memset(memory, 0, size);        //vynulovanie spravovanej pam�te pre lep�iu vizualiz�ciu

		*memorySize = size - (3 * sizeof(int));     //celkov� ve�kos� pam�te, ktor� mo�em alokova� ulo�en� na prv�ch 4B
		*memoryHead = size - (3 * sizeof(int));     //hlavi�ka po�iato�n�ho bloku druh� 4B (celkov� ve�kos� pam�te, ktor� mo�em alokova�) 
		*memoryFooter = size - (3 * sizeof(int));       //p�ta po�iato�n�ho bloku posledn� 4B (celkov� ve�kos� pam�te, ktor� mo�em alokova�)

	}
}





//sl��i na alok�ciu �asti spravovanej vo�nej pam�te
void* memory_alloc(unsigned int size) {

	//o�etrenie ak e�te nebola pridelen� pam� na spr�vu alebo ak je ve�kos� 0
	if (memory == NULL || size == 0) {

		return NULL;        //nie je mo�n� alokova�

	}

	int* memoryHeader = (int*)memory + 1;       //prv� hlavi�ka v pam�ti
	int* memoryEnd = ((int*)((char*)memory + *((int*)memory) + 2 * sizeof(int)));       //posledn� p�ta v pam�ti
	int* memorySize = NULL;      //velkost prech�dzan�ho bloku
	int* memoryForAlloc = NULL;     //ukazovate� na hlavi�ku bloku pre alok�ciu

	char* memoryPtr = (char*)memoryHeader;      //ukazovate� pre pos�vanie sa po bytoch (po�iato�ne iba pridelen� adresa z pam�te)
	char* memoryFooter = memoryPtr;     //ukazovate� na p�tu prech�dzan�ho bloku (po�iato�ne iba pridelen� adresa z pam�te)


	//hladanie best fit
	while (memoryFooter != (char*)memoryEnd) {

		memorySize = (int*)memoryPtr;       //velkost prech�dzan�ho bloku

		//posuvanie
		if (*memorySize < 0) {

			memoryFooter = memoryPtr + (*memorySize) * (-1) + sizeof(int);
			memoryPtr += (*memorySize) * (-1) + 2 * sizeof(int);

		}
		else {

			memoryFooter = memoryPtr + *memorySize + sizeof(int);
			memoryPtr += *memorySize + 2 * sizeof(int);

		}

		//ak sa na�iel presne velk� blok ako po�adovan� velkos� ulo��me adresu a skon��me s hladan�m
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
			//ak blok nie je presne velky ako pozadovany hlada sa velkostne v��i ale aby bol �o najbli��ie k po�adovanej velkosti
				//ak sa u� u� na�iel nejaky vyhovuj�ci
			if (memoryForAlloc) {
				//porovnanie �i nie je �spornej�� aktu�lny
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


	//kontrola �i sa na�iel vyhovuj�ci voln� blok
	if (memoryForAlloc == NULL || *memoryForAlloc < 0) {
		return NULL;        //ak sa nena�iel dostato�ne velk� voln� blok 
	}
	else {
		//rozhodovanie �i sa oplati rozdelit blok aby nevznikli fragmenty
		if ((unsigned)(*memoryForAlloc) > size + 2 * sizeof(int)) {

			//rozdelujem       
			memoryPtr = (char*)memoryForAlloc + *memoryForAlloc + sizeof(int);      //nealokovany p�ta
			*((int*)memoryPtr) = *memoryForAlloc - size - 2 * sizeof(int);

			memset(memoryForAlloc + 1, -1, size);		//memset pre lep�iu vizualiz�ciu 

			memoryPtr = (char*)memoryForAlloc + size + sizeof(int);     //alokovana p�ta      
			*((int*)memoryPtr) = -(int)size;


			*((int*)memoryPtr + 1) = *memoryForAlloc - size - 2 * sizeof(int);      //nealokovan� hlavi�ka

			*memoryForAlloc = -(int)size;       //alokovan� hlavi�ka
		}
		else {

			//nerozdelujem

			memset(memoryForAlloc + 1, -1, *memoryForAlloc);		//memset pre lep�iu vizualiz�ciu

			memoryPtr = (char*)memoryForAlloc + *memoryForAlloc + sizeof(int);       //p�ta
			*((int*)memoryPtr) = -(*memoryForAlloc);        //p�ta na z�porn�
			*memoryForAlloc = -(*memoryForAlloc);       //hlavi�ka na z�porn� 

		}

		return memoryForAlloc + 1; //vratenie ukazovatela na pridelen� pam�

	}
}





//sl��i na uvo�nenie vyhraden�ho bloku pam�ti
int memory_free(void* valid_ptr) {

	int* memorySize = (int*)valid_ptr - 1;      //hlavi�ka pam�ti, ktor� sa m� uvolni�
	int* memoryBegin = (int*)memory + 1;        //prv� hlavi�ka v pam�ti
	int* memoryEnd = ((int*)((char*)memory + *((int*)memory) + 2 * sizeof(int)));       //posledn� p�ta v pam�ti
	int* leftFooter = NULL;     //prv� p�ta na �avo od pam�ti na uvolnenie
	int* rightHeader = NULL;       //prv� hlavi�ka na pravo od pam�ti na uvolnenie

	char* memoryPtrEnd = (char*)valid_ptr - (*memorySize);
	char* leftHeader = NULL;        //prv� hlavi�ka na �avo od pam�ti na uvolnenie
	char* rightFooter = NULL;       //prv� p�ta na pravo od pam�ti na uvolnenie

	bool left = true;       //premenn� �i sa m��e pozrie� na �avo od uvol�ovanej pam�te
	bool right = true;      //premenn� �i sa m��e pozrie� na pravo od uvol�ovanej pam�te

	//hlavi�ka uvolnovanej pam�ti je prv� hlavi�ka v pam�ti
	if (memoryBegin == memorySize) {
		left = false;       //nem��e sa pozrie� do �ava
	}
	//p�ta uvolnovanej pam�ti je posledn� p�ta v pam�ti
	if (memoryEnd == (int*)memoryPtrEnd) {
		right = false;      //nem��e sa pozrie� do prava
	}

	//sp�janie
	if (left && right) {

		leftFooter = memorySize - 1;        //prv� p�ta na �avo od pam�ti na uvolnenie
		rightHeader = (int*)memoryPtrEnd + 1;       //prv� hlavi�ka na pravo od pam�ti na uvolnenie

		//kontrola �i je blok pam�te na pravo voln� 
		if (*rightHeader > 0) {
			//spojenie s blokom na pravo
			rightFooter = (char*)rightHeader + *rightHeader + sizeof(int);      //p�ta bloku na pravo
			*memorySize = (*memorySize * (-1)) + *rightHeader + 2 * sizeof(int);      //hlavi�ka uvolnovan�ho bloku (nov� velkost)
			*((int*)rightFooter) = *memorySize;     //p�ta bloku na pravo (nov� velkost)

			memset(memorySize + 1, 0, *memorySize);		//memset pre lep�iu vizualiz�ciu

		}
		else {
			*memorySize = (*memorySize * (-1));       //hlavi�ka, zmena na kladn�

			memset(valid_ptr, 0, *memorySize);		//memset pre lep�iu vizualiz�ciu

			*((int*)memoryPtrEnd) = (*((int*)memoryPtrEnd) * (-1));       //p�ta, zmena na kladn�
		}


		//kontrola �i je blok pam�te na �avo voln�
		if (*leftFooter > 0) {
			//spojenie s blokom na lavo
			leftHeader = (char*)leftFooter - *leftFooter - sizeof(int);     //hlavi�ka bloku na �avo
			//kontrola prehodene velkost na kladnu v pripade spojenia s pravym
			if (*memorySize < 0) {
				*((int*)leftHeader) = (*memorySize * (-1)) + *leftFooter + 2 * sizeof(int);       //hlavi�ka bloku na �avo (nova velkos�)
				memoryPtrEnd = (char*)valid_ptr - (*memorySize);		//p�ta uvolnovan�ho bloku
				*((int*)memoryPtrEnd) = (*memorySize * (-1)) + *leftFooter + 2 * sizeof(int);		//p�ta uvolnovan�ho bloku (nova velkos�)
			}
			else {
				*((int*)leftHeader) = *memorySize + *leftFooter + 2 * sizeof(int);		//hlavi�ka bloku na �avo (nova velkos�)
				memoryPtrEnd = (char*)valid_ptr + *memorySize;		//p�ta uvolnovan�ho bloku
				*((int*)memoryPtrEnd) = *memorySize + *leftFooter + 2 * sizeof(int);		//p�ta uvolnovan�ho bloku (nova velkos�)
			}

			memset(leftHeader + 4, 0, *((int*)leftHeader));		//memset pre lep�iu vizualiz�ciu
		}
		return 0;

	}
	else if (right) {

		rightHeader = (int*)memoryPtrEnd + 1;

		//kontrola �i je blok pam�te na pravo voln�
		if (*rightHeader > 0) {
			//spojenie s blokom na pravo
			rightFooter = (char*)rightHeader + *rightHeader + sizeof(int);      //p�ta bloku na pravo
			*memorySize = (*memorySize * (-1)) + *rightHeader + 2 * sizeof(int);      //hlavi�ka uvolnovan�ho bloku (nov� velkost)
			*((int*)rightFooter) = *memorySize;     //p�ta bloku na pravo (nov� velkost)

			memset(memorySize + 1, 0, *memorySize);		//memset pre lep�iu vizualiz�ciu

		}
		else {
			*memorySize = (*memorySize * (-1));       //hlavi�ka, zmena na kladn�

			memset(valid_ptr, 0, *memorySize);		//memset pre lep�iu vizualiz�ciu

			*((int*)memoryPtrEnd) = (*((int*)memoryPtrEnd) * (-1));       //p�ta, zmena na kladn�
		}

		return 0;

	}
	else if (left) {

		leftFooter = memorySize - 1;
		//kontrola �i je blok pam�te na �avo voln�
		if (*leftFooter > 0) {
			//spojenie s blokom na lavo
			leftHeader = (char*)leftFooter - *leftFooter - sizeof(int);		//hlavi�ka bloku na �avo
			*((int*)leftHeader) = (*memorySize * (-1)) + *leftFooter + 2 * sizeof(int);		//hlavi�ka bloku na �avo (nova velkos�)
			memoryPtrEnd = (char*)valid_ptr - (*memorySize);		//p�ta uvolnovan�ho bloku
			*((int*)memoryPtrEnd) = (*memorySize * (-1)) + *leftFooter + 2 * sizeof(int);		//p�ta uvolnovan�ho bloku (nova velkos�)

			memset(leftHeader + 4, 0, *((int*)leftHeader));		//memset pre lep�iu vizualiz�ciu

		}
		else {
			*memorySize = (*memorySize * (-1));       //hlavi�ka, zmena na kladn�

			memset(valid_ptr, 0, *memorySize);		//memset pre lep�iu vizualiz�ciu

			*((int*)memoryPtrEnd) = (*((int*)memoryPtrEnd) * (-1));       //p�ta, zmena na kladn�
		}

		return 0;

	}
	else {

		*memorySize = (*memorySize * (-1));       //hlavi�ka, zmena na kladn�

		memset(valid_ptr, 0, *memorySize);		//memset pre lep�iu vizualiz�ciu

		*((int*)memoryPtrEnd) = (*((int*)memoryPtrEnd) * (-1));       //p�ta, zmena na kladn�

		return 0;

	}

	return 1;
}





/*
	sl��i na skontrolovanie, �i parameter (ukazovate�) je platn� ukazovate�,
	ktor� bol v nejakom z predch�dzaj�cich volan� vr�ten� funkciou memory_alloc
	a zatia� nebol uvo�nen� funkciou memory_free.
	Funkcia vr�ti 0, ak je ukazovate� neplatn�, inak vr�ti 1.
*/
int memory_check(void* ptr) {

	//kontrola �i pointer nieje NULL
	if (memory == NULL || ptr == NULL) {
		return 0;
	}

	int* memoryBlockSize = (int*)memory + 1;    //ve�kos� celkovej pam�te pre alok�ciu

	void* memoryStart = (void*)((int*)memory + 2);      //za�iatok celkovej pam�te pre alok�ciu
	void* memoryEnd = (void*)((char*)memory + *((int*)memory) + 2 * sizeof(int));       
	void* memoryFooter = NULL;
	
	//kontrola �i je zo spravovanej pam�te
	if (ptr >= memoryStart && ptr < memoryEnd) {
		//cyklus na prechadzanie pam�te po allokovan�ch blokoch
		do {
			//kontrola zhody
			if (memoryStart == ptr) {
				//kontrola �i je blok alokovan�
				if (*((int*)ptr - 1) < 0) {
					return 1;       //je alokovan� (platn�)
				}
				else {
					return 0;       //nie je alokovan� (neplatn�)
				}
			}
			//posuvanie sa v pam�ti podla toho �i je blok alokovan� alebo nie
			if (*memoryBlockSize > 0) {
				memoryStart = (void*)((char*)memoryStart + *memoryBlockSize + 2 * sizeof(int));
				memoryBlockSize = (int*)memoryStart - 1;
			}
			else {
				memoryStart = (void*)((char*)memoryStart - (*memoryBlockSize) + 8);
				memoryBlockSize = (int*)memoryStart - 1;
			}

		} while ((void*)((char*)memoryStart - 2 * sizeof(int)) != memoryEnd);

		return 0;       //nena�la sa zhoda (neplatn�)

	}
	else {

		return 0;       //nie je z pam�te (neplatn�)

	}

}





//test na alokovanie rovnak�ch blokov
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

	//testovanie pridelovania rovnak�ch 8-24B bloky
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

		//nerovnake bloky 8-24B do 200B pam�te
		/*
		test2(region, 25, 8, 24);
		test2(region, 50, 8, 24);
		test2(region, 75, 8, 24);
		test2(region, 100, 8, 24);
		test2(region, 150, 8, 24);
		test2(region, 200, 8, 24);
		printf("\n");
		*/

		//nerovnake bloky 500-5000B do 100000B pam�te
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

		//nerovnake bloky 8-50000B do 100000B pam�te
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

	//test spajania najpr s �av�m blokom potom s prav�m blokom
	/*
	memory_free(pointer1);		
	memory_free(pointer2);
	memory_free(pointer3);
	*/


	//test spajania najpr s prav�m blokom potom s �av�m blokom
	/*
	memory_free(pointer3);
	memory_free(pointer2);
	memory_free(pointer1);
	*/


	//test spajania najpr s prav�m a �av�m blokom
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