#ifndef FICHIERS_H
#define FICHIERS_H

typedef unsigned long long Valeur;

typedef struct Liste Liste;
typedef struct Table Table;

Liste *charger_fichier(const char *nom_fichier);
/* Charge un fichier de solution (par ex. "solution/coup_0.bin") dans une liste de noeuds
 * RETOURNE
 *   NULL en cas d'echec
 *   La liste de noeuds sinon
 */

int trier_fichier(const char *nom_fichier);
/* Extrait les donners d'un fichier dans une liste de noeuds, trie cette liste, puis enregistre a nouveau la liste dans
 * un fichier du meme nom (les anciennes donnees sont ecrasees)
 * RETOURNE
 *   0 en cas de succes
 *  -1 sinon (le fichier reste inchange)
 */

Valeur ecrire(Liste *liste, const char *nom_fichier);
/* Ecrit dans un fichier (par ex. "solution/coup_1.bin") les noeuds enregistres
 * RETOURNE
 *  >0 pour la quantite de noeuds qui n'ont pas ete ecrits dans le fichier
 *   0 en cas de succes
 */

Valeur ecrire_detruire_table(Table *table, const char *nom_fichier);
/* Ecrit dans un fichier (par ex. "solution/coup_1.bin") les noeuds enregistres et detruit la table
 * RETOURNE
 *  >0 pour la quantite de noeuds qui n'ont pas ete ecrits dans le fichier
 *   0 en cas de succes
 *  <0 en cas d'echec critique de l'ecriture (la table est quand meme detruite)
 */

Valeur creer_sommaire();
Valeur fusionner(int coup, int nb_fichiers);
Valeur separer_fichier(int coup);
int lire_sommaire(int coup);


#endif //FICHIERS_H
