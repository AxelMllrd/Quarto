#ifndef QUARTO_H
#define QUARTO_H

typedef unsigned char Octet;
typedef unsigned short Bitboard;
typedef unsigned long long Valeur;

typedef struct Plateau {
    Bitboard caracteristique[4];
    Bitboard placements;
} Plateau;

typedef struct Precalcul {
    int *alignements;
    Bitboard **symetries;
    Octet *meilleures_symetries;
} Precalcul;

typedef struct Noeud {
    Plateau plateau;
    Octet victoire; // 1 -> blanc, >1 -> noir, 0 -> nulle, <0 -> non defini
} Noeud;

typedef struct Liste {
    Noeud *noeuds;
    int nb_noeuds;
} Liste;

typedef struct Element {
    Noeud *noeuds;
    int nb_noeuds;
    int taille_allouee;
} Element;

typedef struct Table {
    int taille, nb_noeuds;
    Element *elements;
} Table;

#define bit(valeur, shift) (((valeur) >> (shift)) & 1)

Plateau plateau_depart();
/* RETOURNE un plateau vide */

int placer(Plateau *plateau, Octet piece, Octet placement);
/* Place une piece sur le plateau en verifiant que le coup soit legal
 * RETOURNE
 *   0 en cas de succes
 *  -1 si la case est deja occupee
 *  -2 si la piece est deja posee
 */

void placer_valide(Plateau *plateau, Octet piece, Octet placement);
/* Identique au precedent a l'exception qu'aucune verification n'est effectuee
 * RETOURNE
 *   0 en cas de succes
 *  -1 si la case est deja occupee
 *  -2 si la piece est deja posee
 */

Valeur valeur(const Plateau *plateau);
/* RETOURNE
 *   Une valeur sur 64 bits unique aux caracteristiques du plateau (pas les placements)
 */

int plateau_egal(const Plateau *p1, const Plateau *p2);
/* RETOURNE
 *   1 si les deux plateaux sot strictement identiques
 *   0 sinon
 */

int comparer_plateaux(const void *p1, const void *p2);
/* Compare deux plateaux
 * RETOURNE
 *   1 si p1 > p2
 *   0 si p1 == p2
 *  -1 si p1 < p2
 *  Note : On compare d'abord le placement avant les caracteristiques
 */

#endif //QUARTO_H
