#define _CRT_SECURE_NO_WARNINGS
#include "header.h"
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int brojKonverzija = 0;  // Globalna varijabla za brojanje uspješnih konverzija

int main() {
    setlocale(LC_ALL, "en_US.UTF-8");

    remove("rezultati.txt");

    Valuta* valute = malloc(MAX_VALUTE * sizeof(Valuta));
    if (valute == NULL) {
        perror("Neuspjelo zauzimanje memorije");
        return 1;
    }
    int brojValuta = 0;

    ucitajValuteIzDatoteke("valute.txt", valute, &brojValuta);

    qsort(valute, brojValuta, sizeof(Valuta), compareValute);

    char izborIzvoraStr[100], izborCiljaStr[100];
    int izborIzvora = -1, izborCilja = -1;
    double iznos;
    char imeValute[MAX_IME + 1];

    do {
        ispisiIzbornik(valute, brojValuta);

        printf("\n\nOdaberite izvornu valutu (0 za izlaz, 's' za pretrazivanje): ");
        if (fgets(izborIzvoraStr, sizeof(izborIzvoraStr), stdin) == NULL) {
            printf("\n\nPogreška prilikom čitanja unosa.\n");
            continue;
        }
        izborIzvoraStr[strcspn(izborIzvoraStr, "\n")] = '\0'; // Ukloni novi red

        if (strcmp(izborIzvoraStr, "0") == 0) {
            printf("Hvala na koristenju naseg konvertera valuta!\n");
            fflush(stdout);
            break;
        }
        else if (strcmp(izborIzvoraStr, "s") == 0 || strcmp(izborIzvoraStr, "S") == 0) {
            printf("Unesite ime valute za pretrazivanje: ");
            if (fgets(imeValute, sizeof(imeValute), stdin) != NULL) {
                imeValute[strcspn(imeValute, "\n")] = '\0';
                Valuta* rezultat = pretraziValutu(valute, brojValuta, imeValute);
                if (rezultat != NULL) {
                    printf("Pronadena valuta: %s (%s) - Tecaj: %.2f\n", rezultat->ime, rezultat->skracenica, rezultat->tecaj);
                }
                else {
                    printf("Valuta s imenom '%s' nije pronadena.\n", imeValute);
                }
            }
            continue;
        }
        else if (isNumber(izborIzvoraStr)) {
            izborIzvora = atoi(izborIzvoraStr);
            if (izborIzvora < 1 || izborIzvora > brojValuta) {
                printf("\n\nNeispravan unos. Molimo unesite broj izmedu 1 i %d.\n", brojValuta);
                continue;
            }
        }
        else {
            printf("\n\nNeispravan unos. Molimo unesite broj ili 's' za pretrazivanje.\n");
            continue;
        }

        // Odabir ciljne valute
        printf("Odaberite ciljnu valutu (0 za izlaz): ");
        if (fgets(izborCiljaStr, sizeof(izborCiljaStr), stdin) == NULL) {
            printf("\n\nPogreška prilikom čitanja unosa.\n");
            continue;
        }
        izborCiljaStr[strcspn(izborCiljaStr, "\n")] = '\0'; // Ukloni novi red

        if (strcmp(izborCiljaStr, "0") == 0) {
            printf("Hvala što ste koristili naš konverter valuta!\n");
            fflush(stdout);
            break;
        }
        else if (isNumber(izborCiljaStr)) {
            izborCilja = atoi(izborCiljaStr);
            if (izborCilja < 1 || izborCilja > brojValuta) {
                printf("\n\nNeispravan unos. Molimo unesite broj između 1 i %d.\n", brojValuta);
                continue;
            }
        }
        else {
            printf("\n\nNeispravan unos. Molimo unesite broj.\n");
            continue;
        }

        printf("Unesite iznos za konverziju: ");
        if (scanf("%lf", &iznos) != 1 || iznos <= 0) {
            printf("\n\nNeispravan unos. Molimo unesite pozitivni broj.\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n'); // Očisti ulazni spremnik

        double iznosCilja = pretvoriIznos(iznos, valute[izborIzvora - 1].tecaj, valute[izborCilja - 1].tecaj);
        if (iznosCilja < 0) {
            continue;
        }

        printf("\n%.2f %s je ekvivalentno %.2f %s.\n", iznos, valute[izborIzvora - 1].skracenica, iznosCilja, valute[izborCilja - 1].skracenica);
        zapisiRezultatUDatoteku("rezultati.txt", iznos, valute[izborIzvora - 1].skracenica, iznosCilja, valute[izborCilja - 1].skracenica);
        brojKonverzija++;  // Povećaj broj uspješnih konverzija

    } while (1);

    printf("\nUkupan broj uspjesnih konverzija: %d\n", brojKonverzija);

    azurirajValute("valute.txt", valute, brojValuta);

    long velicinaDatoteke = dohvatiVelicinuDatoteke("valute.txt");
    if (velicinaDatoteke != -1) {
        printf("Velicina datoteke 'valute.txt' je: %ld bajtova\n", velicinaDatoteke);
    }

    for (int i = 0; i < brojValuta; i++) {
        sigurnoOslobodi(valute[i].skracenica);
        sigurnoOslobodi(valute[i].ime);
    }
    free(valute);
    valute = NULL;

    return 0;
}
