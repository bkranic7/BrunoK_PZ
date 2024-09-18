#pragma once
#ifndef HEADER_H
#define HEADER_H

extern int brojKonverzija;

#define MAX_VALUTE 100
#define MAX_IME 60
#define MAX_SKRACENICA 10

typedef struct {
    char* skracenica;
    char* ime;
    double tecaj;
} Valuta;

// Funkcije za rad sa valutama
void ispisiIzbornik(Valuta valute[], int brojValuta);

extern void ucitajValuteIzDatoteke(const char* datoteka, Valuta* valute, int* brojValuta);
extern double pretvoriIznos(double iznos, double tecajIzvora, double tecajCilja);

void zapisiRezultatUDatoteku(const char* datoteka, double iznos, const char* izvornaValuta, double iznosCilja, const char* ciljnaValuta);
void sigurnoOslobodi(char* p);
Valuta* pretraziValutu(Valuta valute[], int brojValuta, const char* ime);

// Funkcije za sortiranje i pretragu
void quickSort(Valuta valute[], int left, int right);
int partition(Valuta valute[], int left, int right);
void swapValuta(Valuta* a, Valuta* b);
Valuta* pretraziValutuRekurzivno(Valuta valute[], int low, int high, const char* ime);
int compareValute(const void* a, const void* b);
int compareValuteForSearch(const void* a, const void* b);

void azurirajValute(const char* datoteka, Valuta* valute, int brojValuta);

#endif // HEADER_H


