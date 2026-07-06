#include <stdlib.h>
#include <stdio.h>

#include "quarto.h"

Plateau plateau_depart() {
    return (Plateau){0, 0, 0, 0, 0};
}

int placer(Plateau *plateau, Octet piece, Octet placement) {
    placement %= 16;

    if (bit(plateau->placements, placement)) return -1;

    Bitboard test = plateau->placements &
            ~((plateau->caracteristique[0] ^ -(bit(piece, 0))) |
              (plateau->caracteristique[1] ^ -(bit(piece, 1))) |
              (plateau->caracteristique[2] ^ -(bit(piece, 2))) |
              (plateau->caracteristique[3] ^ -(bit(piece, 3))));
    if (test) return -2;

    for (int i = 0; i < 4; i++)
        plateau->caracteristique[i] ^= bit(piece, i) << placement;
    plateau->placements |= 1 << placement;

    return 0;
}

void placer_valide(Plateau *plateau, Octet piece, Octet placement) {
    for (int i = 0; i < 4; i++)
        plateau->caracteristique[i] ^= bit(piece, i) << placement;
    plateau->placements |= 1 << placement;
}

Valeur valeur(const Plateau *plateau) {
    return (Valeur)plateau->caracteristique[0]       |
           (Valeur)plateau->caracteristique[1] << 16 |
           (Valeur)plateau->caracteristique[2] << 32 |
           (Valeur)plateau->caracteristique[3] << 48;
}

int plateau_egal(const Plateau *p1, const Plateau *p2) {
    if (p1->placements == p2->placements && valeur(p1) == valeur(p2)) return 1;
    return 0;
}

int comparer_plateaux(const void *p1, const void *p2) {
    Plateau *plat1 = (Plateau *)p1, *plat2 = (Plateau *)p2;
    if (plat1->placements != plat2->placements)
        return plat1->placements > plat2->placements ? 1 : -1;

    Valeur v1 = valeur(plat1), v2 = valeur(plat2);
    if (v1 != v2) return v1 > v2 ? 1 : -1;

    return 0;
}