#include <stdlib.h>

#include "quarto.h"
#include "precalcul.h"

//--- Alignements ---///

int *calculer_alignements() {
    int *solution = calloc(2048, sizeof(*solution));
    if (solution == NULL) return NULL;

    Bitboard masque[10] = {
        0x000F, 0x00F0, 0x0F00, 0xF000,
        0x1111, 0x2222, 0x4444, 0x8888,
        0x1248, 0x8421
    };

    for (Bitboard bitboard = 1; bitboard > 0; bitboard++) {
        for (int j = 0; j < 10; j++) {
            if ((bitboard & masque[j]) == masque[j]) {
                solution[bitboard / 32] |= 1 << (bitboard % 32);
                break;
            }
        }
    }

    return solution;
}

int alignement(const int *solution, Bitboard bitboard) {
    return bit(solution[bitboard / 32], bitboard % 32);
}

int victoire(const Plateau *plateau, const int *alignements) {
    for (int i = 0; i < 4; i++) {
        Bitboard cara = plateau->caracteristique[i] & plateau->placements;
        if (alignement(alignements, cara)) return i + 1;

        cara = ~plateau->caracteristique[i] & plateau->placements;
        if (alignement(alignements, cara)) return i + 1;
    }

    return 0;
}

//--- Symetries ---//

Bitboard rota_transposee(Bitboard bitboard) {
    return (bitboard & 0x8421) |
           (bitboard & 0x0842) << 3 |
           (bitboard & 0x4210) >> 3 |
           (bitboard & 0x0084) << 6 |
           (bitboard & 0x2100) >> 6 |
           (bitboard & 0x0008) << 9 |
           (bitboard & 0x1000) >> 9;
}
Bitboard rota_horizontale(Bitboard bitboard) {
    return (bitboard & 0xF000) >> 12 |
           (bitboard & 0x0F00) >> 4  |
           (bitboard & 0x00F0) << 4  |
           (bitboard & 0x000F) << 12;
}
Bitboard rota_verticale(Bitboard bitboard) {
    return (bitboard & 0x8888) >> 3 |
           (bitboard & 0x4444) >> 1 |
           (bitboard & 0x2222) << 1 |
           (bitboard & 0x1111) << 3;
}
Bitboard rota_90_deg(Bitboard bitboard) {
    return rota_transposee(rota_horizontale(bitboard));
}
Bitboard rota_180_deg(Bitboard bitboard) {
    return rota_horizontale(rota_verticale(bitboard));
}
Bitboard rota_270_deg(Bitboard bitboard) {
    return rota_horizontale(rota_transposee(bitboard));
}
Bitboard rota_90_deg_inverse(Bitboard bitboard) {
    return rota_horizontale(rota_90_deg(bitboard));
}

void detruire_symetries(Bitboard **symetries) {
    if (symetries == NULL) return;

    for (int i = 0; i < 7; i++) free(symetries[i]);
    free(symetries);
}

Bitboard **calculer_symetries() {
    Bitboard (*rot[])(Bitboard) = {
        rota_90_deg, rota_180_deg, rota_270_deg, rota_verticale,
        rota_90_deg_inverse, rota_horizontale, rota_transposee
    };

    Bitboard **symetries = calloc(7, sizeof(*symetries));
    if (symetries == NULL) return NULL;

    for (int i = 0; i < 7; i++) {
        Bitboard *type = malloc(65536 * sizeof(*type));
        if (type == NULL) {
            detruire_symetries(symetries);
            return NULL;
        }
        symetries[i] = type;
        type[0] = 0;

        for (Bitboard b = 1; b > 0; b++) {
            type[b] = rot[i](b);
        }
    }

    return symetries;
}

//--- Meilleures symetries ---//

Octet *calculer_meilleures_symetries(Bitboard **symetries) {
    if (symetries == NULL) return NULL;

    Octet *meilleure_sym = calloc(65536, sizeof(*meilleure_sym));
    if (meilleure_sym == NULL) return NULL;
    meilleure_sym[0] = 1;

    for (Bitboard b = 1; b > 0; b++) {
        Octet resultat = 1;
        Bitboard min = b;
        for (int i = 0; i < 7; i++) {
            Bitboard sym = symetries[i][b];
            if (sym < min) {
                min = sym;
                resultat = 1 << (i + 1);
            }
            else if (sym == min)
                resultat |= 1 << (i + 1);
        }

        meilleure_sym[b] = resultat;
    }

    return meilleure_sym;
}

Precalcul *precaluler() {
    Precalcul *precalcul = malloc(sizeof(*precalcul));
    if (precalcul == NULL) return NULL;

    precalcul->alignements = calculer_alignements();
    precalcul->symetries = calculer_symetries();
    precalcul->meilleures_symetries = calculer_meilleures_symetries(precalcul->symetries);
    if (!(precalcul->alignements && precalcul->symetries && precalcul->meilleures_symetries)) {
        detruire_precalcul(precalcul);
        return NULL;
    }

    return precalcul;
}

void detruire_precalcul(Precalcul *precalcul) {
    free(precalcul->alignements);
    detruire_symetries(precalcul->symetries);
    free(precalcul->meilleures_symetries);
    free(precalcul);
}