#include "quarto.h"

#define echanger(a, b) do { typeof(a) _t_inmacro = a; a = b; b = _t_inmacro; } while(0)

void trier(Plateau *plateau) {
    Bitboard *cara = plateau->caracteristique;

    if (cara[0] < cara[1]) echanger(cara[0], cara[1]);
    if (cara[2] < cara[3]) echanger(cara[2], cara[3]);
    if (cara[0] < cara[2]) echanger(cara[0], cara[2]);
    if (cara[1] < cara[3]) echanger(cara[1], cara[3]);
    if (cara[1] < cara[2]) echanger(cara[1], cara[2]);
}

int canonique(Plateau *plateau, const Precalcul *precalcul) {
    Bitboard *cara = plateau->caracteristique;
    Bitboard placements = plateau->placements;
    Bitboard **symetries = precalcul->symetries;

    Plateau plat[8];
    Valeur val_min = (Valeur)(-1);
    int min = -1;

    Octet meil_sym = precalcul->meilleures_symetries[plateau->placements];

    while (meil_sym != 0) {
        int i = __builtin_ctz(meil_sym);
        meil_sym &= meil_sym - 1;

        if (i == 0) plat[0] = *plateau;
        else plat[i] = (Plateau){
            symetries[i - 1][cara[0]],
            symetries[i - 1][cara[1]],
            symetries[i - 1][cara[2]],
            symetries[i - 1][cara[3]],
            symetries[i - 1][placements]
        };

        for (int c = 0; c < 4; c++) {
            if ((~plat[i].caracteristique[c] & plat[i].placements) < plat[i].caracteristique[c])
                plat[i].caracteristique[c] = ~plat[i].caracteristique[c] & plat[i].placements;
        }

        trier(&plat[i]);
        Valeur val = valeur(&plat[i]);
        if (val < val_min) {
            val_min = val;
            min = i;
        }
    }

    if (min == -1) return -1;

    *plateau = plat[min];
    return 0;
}

int canonique_2(Plateau *plateau, const Precalcul *precalcul) {
    Bitboard *cara = plateau->caracteristique;
    Bitboard placements = plateau->placements;
    Bitboard **symetries = precalcul->symetries;

    Octet meil_sym = precalcul->meilleures_symetries[plateau->placements];

    if (meil_sym <= 1) {
        int i = __builtin_ctz(meil_sym);

        *plateau = (Plateau){
            symetries[i - 1][cara[0]],
            symetries[i - 1][cara[1]],
            symetries[i - 1][cara[2]],
            symetries[i - 1][cara[3]],
            symetries[i - 1][placements]
        };
    }

    for (int c = 0; c < 4; c++) {
        if ((~cara[c] & plateau->placements) < cara[c])
            cara[c] = ~cara[c] & plateau->placements;
    }

    trier(plateau);

    return 0;
}