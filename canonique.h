#ifndef CANONIQUE_H
#define CANONIQUE_H

typedef struct Plateau Plateau;
typedef struct Precalcul Precalcul;

int canonique(Plateau *plateau, const Precalcul *precalcul);
/* Calcule la forme canonique d'un plateau permettant de selectionner une combinaison unique par symetrie
 * La fonction reecrit la symetrie directement dans le plateau donne en argument
 * RETOURNE
 *   0 en cas de succes
 *  -1 si aucune meilleure symetrie n'est trouve dans l'objet Precalcul (le plateau reste inchange)
 */

int canonique_2(Plateau *plateau, const Precalcul *precalcul);
/* Identique au precedent a l'exception qu'il ne va tester qu'une meilleure symetrie
 * Le temps de calcul est systematiquement egal ou plus court et est moins gourmand en memoire
 * En revanche, dans certains cas, deux plateaux qui sont equivalents peuvent se retrouver avec des formes caniniques
 * differentes
 * RETOURNE
 *   0 dans tous les cas
 */

#endif //CANONIQUE_H
