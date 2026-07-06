#include <stdio.h>
#include <stdlib.h>

#include "quarto.h"
#include "fichiers.h"
#include "table.h"

Liste *charger_fichier(const char *nom_fichier) {
    FILE *fichier = fopen(nom_fichier, "rb");
    if (fichier == NULL) return NULL;

    char entete[6];
    fread(entete, sizeof(char), sizeof(entete), fichier);
    if (entete[0] != 'Q' || entete[1] != 'U' || entete[2] != 'A' ||
        entete[3] != 'R' || entete[4] != 'T' || entete[5] != 'O')
        goto erreur_1;

    int nb_noeuds;
    fread(&nb_noeuds, sizeof(int), 1, fichier);
    if (nb_noeuds < 1) goto erreur_1;

    Liste *liste = malloc(sizeof(*liste));
    if (liste == NULL) goto erreur_1;

    Noeud *noeuds = malloc(nb_noeuds * sizeof(*noeuds));
    if (noeuds == NULL) goto erreur_2;

    size_t n = fread(noeuds, sizeof(*noeuds), nb_noeuds, fichier);
    if (n != nb_noeuds) goto erreur_3;

    liste->noeuds = noeuds;
    liste->nb_noeuds = nb_noeuds;
    return liste;

    erreur_3:
    free(noeuds);

    erreur_2:
    free(liste);

    erreur_1:
    fclose(fichier);

    return NULL;
}

int trier_fichier(const char *nom_fichier) {
    Liste *liste = charger_fichier(nom_fichier);
    if (liste == NULL) return -1;

    trier_liste(liste);

    ecrire(liste, nom_fichier);

    detruire_liste(liste);
    return 0;
}

int ecrire(Liste *liste, const char *nom_fichier) {
    if (liste == NULL) return -1;

    FILE *fichier = fopen(nom_fichier, "wb");
    if (fichier == NULL) return -2;

    fwrite("QUARTO", sizeof(char), 6, fichier);
    fwrite(&liste->nb_noeuds, sizeof(liste->nb_noeuds), 1, fichier);

    int n = (int)fwrite(liste->noeuds, sizeof(*liste->noeuds), liste->nb_noeuds, fichier);

    fclose(fichier);
    return liste->nb_noeuds - n;
}

int ecrire_detruire_table(Table *table, const char *nom_fichier) {
    if (table == NULL) return -1;

    FILE *fichier = fopen(nom_fichier, "wb");
    if (fichier == NULL) {
        detruire_table(table);
        return -2;
    }

    fwrite("QUARTO", sizeof(char), 6, fichier);
    fwrite(&table->nb_noeuds, sizeof(table->nb_noeuds), 1, fichier);

    int nb_erreur = 0;
    for (int i = 0; i < table->taille; i++) {
        Element *element = &table->elements[i];
        if (element->noeuds == NULL) continue;

        int n = (int)fwrite(element->noeuds, sizeof(*element->noeuds), element->nb_noeuds, fichier);
        if (n != element->nb_noeuds) nb_erreur += element->nb_noeuds - n;
        free(element->noeuds);
    }

    fclose(fichier);
    return nb_erreur;
}