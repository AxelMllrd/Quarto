#ifndef TABLE_H
#define TABLE_H

typedef unsigned char Octet;
typedef unsigned short Bitboard;
typedef unsigned long long Valeur;

typedef struct Plateau Plateau;
typedef struct Noeud Noeud;
typedef struct Liste Liste;
typedef struct Table Table;

int comparer_noeuds(const void *n1, const void *n2);
/* Compare les plateaux respectifs des noeuds
 * RETOURNE
 *   1 si n1->plateau > n2->plateau
 *   0 si n1->plateau == n2->plateau
 *  -1 si n1->plateau < n2->plateau
 */

Liste *liste_depart();
/* Cree la liste de depart avec 1 seul noeud : le plateau vide
 * RETOURNE
 *   NULL en cas d'echec
 *   La liste de depart sinon
 */

void trier_liste(Liste *liste);
/* Trie une liste dans l'ordre croissant par un algorithme de Quick Sort */

void detruire_liste(Liste *liste);
/* Detruit la liste */

Valeur hacher(const Plateau *plateau);
/* RETOURNE
 *   Une valeure de hachage unique selon le plateau
 */

Table *creer_table(int taille);
/* Cree une table de hachage
 * RETOURNE
 *   NULL en cas d'echec
 *   La table de hachage sinon
 */

void detruire_table(Table *table);
/* Detruit la table de hachage */

Noeud *ajouter_plateau(Table *table, const Plateau *plateau);
/* Ajoute un nouveau noeud a la table de hachage
 * RETOURNE
 *   NULL en cas d'echec de l'ajout
 *   Le noeud sinon
 * Note : Si le noeud existe deja, il n'est pas remplace
 */

Liste *convertir_table(Table *table);
/* Convertit la table de hachage en liste comprenant uniquement les elements enregistres dans la table
 * RETOURNE
 *   NULL en cas d'echec
 *   La liste de noeuds sinon
 */

Liste *convertir_detruire_table(Table *table);
/* Convertit la table de hachage en liste et la detruit
 * RETOURNE
 *   NULL en cas d'echec (la table est quand meme detruite)
 *   La liste de noeuds sinon
 */

#endif //TABLE_H
