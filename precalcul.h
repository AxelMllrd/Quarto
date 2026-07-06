#ifndef PRECALCUL_H
#define PRECALCUL_H

typedef unsigned short Bitboard;

typedef struct Precalcul Precalcul;

int alignement(const int *solution, Bitboard bitboard);
/* RETOURNE
 *   1 s'il un alignement est present dans une bitboard (4 verticaux, 4 horizontaux, 2 diagonaux)
 *   0 sinon
 */

int victoire(const Plateau *plateau, const int *alignements);
/* RETOURNE
 *   1 si le plateau possede une condition de victoire (au moins un alignement parmi les 4 caracteristiques)
 *   0 sinon
 */

Precalcul *precaluler();
/* Effectue le calcul definitif pour chaque bitboard possible (2^16) des alignements, symetries et la meilleure
 * symetrie pour un placement de pieces donne
 * RETOURNE
 *   NULL en cas d'echec
 *   Un objet Precalcul contenant ces solutions sinon
 */

void detruire_precalcul(Precalcul *precalcul);
/* Detruit l'objet Precalcul */

#endif //PRECALCUL_H
