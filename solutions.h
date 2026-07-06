#ifndef SOLUTIONS_H
#define SOLUTIONS_H

typedef struct Precalcul Precalcul;

int solution_coup(const Precalcul *precalcul, int coup);
/* Calcule et enregistre dans un fichier nomme "solution/coup_{coup}.bin" les noeuds qui decoulent des positions
 * precedentes - ces dernieres sont obtenues par le fichier precedent
 * RETOURNE
 *  <0 en cas d'echec critique (chargement du fichier, creation de la table, etc.)
 *   0 en cas de succes
 *  >0 pour chaque noeud qui n'a pas pu etre ajouter a la table
 */

#endif //SOLUTIONS_H
