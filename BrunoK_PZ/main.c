#define _CRT_SECURE_NO_WARNINGS
#include "header.h"
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    setlocale(LC_ALL, "en_US.UTF-8");

    // Brišemo datoteku "rezultati.txt" na početku programa
    remove("rezultati.txt");

    Valuta* valute = malloc(MAX_VALUTE * sizeof(Valuta));
    if (valute == NULL) {
        perror("Neuspjelo zauzimanje memorije");
        return 1;
    }
    int brojValuta = 0;

    ucitajValuteIzDatoteke("valute.txt", valute, &brojValuta);

    // Sortiraj valute po abecednom redu
    qsort(valute, brojValuta, sizeof(Valuta), compareValute);

    int izborIzvora = -1, izborCilja = -1;
    double iznos;
    char imeValute[MAX_IME + 1];

    do {
        ispisiIzbornik(valute, brojValuta);

        printf("\n\nOdaberite izvornu valutu (0 za izlaz, -1 za pretrazivanje): ");
        if (scanf("%d", &izborIzvora) != 1 || izborIzvora < -1 || izborIzvora > brojValuta) {
            if (feof(stdin)) {
                printf("\n\nNema više dostupnih valuta.\n");
                break;
            }
            printf("\n\nNeispravan unos. Molimo unesite ponovno.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        if (izborIzvora == 0 || izborCilja == 0) {
            printf("Hvala na koristenju naseg konvertera valuta!\n"); 
            fflush(stdout);  
            break;
        }

        if (izborIzvora == -1) {
            printf("Unesite ime valute za pretrazivanje: ");
            while (getchar() != '\n'); // Clear input buffer
            if (fgets(imeValute, sizeof(imeValute), stdin) != NULL) {
                imeValute[strcspn(imeValute, "\n")] = '\0'; // Remove newline character from the end of the input
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

        printf("Odaberite ciljnu valutu (0 za izlaz): ");
        if (scanf("%d", &izborCilja) != 1 || izborCilja < 0 || izborCilja > brojValuta) {
            if (feof(stdin)) {
                printf("\n\nNema više dostupnih valuta.\n");
                break;
            }
            printf("\n\nNeispravan unos. Molimo unesite ponovno.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        if (izborCilja == 0) {
            printf("Hvala što ste koristili naš konverter valuta!\n");
            fflush(stdout); // Očisti buffer i odmah ispiši poruku
            break;
        }

        printf("Unesite iznos za konverziju: ");
        if (scanf("%lf", &iznos) != 1 || iznos <= 0) {
            printf("\n\nNeispravan unos. Molimo unesite ponovno.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        double iznosCilja = pretvoriIznos(iznos, valute[izborIzvora - 1].tecaj, valute[izborCilja - 1].tecaj);
        if (iznosCilja < 0) {
            continue;
        }

        printf("\n%.2f %s je ekvivalentno %.2f %s.\n", iznos, valute[izborIzvora - 1].skracenica, iznosCilja, valute[izborCilja - 1].skracenica);

        zapisiRezultatUDatoteku("rezultati.txt", iznos, valute[izborIzvora - 1].skracenica, iznosCilja, valute[izborCilja - 1].skracenica);

    } while (izborIzvora != 0 && izborCilja != 0);

    azurirajValute("valute.txt", valute, brojValuta);

    for (int i = 0; i < brojValuta; i++) {
        sigurnoOslobodi(valute[i].skracenica);
        sigurnoOslobodi(valute[i].ime);
    }
    free(valute);
    valute = NULL; // Osiguranje da se glavni pokazivač postavi na NULL

    return 0;
}
