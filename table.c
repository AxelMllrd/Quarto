#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quarto.h"

int comparer_noeuds(const void *n1, const void *n2) {
    Noeud *noeud1 = (Noeud *)n1, *noeud2 = (Noeud *)n2;
    return comparer_plateaux(&noeud1->plateau, &noeud2->plateau);
}

Liste *liste_depart() {
    Liste *liste = malloc(sizeof(*liste));
    if (liste == NULL) return NULL;

    Noeud *noeud = malloc(sizeof(*noeud));
    if (noeud == NULL) {
        free(liste);
        return NULL;
    }

    noeud->plateau = plateau_depart();
    noeud->victoire = 0;
    liste->noeuds = noeud;
    liste->nb_noeuds = 1;

    return liste;
}

void trier_liste(Liste *liste) {
    qsort(liste->noeuds, liste->nb_noeuds, sizeof(*liste->noeuds), comparer_noeuds);
}

void detruire_liste(Liste *liste) {
    if (liste == NULL) return;

    free(liste->noeuds);
    free(liste);
}

Valeur hacher(const Plateau *plateau) {
    Valeur hachage = valeur(plateau) ^ plateau->placements * 0xC4CEB9FE1A85EC53ULL;
    hachage ^= hachage >> 33;
    //hash *= 0xFF51AFD7ED558CCDULL;
    //hash ^= hash >> 33;.
    //hash *= 0xC4CEB9FE1A85EC53ULL;
    //hash ^= hash >> 33;

    return hachage;
}

Table *creer_table(int taille) {
    if (taille <= 0) return NULL;

    Table *table = malloc(sizeof(*table));
    if (table == NULL) return NULL;

    table->elements = calloc(taille, sizeof(*table->elements));
    if (table->elements == NULL) {
        free(table);
        return NULL;
    }

    table->taille = taille;

    return table;
}

void detruire_table(Table *table) {
    if (table == NULL) return;

    for (int i = 0; i < table->taille; i++) free(table->elements[i].noeuds);
    free(table->elements);
    free(table);
}

Noeud *ajouter_plateau(Table *table, const Plateau *plateau) {
    unsigned int n = hacher(plateau) % table->taille;
    Element *element = &table->elements[n];

    for (int i = 0; i < element->nb_noeuds; i++) {
        if (plateau_egal(plateau, &element->noeuds[i].plateau))
            return &element->noeuds[i];
    }

    table->nb_noeuds++;
    if (element->taille_allouee < ++element->nb_noeuds) {
        element->taille_allouee += 5;
        Noeud *noeuds = realloc(element->noeuds, element->taille_allouee * sizeof(*element->noeuds));
        if (noeuds == NULL) {
            element->taille_allouee -= 5;
            element->nb_noeuds--;
            table->nb_noeuds--;
            return NULL;
        }
        element->noeuds = noeuds;
    }
    Noeud *noeud = &element->noeuds[element->nb_noeuds - 1];
    noeud->plateau = *plateau;
    noeud->victoire = (Octet)-1;

    return noeud;
}

Liste *convertir_table(Table *table) {
    if (table == NULL || table->nb_noeuds < 1) return NULL;

    Liste *liste = malloc(sizeof(*liste));
    if (liste == NULL) return NULL;

    Noeud *noeuds = malloc(table->nb_noeuds * sizeof(*noeuds));
    if (noeuds == NULL) {
        free(liste);
        return NULL;
    }

    int indice = 0;
    for (int i = 0; i < table->taille; i++) {
        Element *element = &table->elements[i];
        if (element->nb_noeuds > 0) {
            memcpy(noeuds + indice, element->noeuds, element->nb_noeuds * sizeof(*noeuds));
            indice += element->nb_noeuds;
            indice += element->nb_noeuds;
        }
    }

    liste->noeuds = noeuds;
    liste->nb_noeuds = table->nb_noeuds;
    return liste;
}

Liste *convertir_detruire_table(Table *table) {
    if (table == NULL || table->nb_noeuds < 1) {
        detruire_table(table);
        return NULL;
    }

    Liste *liste = malloc(sizeof(*liste));
    if (liste == NULL) {
        detruire_table(table);
        return NULL;
    }

    Noeud *noeuds = malloc(table->nb_noeuds * sizeof(*noeuds));
    if (noeuds == NULL) {
        detruire_table(table);
        free(liste);
        return NULL;
    }

    int indice = 0;
    for (int i = 0; i < table->taille; i++) {
        Element *element = &table->elements[i];
        if (element->nb_noeuds > 0) {
            memcpy(noeuds + indice, element->noeuds, element->nb_noeuds * sizeof(*noeuds));
            indice += element->nb_noeuds;
            free(element->noeuds);
        }
    }

    liste->noeuds = noeuds;
    liste->nb_noeuds = table->nb_noeuds;

    free(table->elements);
    free(table);
    return liste;
}