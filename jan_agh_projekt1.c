
#include <stdio.h>										//____ZPP2 - Projekt1 - Akad. rok 2020/2021____
#include <stdlib.h>												//Jan Agh, FIIT STUBA
#include <string.h>									//Copyright(c) 2021 Jan Agh, all rights reserved
#include <math.h>

#define GOODS "tovar.txt"
#define SUPPL "dodavatelia.txt"					//!!!!!!!DOLEZITE!!!!!!! <<<<< SEM ZADAT MENA SUBOROV

void v(FILE**, FILE**, int*, int*, int*);
void o(FILE*, FILE*, int*, int*);
void n(FILE*, FILE*, char***, char***, char***, int**, int**, float**, float**, int*, int*);
void s(char**, int*, int*, int*);
void h(int*, int*);
void p(FILE*, char**, int*, float*, float*, int*, int*, int*);
void z(char**, float*, int*);

int getNumOfLines(FILE*, int);						//zoznam pomocnych funkcii
void deallocStrings(char***, int);
void allocStrings(char*** , int);
void deallocAll(char***, char***, char***, int**, int**, float**, float**, int*, int*);
void sortArray(int*, float*, int);
void closeFiles(FILE**, FILE**);

int main() {

	char command;
	int goods_lines = 0, suppl_lines = 0, check = 1;

	char **items = NULL, ** companies = NULL, ** addresses = NULL;
	FILE* goods = NULL, * suppliers = NULL;
	int *storage = NULL, *id = NULL;
	float* prices = NULL, * weights = NULL;

	do {
		printf("ZADAJTE NAZOV PRIKAZU: ");			//nacitanie znaku prikazu
		scanf(" %c", &command);
		putchar('\n');

		switch (command){
			case 'v':
				v(&goods, &suppliers, &goods_lines, &suppl_lines, &check); break;
			case 'o':
				o(goods, suppliers, &goods_lines, &suppl_lines); break;
			case 'n':
				n(goods, suppliers, &items, &companies, &addresses, &storage,
					&id, &prices, &weights, &goods_lines, &suppl_lines); break;
			case 's':
				s(items, id, storage, &goods_lines); break;
			case 'h':
				h(storage, &goods_lines); break;
			case 'p':
				p(goods, items, storage, prices, weights, id, &goods_lines, &check); break;
			case 'z':
				z(items, weights, &goods_lines); break;
		}
	} while (command != 'k' && check);
													//prikaz 'k' zrealizovany pomocou dvoch pomocnych funkcii nizsie
	closeFiles(&goods, &suppliers);
	deallocAll(&items, &companies, &addresses, &storage, &id, &prices, &weights, &goods_lines, &suppl_lines);

	return 0;
}

void v(FILE** goods, FILE** suppliers, int* goods_lines, int* suppl_lines, int* check) {

	char buffer1[51] = "\0", buffer2[51] = "\0";
	int amount, id, c;
	float price, mass;

	closeFiles(goods, suppliers);			//zatvorenie suborov, ak uz boli predtym otvorene

	if ((*goods = fopen(GOODS, "r+")) != NULL && (*suppliers = fopen(SUPPL, "r")) != NULL) {

		*goods_lines = getNumOfLines(*goods, 6);			//zistenie poctu zaznamov v suboroch
		*suppl_lines = getNumOfLines(*suppliers, 4);
		
		for (c = 0; c < *goods_lines; c++) {			//nacitanie a vypis zaznamov z tovar.txt
			fscanf(*goods, "%50[^\n]\n%d\n%f\n%f\n%d\n\n", buffer1, &amount, &price, &mass, &id);
			printf("Nazov tovaru: %s\nPocet kusov na sklade: %d\nCena: %.2f\nHmotnost: %.4f\nID dodavatela: %d\n\n",
				buffer1, amount, price, mass, id);		
		}
		printf("---------------------------------------------------------------\n");
		printf("---------------------------------------------------------------\n\n");

		for (c = 0; c < *suppl_lines; c++) {		//nacitanie a vypis zaznamov z dodavatelia.txt
			fscanf(*suppliers, "%d\n%50[^\n]\n%50[^\n]\n\n", &id, buffer1, buffer2);
			printf("ID dodavatela: %d\nMeno dodavatela: %s\nAdresa dodavatela: %s\n\n", id, buffer1, buffer2);
		}
		rewind(*goods), rewind(*suppliers);		//navrat kurzorov na zaciatok suborov
	}
	else {
		printf("Subory sa nepodarilo otvorit.\n");
		*check = 0;
	}
}

