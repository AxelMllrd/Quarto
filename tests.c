#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#include "tests.h"
#include "quarto.h"
#include "precalcul.h"
#include "canonique.h"
#include "table.h"
#include "solutions.h"
#include "affichage.h"
#include "fichiers.h"

#define FOREGROUND_WHITE (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define printerr(...) do {set_color(FOREGROUND_RED); printf(__VA_ARGS__); set_color(FOREGROUND_WHITE); } while (0)

void set_color(WORD color) {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(console, color);
}

static struct timespec start;

void start_timer() {
    clock_gettime(CLOCK_REALTIME, &start);
}

double get_time() {
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);

    return (double)(time.tv_sec - start.tv_sec) + (double)(time.tv_nsec - start.tv_nsec) / 1000000000.0;
}

Plateau rand_plateau() {
    Bitboard placements = rand();
    return (Plateau) {
        rand() & placements,
        rand() & placements,
        rand() & placements,
        rand() & placements,
        placements
    };
}

#define n 10000
#define m 1000
void test_hachage() {
    Precalcul *precalcul = precaluler();
    if (precalcul == NULL) {
        printf("\x1b[31m" "Echec des precalculs.\nTest non valide\n");
        return;
    }

    Plateau plat[n];
    int recurrence[n] = {0}, moyenne[n] = {0};
    int max = 0;

    for (float j = 0; j < m; j+=1.0f) {
        for (int i = 0; i < n; i++) {
            plat[i] = rand_plateau();
            canonique(&plat[i], precalcul);

            int v = (int)(hacher(&plat[i]) % n);
            if (++recurrence[v] > max) max = recurrence[v];
        }

        for (int i = 0; i < n; i++) {
            if (recurrence[i] > max || recurrence[i] >= 100) printf("\n MAX : %d, RECUR : %d\n", max, recurrence[i]);
            else moyenne[recurrence[i]]++;
            recurrence[i] = 0;
        }
    }

    printf("Distribution de la redondance du hachage : \n");
    for (int i = 0; i < max; i++) {
        printf("%d : %06.3f%%\n", i, 100.0f * (float)moyenne[i] / (float)(n * m));
    }

    detruire_precalcul(precalcul);
}
#undef n
#undef m

void test_table() {
    Precalcul *precalcul = precaluler();
    if (precalcul == NULL) {
        printerr("Erreur des precalculs\n");
        return;
    }

    Table *table = creer_table(3 * 100000);
    if (table == NULL) {
        printerr("Erreur lors de la creation de la table\n");
        detruire_precalcul(precalcul);
        return;
    }

    for (int i = 0; i < 100000; i++) {
        Plateau plateau = rand_plateau();
        canonique(&plateau, precalcul);
        Noeud *n1 = ajouter_plateau(table, &plateau);
        if (n1 == NULL) {
            printerr("Echec dans l'ajout du plateau %d\n", i);
        }
        else {
            Noeud *n2 = ajouter_plateau(table, &n1->plateau);
            if (n2 != n1) printf("Plateau non retrouve\n");
        }
    }

    printf("Nb elements : %d\n", table->nb_noeuds);

    detruire_table(table);
}

void test_solution_coup(int coup, int indice) {
    Precalcul *precalcul = precaluler();
    if (precalcul == NULL) {
        printerr("Erreur des precalculs\n");
        return;
    }

    printf("Calculs des solutions...\n");
    double t = get_time();
    int err = solution_coup(precalcul, coup, indice);
    if (err) {
        printerr("Erreur (%d) de calcul au coup %d\n", err, coup);
        return;
    }
    printf("Solution coup %d calculee en %f s\n", coup, get_time() - t);
    int nb_fichiers = lire_sommaire(coup);
    if (nb_fichiers < 1) {
        printerr("Erreur (%d) lors de la lecture du sommaire\n", nb_fichiers);
        return;
    }
    printf("Fusion des %d fichiers temporaires...\n", nb_fichiers);
    if ((err = (int)fusionner(coup + 1, nb_fichiers))) {
        printerr("Erreur (%d) lors de la fusion des fichiers\n", err);
        return;
    }

    printf("Separation du fichier...\n");
    if ((err = (int)separer_fichier(coup + 1))) {
        printerr("Erreur (%d) lors de la separation du fichier\n", err);
        return;
    }

    printf("\n");
}

void test_trier() {
    Liste *liste_avant = charger_fichier("solutions/coup_2.bin");
    if (liste_avant == NULL) {
        printerr("Erreur lors du chargement de la liste avant le tri\n");
        return;
    }

    int err = trier_fichier("solutions/coup_2.bin");
    if (err) {
        printerr("Erreur (%d) lors du tri\n", err);
        return;
    }

    Liste *liste_apres = charger_fichier("solutions/coup_2.bin");
    if (liste_apres == NULL) {
        printerr("Erreur lors du chargement de la liste apres le tri\n");
        return;
    }

    for (int i = 0; i < liste_apres->nb_noeuds; i++) {
        printf("Plateau %d :\nAvant : \n", i);
        aff_plateau(&liste_avant->noeuds[i].plateau);
        printf("Apres : \n");
        aff_plateau(&liste_apres->noeuds[i].plateau);
        printf("\n");
    }

    detruire_liste(liste_avant);
    detruire_liste(liste_apres);
}