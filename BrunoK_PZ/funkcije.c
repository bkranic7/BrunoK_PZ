#define _CRT_SECURE_NO_WARNINGS
#include "header.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Local functions with static keyword
static inline void swapValuta(Valuta* a, Valuta* b) {
    Valuta temp = *a;
    *a = *b;
    *b = temp;
}

int compareValute(const void* a, const void* b) {
    Valuta* valutaA = (Valuta*)a;
    Valuta* valutaB = (Valuta*)b;
    return strcmp(valutaA->ime, valutaB->ime);
}


static int compareValuteForSearch(const void* a, const void* b) {
    const char* ime = (const char*)a;
    const Valuta* valuta = (const Valuta*)b;
    return strcmp(ime, valuta->ime);
}

// Function implementations
void ispisiIzbornik(Valuta valute[], int brojValuta) {
    printf("\n+---------------------------------------------------------------------+");
    printf("\n|                     KONVERTER VALUTA:                               |");
    printf("\n+---------------------------------------------------------------------+");
    printf("\n| %-4s %-60s |", "REDNI", "VALUTA");
    printf("\n+---------------------------------------------------------------------+");
    for (int i = 0; i < brojValuta; i++) {
        int len = strlen(valute[i].ime);
        int spaces = 60 - len;
        printf("\n| %-4d %-s%-*s |", i + 1, valute[i].ime, spaces, "");
    }
    printf("\n+---------------------------------------------------------------------+");
    printf("\n| %-4s %-60s |", "0.", "IZLAZ");
    printf("\n+---------------------------------------------------------------------+");
}

void ucitajValuteIzDatoteke(const char* datoteka, Valuta* valute, int* brojValuta) {
    FILE* dat = fopen(datoteka, "r");
    if (dat == NULL) {
        perror("Nije moguće otvoriti datoteku");
        return;
    }

    *brojValuta = 0;
    char linija[256];
    while ((*brojValuta < MAX_VALUTE) && fgets(linija, sizeof(linija), dat) != NULL) {
        char skracenica[MAX_SKRACENICA + 1];
        double tecaj;
        char ime[MAX_IME + 1];

        if (sscanf(linija, "%s %lf %[^\n]", skracenica, &tecaj, ime) == 3) {
            valute[*brojValuta].skracenica = _strdup(skracenica);
            valute[*brojValuta].ime = _strdup(ime);
            if (valute[*brojValuta].skracenica == NULL || valute[*brojValuta].ime == NULL) {
                printf("Greška: Neuspjelo zauzimanje memorije.\n");
                fclose(dat);
                return;
            }
            valute[*brojValuta].tecaj = tecaj;
            (*brojValuta)++;
        }
    }

    if (ferror(dat)) {
        perror("Pogreška prilikom čitanja datoteke");
    }

    fclose(dat);
    qsort(valute, *brojValuta, sizeof(Valuta), compareValute);
}

void quickSort(Valuta valute[], int left, int right) {
    if (left < right) {
        int pivotIndex = partition(valute, left, right);
        quickSort(valute, left, pivotIndex - 1);
        quickSort(valute, pivotIndex + 1, right);
    }
}

int partition(Valuta valute[], int left, int right) {
    Valuta pivot = valute[right];
    int i = left - 1;

    for (int j = left; j < right; j++) {
        if (strcmp(valute[j].ime, pivot.ime) < 0) {
            i++;
            swapValuta(&valute[i], &valute[j]);
        }
    }

    swapValuta(&valute[i + 1], &valute[right]);
    return i + 1;
}

Valuta* pretraziValutuRekurzivno(Valuta valute[], int low, int high, const char* ime) {
    if (low <= high) {
        int mid = low + (high - low) / 2;

        int cmp = strcmp(valute[mid].ime, ime);
        if (cmp == 0)
            return &valute[mid];
        else if (cmp < 0)
            return pretraziValutuRekurzivno(valute, mid + 1, high, ime);
        else
            return pretraziValutuRekurzivno(valute, low, mid - 1, ime);
    }
    return NULL;
}

void zapisiRezultatUDatoteku(const char* datoteka, double iznos, const char* izvornaValuta, double iznosCilja, const char* ciljnaValuta) {
    FILE* dat = fopen(datoteka, "a");
    if (dat == NULL) {
        printf("Nije moguće otvoriti datoteku za pisanje.\n");
        return;
    }

    fprintf(dat, "%.2f %s je ekvivalentno %.2f %s.\n", iznos, izvornaValuta, iznosCilja, ciljnaValuta);
    fclose(dat);
}


void azurirajValute(const char* datoteka, Valuta* valute, int brojValuta) {
    FILE* dat = fopen(datoteka, "w");
    if (dat == NULL) {
        perror("Nije moguće otvoriti datoteku za pisanje");
        return;
    }

    for (int i = 0; i < brojValuta; i++) {
        fprintf(dat, "%s %lf %s\n", valute[i].skracenica, valute[i].tecaj, valute[i].ime);
    }

    if (ferror(dat)) {
        perror("Pogreška prilikom pisanja u datoteku");
    }

    fclose(dat);
}

void kopirajDatoteku(const char* izvornaDatoteka, const char* ciljnaDatoteka) {
    FILE* izvor = fopen(izvornaDatoteka, "r");
    if (izvor == NULL) {
        perror("Nije moguće otvoriti izvornu datoteku");
        return;
    }

    FILE* cilj = fopen(ciljnaDatoteka, "w");
    if (cilj == NULL) {
        perror("Nije moguće otvoriti ciljnu datoteku");
        fclose(izvor);
        return;
    }

    char buffer[1024];
    size_t bytes;

    while ((bytes = fread(buffer, 1, sizeof(buffer), izvor)) > 0) {
        if (fwrite(buffer, 1, bytes, cilj) != bytes) {
            perror("Pogreška prilikom pisanja u ciljnu datoteku");
            break;
        }
    }

    if (ferror(izvor)) {
        perror("Pogreška prilikom čitanja iz izvorne datoteke");
    }

    fclose(izvor);
    fclose(cilj);
}


void sigurnoOslobodi(char* p) {
    if (p != NULL) {
        free(p);
        p = NULL; 
    }
}

Valuta* pretraziValutu(Valuta valute[], int brojValuta, const char* ime) {
    return (Valuta*)bsearch(ime, valute, brojValuta, sizeof(Valuta), compareValuteForSearch);
}

double pretvoriIznos(double iznos, double tecajIzvora, double tecajCilja) {
    return iznos * (tecajCilja / tecajIzvora);
}