void o(FILE* goods, FILE* suppliers, int* goods_lines, int* suppl_lines) {

	float price = 0, price1;
	int id, search, counter = 0, x;
	char name[51] = "\0", buffer[51] = "\0";

	printf("Zadajte ID dodavatela: ");
	scanf("%d", &id);

	if (goods != NULL && suppliers != NULL) {
		for (x = 0; x < *suppl_lines; x++) {			//hladanie ID v dodavatelia.txt
			fscanf(suppliers, "%d\n%*[^\n]\n%*[^\n]\n\n", &search);
			if (search == id) {
				counter++; break;
			}
		}
		if (counter) {			//ak dane ID existuje, vykona sa vyhladanie najdrahsieho tovaru
			for (x = 0; x < *goods_lines; x++) {
				fscanf(goods, "%50[^\n]\n%*[^\n]\n%f\n%*[^\n]\n%d\n", buffer, &price1, &search);
				if (search == id) {
					price = (price1 > price ? price1 : price);
					strcpy(name, buffer);			
				}
			}
			printf("Najdrahsi tovar je: %s.\n", name);
		}
		rewind(suppliers), rewind(goods);	
	}
	else {
		printf("Subory sa nepodarilo otvorit alebo prikaz 'v' nebol este aktivovany.\n");
	}
	putchar('\n');
}

void n(FILE* goods, FILE* suppliers, char*** items, char*** companies, char*** addresses,
	int** storage, int** id, float** prices, float** weights, int* goods_lines, int* suppl_lines) {

	int x;
	
	if (goods != NULL && suppliers != NULL) {			//dealokovanie poli, ak uz boli vytvorene 
		deallocAll(items, companies, addresses, storage, id, prices, weights, goods_lines, suppl_lines);

		*suppl_lines = *goods_lines = 0;			//spocitanie zaznamov v suboroch
		*goods_lines = getNumOfLines(goods, 6);
		*suppl_lines = getNumOfLines(suppliers, 4);
		allocStrings(items, *goods_lines);				//alokovanie pamate pre polia retazcov
		allocStrings(companies, *suppl_lines);
		allocStrings(addresses, *suppl_lines);
		*storage = (int*)malloc(*goods_lines * sizeof(int));		//alokovanie pamate pre ciselne hodnoty
		*id = (int*)malloc(*goods_lines * sizeof(int));
		*prices = (float*)malloc(*goods_lines * sizeof(float));
		*weights = (float*)malloc(*goods_lines * sizeof(float));

		for (x = 0; x < *goods_lines; x++) {		//nacitanie zaznamov z tovar.txt a ich ulozenie do poli
			fscanf(goods, "%50[^\n]\n%d\n%f\n%f\n%d\n", (*items)[x], &(*storage)[x], &(*prices)[x], &(*weights)[x], &(*id)[x]);
		}

		for (x = 0; x < *suppl_lines; x++) {		//nacitanie zaznamov z dodavatelia.txt a ich ulozenie do poli
			fscanf(suppliers, "%*[^\n]\n%50[^\n]\n%50[^\n]\n\n", (*companies)[x], (*addresses)[x]);
		}
		rewind(goods), rewind(suppliers);
	}
}

void s(char** items, int* id, int* storage, int* goods_lines) {

	int search, index;

	printf("Zadajte ID dodavatela: ");
	scanf("%d", &search);

	if (items != NULL && id != NULL && storage != NULL) {
		for (index = 0; index < *goods_lines; index++) {		//najdenie tovarov dodavanych dodavatelom podla jeho ID
			if (id[index] == search) {
				printf("%s (%dks na sklade)\n", items[index], storage[index]);
			}
		}
	}
	else {
		printf("Polia nie su vytvorene.\n");
	}
	putchar('\n');
}

void h(int* storage, int* goods_lines) {
	
	int histogram[10] = {0}, x, size = (int)sizeof(histogram) / sizeof(int);

	if (storage != NULL) {
		for (x = 0; x < size; x++) {
			for (int y = 0; y < *goods_lines; y++) {		//spocitanie tovarov patriacich do danej kategorie
				if (storage[y] >= (x * 10) && storage[y] <= (x * 10) + 9) {
					histogram[x]++;
				}
			}
			printf("<%d,%d>: %d\n", x * 10, (x * 10) + 9, histogram[x]);
		}
	}
	else {
		printf("Polia nie su vytvorene.\n");
	}
	putchar('\n');
}

