#include <stdio.h>
#include <stdlib.h>

#include "affichage.h"
#include "quarto.h"
#include "precalcul.h"
#include "canonique.h"
#include "table.h"
#include "fichiers.h"

int solution_coup(const Precalcul *precalcul, int coup, int indice) {
    if (coup < 0 || coup >= 16) return -1;

    char nom_fichier_source[] = "solutions/coup00.000.bin";
    Liste *liste;
    if (coup == 0) {
        liste = liste_depart();
        if (liste == NULL) return -1;
        indice = 0;
    }
    else {
        nom_fichier_source[14] = (char)('0' + (coup / 10) % 10);
        nom_fichier_source[15] = (char)('0' + coup % 10);
        nom_fichier_source[17] = (char)('0' + (indice / 100) % 10);
        nom_fichier_source[18] = (char)('0' + (indice / 10) % 10);
        nom_fichier_source[19] = (char)('0' + indice % 10);
        liste = charger_fichier(nom_fichier_source);
        if (liste == NULL) return -2;

        if (liste->nb_noeuds * 20 * sizeof(Element) > MAX_RAM) {
            detruire_liste(liste);
            return -3;
        }
    }

    Table *table = creer_table((int)liste->nb_noeuds * 20);
    if (table == NULL) {
        detruire_liste(liste);
        return -4;
    }

    int err_creation_noeuds = 0;
    const int nulle = 0, joueur_suivant = 1 + !(coup % 2);
    const int pourcent = liste->nb_noeuds / 100 == 0 ? 1 : (int)(liste->nb_noeuds / 100);

    printf("Nombre de noeuds initiaux : %lld\n", liste->nb_noeuds);
    for (int i = 0; i < liste->nb_noeuds; i++) {
        if (i % pourcent == 0) printf("\r%d%%", (int)(100 * i / liste->nb_noeuds));

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

                if (n->victoire == (Octet)-1) {
                    if (victoire(&p, precalcul->alignements)) n->victoire = joueur_suivant;
                    else n->victoire = nulle;
                }
            }
        }
    }
    printf("\r100%%\n");

    detruire_liste(liste);
    printf("Nombre de noeuds dans table : %d\n", table->nb_noeuds);
    liste = convertir_detruire_table(table);
    if (liste == NULL) return -5;
    trier_liste(liste);
    printf("Nombre de noeuds dans liste : %lld\n", liste->nb_noeuds);

    char nom_fichier_destination[] = "temporaire/000.bin";
    nom_fichier_destination[11] = (char)('0' + (indice / 100) % 10);
    nom_fichier_destination[12] = (char)('0' + (indice / 10) % 10);
    nom_fichier_destination[13] = (char)('0' + indice % 10);

    Valeur n = ecrire(liste, nom_fichier_destination);

    detruire_liste(liste);
    if (n) return -6;
    return err_creation_noeuds;
}