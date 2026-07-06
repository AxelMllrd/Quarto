#ifndef AFFICHAGE_H
#define AFFICHAGE_H

typedef unsigned char Octet;
typedef unsigned short Bitboard;

typedef struct Plateau Plateau;

void aff_octet(Octet o);
/* Affiche un octet de 8 bits en binaire */

void aff_bitboard(Bitboard bitboard);
/* Affiche une bitboard 4 par 4 bits
 * Note : Les bits se lisent de droite a gauche et de bas en haut (le bit de poids faible est en bas a droite)
 */

void aff_plateau(const Plateau *plateau);
/* Affiche un plateau 4 par 4 cases, les pieces sont representees en binaire et les cases vides par "----"
 * Note : Le plateau se lit de droite a gauche, et de bas en haut (la case 0 est en bas a droite)
 */

#endif //AFFICHAGE_H
