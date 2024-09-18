#define _CRT_SECURE_NO_WARNINGS
#include "header.h"
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

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
            while (getchar() != '\n');
            continue;
        }

        if (izborIzvora == 0 || izborCilja == 0) {
            printf("Hvala na koristenju naseg konvertera valuta!\n");
            fflush(stdout);
            break;
        }

        if (izborIzvora == -1) {
            printf("Unesite ime valute za pretrazivanje: ");
            while (getchar() != '\n');
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

        printf("Odaberite ciljnu valutu (0 za izlaz): ");
        if (scanf("%d", &izborCilja) != 1 || izborCilja < 0 || izborCilja > brojValuta) {
            if (feof(stdin)) {
                printf("\n\nNema više dostupnih valuta.\n");
                break;
            }
            printf("\n\nNeispravan unos. Molimo unesite ponovno.\n");
            while (getchar() != '\n');
            continue;
        }

        if (izborCilja == 0) {
            printf("Hvala što ste koristili naš konverter valuta!\n");
            fflush(stdout);
            break;
        }

        printf("Unesite iznos za konverziju: ");
        if (scanf("%lf", &iznos) != 1 || iznos <= 0) {
            printf("\n\nNeispravan unos. Molimo unesite ponovno.\n");
            while (getchar() != '\n');
            continue;
        }

        double iznosCilja = pretvoriIznos(iznos, valute[izborIzvora - 1].tecaj, valute[izborCilja - 1].tecaj);
        if (iznosCilja < 0) {
            continue;
        }

        if (iznosCilja > 0) {
            printf("\n%.2f %s je ekvivalentno %.2f %s.\n", iznos, valute[izborIzvora - 1].skracenica, iznosCilja, valute[izborCilja - 1].skracenica);
            zapisiRezultatUDatoteku("rezultati.txt", iznos, valute[izborIzvora - 1].skracenica, iznosCilja, valute[izborCilja - 1].skracenica);
            brojKonverzija++;  // Povećaj broj uspješnih konverzija
        }

    } while (izborIzvora != 0 && izborCilja != 0);

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
