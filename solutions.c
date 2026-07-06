#include <stdio.h>
#include <stdlib.h>

#include "quarto.h"
#include "precalcul.h"
#include "canonique.h"
#include "table.h"
#include "fichiers.h"

#define MAX_RAM 500000000

int solution_coup(const Precalcul *precalcul, int coup) {
    if (coup < 0 || coup >= 16) return -1;

    char nom_fichier[] = "solutions/coup_0.00.bin";
    Liste *liste;
    if (coup == 0) {
        liste = liste_depart();
        if (liste == NULL) return -1;
    }
    else {
        nom_fichier[15] = (char)('0' + coup);
        liste = charger_fichier(nom_fichier);
        if (liste == NULL) return -2;

        if (liste->nb_noeuds * 20 * sizeof(Element) > MAX_RAM) { // Ne pas depasser 1 Go de RAM
            detruire_liste(liste);
            return -3;
        }
    }

    Table *table = creer_table(liste->nb_noeuds * 20);
    if (table == NULL) {
        detruire_liste(liste);
        return -4;
    }

    int err_creation_noeuds = 0;
    const int nulle = 0, joueur_suivant = 1 + !(coup % 2);
    const int pourcent = liste->nb_noeuds / 100 == 0 ? 1 : liste->nb_noeuds / 100;

    printf("Nombre de noeuds : %d\n", liste->nb_noeuds);
    for (int i = 0; i < liste->nb_noeuds; i++) {
        if (i % pourcent == 0) printf("\r%d%%", 100 * i / liste->nb_noeuds);

        const Plateau *plateau = &liste->noeuds[i].plateau;

        for (Octet piece = 0; piece < 16; piece++) {
            Bitboard test = plateau->placements &
            ~((plateau->caracteristique[0] ^ -(bit(piece, 0))) |
              (plateau->caracteristique[1] ^ -(bit(piece, 1))) |
              (plateau->caracteristique[2] ^ -(bit(piece, 2))) |
              (plateau->caracteristique[3] ^ -(bit(piece, 3))));
            if (test) continue;

            for (Octet placement = 0; placement < 16; placement++) {
                if (bit(plateau->placements, placement)) continue;

                Plateau p = *plateau;
                placer_valide(&p, piece, placement);
                canonique(&p, precalcul);
                Noeud *n = ajouter_plateau(table, &p);
                if (n == NULL) {
                    err_creation_noeuds++;
                    continue;
                }

                if (n->victoire == -1) {
                    if (victoire(&p, precalcul->alignements)) n->victoire = joueur_suivant;
                    else n->victoire = nulle;
                }
            }
        }
    }
    printf("\r100%%\n");

    nom_fichier[15] = (char)('1' + coup);

    detruire_liste(liste);
    liste = convertir_detruire_table(table);
    if (liste == NULL) return -5;
    //trier_liste(liste);

    int nb_fichiers = 1 + liste->nb_noeuds * 20 * (int)sizeof(Element) / MAX_RAM;
    Liste listes_separees[nb_fichiers];

    int nb_noeuds = liste->nb_noeuds / nb_fichiers, additionnels = liste->nb_noeuds % nb_fichiers;
    int indice = 0;
    for (int i = 0; i < nb_fichiers; i++) {
        listes_separees[i].noeuds = liste->noeuds + indice;
        listes_separees[i].nb_noeuds = nb_noeuds + (i < additionnels ? 1 : 0);
        indice += listes_separees[i].nb_noeuds;
    }

    for (int i = 0; i < nb_fichiers; i++) {
        nom_fichier[17] = (char)('0' + i / 10);
        nom_fichier[18] = (char)('0' + i % 10);
        int err = ecrire(&listes_separees[i], nom_fichier);
        if (err < 0) {
            detruire_liste(liste);
            return err -7;
        }
    }

    detruire_liste(liste);
    return err_creation_noeuds;
}