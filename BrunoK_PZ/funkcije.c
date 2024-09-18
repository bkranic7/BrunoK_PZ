#define _CRT_SECURE_NO_WARNINGS
#include "header.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



inline void swapValuta(Valuta* v1, Valuta* v2) {
    if (v1 == NULL || v2 == NULL) {
        printf("Pogreška: NULL pokazivač u swapValuta funkciji.\n");
        return;
    }
    Valuta temp = *v1;
    *v1 = *v2;
    *v2 = temp;
}

inline int compareValute(const void* a, const void* b) {
    Valuta* valutaA = (Valuta*)a;
    Valuta* valutaB = (Valuta*)b;
    return strcmp(valutaA->ime, valutaB->ime);
}


static int compareValuteForSearch(const void* a, const void* b) {
    const char* ime = (const char*)a;
    const Valuta* valuta = (const Valuta*)b;
    return strcmp(ime, valuta->ime);
}

void ispisiIzbornik(Valuta valute[], int brojValuta) {
    printf("\n+---------------------------------------------------------------------+");
    printf("\n|                     KONVERTER VALUTA:                 |");
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

inline int partition(Valuta valute[], int left, int right) {
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

Valuta* pretraziValutuRekurzivno(Valuta* valute, int low, int high, const char* ime) {
    if (valute == NULL || ime == NULL) {
        printf("Pogreška: NULL pokazivač u pretraziValutuRekurzivno funkciji.\n");
        return NULL;
    }
    if (low > high) return NULL;

    int mid = (low + high) / 2;
    int cmp = strcmp(valute[mid].ime, ime);

    if (cmp == 0) {
        return &valute[mid];
    }
    else if (cmp < 0) {
        return pretraziValutuRekurzivno(valute, mid + 1, high, ime);
    }
    else {
        return pretraziValutuRekurzivno(valute, low, mid - 1, ime);
    }
    return NULL;
}


void zapisiRezultatUDatoteku(const char* datoteka, double iznos, const char* izvornaValuta, double iznosCilja, const char* ciljnaValuta) {
    if (datoteka == NULL || izvornaValuta == NULL || ciljnaValuta == NULL) {
        printf("Greška: Neispravan pokazivač.\n");
        return;
    }

    FILE* dat = fopen(datoteka, "a");
    if (dat == NULL) {
        printf("Nije moguće otvoriti datoteku za pisanje.\n");
        return;
    }

    fprintf(dat, "%.2f %s je ekvivalentno %.2f %s.\n", iznos, izvornaValuta, iznosCilja, ciljnaValuta);
    fclose(dat);
}

void azurirajValute(const char* datoteka, Valuta* valute, int brojValuta) {
    if (datoteka == NULL || valute == NULL) {
        printf("Greška: Neispravan pokazivač.\n");
        return;
    }

    FILE* dat = fopen(datoteka, "w");
    if (dat == NULL) {
        perror("Nije moguće otvoriti datoteku za pisanje");
        return;
    }

    for (int i = 0; i < brojValuta; i++) {
        if (valute[i].skracenica == NULL || valute[i].ime == NULL) {
            printf("Greška: Neispravan pokazivač za valutu.\n");
            continue;
        }
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

// Funkcija za dobivanje veličine datoteke
long dohvatiVelicinuDatoteke(const char* datoteka) {
    FILE* dat = fopen(datoteka, "r");
    if (dat == NULL) {
        perror("Nije moguće otvoriti datoteku za dobivanje veličine");
        return -1;
    }

    // Korištenje fseek i ftell za dobivanje veličine
    if (fseek(dat, 0, SEEK_END) != 0) {
        perror("Pogreška prilikom pozicioniranja na kraj datoteke");
        fclose(dat);
        return -1;
    }

    long velicina = ftell(dat); // Vraća trenutnu poziciju (veličinu datoteke)
    if (velicina == -1L) {
        perror("Pogreška prilikom dobivanja veličine datoteke");
        fclose(dat);
        return -1;
    }

    rewind(dat); // Korištenje rewind da se vrati na početak datoteke
    fclose(dat);
    return velicina;
}

Valuta* pretraziValutu(Valuta* valute, int brojValuta, const char* ime) {
    if (valute == NULL || ime == NULL) {
        printf("Pogreška: NULL pokazivač u pretraziValutu funkciji.\n");
        return NULL;
    }
    for (int i = 0; i < brojValuta; i++) {
        if (strcmp(valute[i].ime, ime) == 0) {
            return &valute[i];
        }
    }
    return NULL;
}

double pretvoriIznos(double iznos, double tecajIzvora, double tecajCilja) {
    return iznos * (tecajCilja / tecajIzvora);
}