void p(FILE* goods, char** items, int* storage, float* prices, float* weights, int* id, int* goods_lines, int* check) {

	char item[51] = "\0";
	float num;
	int counter = 0, x;

	printf("Zadajte meno tovaru: ");
	scanf("%s", item);
	printf("Zadajte pocet kusov tovaru na sklade: ");
	scanf("%f", &num);
	num = ((num >= 0 && num <= 99) ? num : 0);	  //ak pouzivatel zada cislo mensie ako 0 alebo vacsie ako 99,
												  //do poli a suborov sa ulozi 0
	if (items != NULL && storage != NULL) {
		if (goods != NULL) {
			for (x = 0; x < *goods_lines; x++) {	//test, ci dany tovar vobec existuje v subore
				if (!strcmp(item, items[x])) {
					storage[x] = (int)num, counter++; break;
				}
			}
			if (counter) {			//ak sa dany tovar nasiel, vykona sa zapis do suboru tovar.txt
				for (x = 0; x < *goods_lines; x++) {
					fprintf(goods, "%s\n%d\n%.2f\n%.4f\n%d", items[x], storage[x], prices[x], weights[x], id[x]);

					if (x != *goods_lines - 1) {	//uprava, aby sa po poslednom zazname nevkladal novy riadok
						fprintf(goods, "\n\n");
					}
				}
				rewind(goods);
				printf("Aktualny pocet kusov na sklade tovaru %s je: %d.\n", item, (int)num);
			}
		}
		else {
			printf("Neotvoreny subor.\n");
			*check = 0;
		}
	}
	else {
		printf("Polia nie su vytvorene.\n");
	}
	putchar('\n');
}

void z(char** items, float* weights, int* goods_lines) {

	float min, max, *res = (float*) malloc(*goods_lines * sizeof(float));
	int* ind = (int*)malloc(*goods_lines * sizeof(int)), count = 0, x;

	printf("Zadajte dve realne cisla oddelene medzerou: ");
	scanf("%f %f", &min, &max);

	if (items != NULL && weights != NULL) {
		if (min > max) {
			printf("Prve cislo musi byt mensie ako druhe cislo.\n");
		}
		else {
			for (x = 0; x < *goods_lines; x++) {
				if (weights[x] >= min && weights[x] <= max) {
					res[count] = weights[x], ind[count] = x;
					count++;
				}
			}
			sortArray(ind, res, count);			//triedenie poli

			for (x = 0; x < count; x++) {
				printf("%s\n", items[ind[x]]);
			}
		}
	}
	else {
		printf("Polia nie su vytvorene.\n");
	}
	free(res), free(ind);
	putchar('\n');
}

//****************************************POMOCNE FUNKCIE**************************************************

int getNumOfLines(FILE* f, int divider) {		//funkcia na zistenie poctu zaznamov v suboroch
	char x;
	int count = 0;						//divider = hodnota, kolko poloziek sa nachadza v jednom zazname

	while ((x = fgetc(f)) != EOF) {
		if (x == '\n') {
			count++;
		}
	}
	rewind(f);
	return (count + 2) / divider;
}

void deallocStrings(char*** dealloc, int size) {		//funkcia na dealokovanie poli retazcov
	for (int x = 0; x < size; x++) {
		free((*dealloc)[x]);
	}
	free(*dealloc);
	*dealloc = NULL;
}

void closeFiles(FILE** goods, FILE** suppliers) {		//funkcia na zatvorenie suborov
	if (*goods != NULL)
		fclose(*goods);
	if (*suppliers != NULL)
		fclose(*suppliers);
}

void deallocAll(char*** items, char*** companies, char*** addresses,
	int** storage, int** id, float** prices, float** weights, int* goods_lines, int* suppl_lines) {
	if (*items != NULL)
		deallocStrings(items, *goods_lines);		//funkcia na dealokovanie vsetkych poli
	if (*companies != NULL)
		deallocStrings(companies, *suppl_lines);
	if (*addresses != NULL)
		deallocStrings(addresses, *suppl_lines);
	if (*storage != NULL)
		free(*storage), *storage = NULL;
	if (*id != NULL)
		free(*id), *id = NULL;
	if (*prices != NULL)
		free(*prices), *prices = NULL;
	if (*weights != NULL)
		free(*weights), *weights = NULL;
}

void allocStrings(char*** alloc, int size) {			//funkcia na alokovanie poli retazcov
	(*alloc) = (char**)malloc(size * sizeof(char*));
	for (int x = 0; x < size; x++) {
		(*alloc)[x] = (char*)malloc(51 * sizeof(char));
	}
}

void sortArray(int* arr1, float* arr2, int count) {			//funkcia na utriedenie prvkov v poli
	for (int r = 0; r < count; r++) {
		for (int h = r + 1; h < count; h++) {
			if (arr2[r] > arr2[h]) {
				arr2[h] += arr2[r], arr2[r] = arr2[h] - arr2[r], arr2[h] -= arr2[r];
				arr1[h] += arr1[r], arr1[r] = arr1[h] - arr1[r], arr1[h] -= arr1[r];
			}
		}
	}
}