#include <stdio.h>
#include <stdlib.h>

#include "quarto.h"
#include "fichiers.h"
#include "table.h"

Liste *charger_fichier(const char *nom_fichier) {
    FILE *fichier = fopen(nom_fichier, "rb");
    if (fichier == NULL) return NULL;

    char entete[6];
    fread(entete, sizeof(char), sizeof(entete), fichier);
    if (entete[0] != 'Q' || entete[1] != 'U' || entete[2] != 'A' ||
        entete[3] != 'R' || entete[4] != 'T' || entete[5] != 'O')
        goto erreur_1;

    Valeur nb_noeuds;
    fread(&nb_noeuds, sizeof(nb_noeuds), 1, fichier);
    if (nb_noeuds < 1) goto erreur_1;

    Liste *liste = malloc(sizeof(*liste));
    if (liste == NULL) goto erreur_1;

    Noeud *noeuds = malloc(nb_noeuds * sizeof(*noeuds));
    if (noeuds == NULL) goto erreur_2;

    size_t n = fread(noeuds, sizeof(*noeuds), nb_noeuds, fichier);
    if (n != nb_noeuds) goto erreur_3;

    liste->noeuds = noeuds;
    liste->nb_noeuds = nb_noeuds;
    fclose(fichier);
    return liste;

    erreur_3:
    free(noeuds);

    erreur_2:
    free(liste);

    erreur_1:
    fclose(fichier);

    return NULL;
}

int trier_fichier(const char *nom_fichier) {
    Liste *liste = charger_fichier(nom_fichier);
    if (liste == NULL) return -1;

    trier_liste(liste);

    ecrire(liste, nom_fichier);

    detruire_liste(liste);
    return 0;
}

Valeur ecrire(Liste *liste, const char *nom_fichier) {
    if (liste == NULL) return -1;

    FILE *fichier = fopen(nom_fichier, "wb");
    if (fichier == NULL) return -2;

    fwrite("QUARTO", sizeof(char), 6, fichier);
    fwrite(&liste->nb_noeuds, sizeof(liste->nb_noeuds), 1, fichier);

    Valeur n = fwrite(liste->noeuds, sizeof(*liste->noeuds), liste->nb_noeuds, fichier);

    fclose(fichier);
    return liste->nb_noeuds - n;
}

Valeur ecrire_detruire_table(Table *table, const char *nom_fichier) {
    if (table == NULL) return -1;

    FILE *fichier = fopen(nom_fichier, "wb");
    if (fichier == NULL) {
        detruire_table(table);
        return -2;
    }

    fwrite("QUARTO", sizeof(char), 6, fichier);
    fwrite(&table->nb_noeuds, sizeof(table->nb_noeuds), 1, fichier);

    Valeur nb_erreur = 0;
    for (int i = 0; i < table->taille; i++) {
        Element *element = &table->elements[i];
        if (element->noeuds == NULL) continue;

        Valeur n = fwrite(element->noeuds, sizeof(*element->noeuds), element->nb_noeuds, fichier);
        if (n != element->nb_noeuds) nb_erreur += element->nb_noeuds - n;
        free(element->noeuds);
    }

    fclose(fichier);
    return nb_erreur;
}

Valeur creer_sommaire() {
    int nb_fichiers[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

    FILE *fichier = fopen("solutions/sommaire.bin", "wb");
    if (fichier == NULL) return -1;

    fwrite("QUARTO", sizeof(char), 6, fichier);
    Valeur n = fwrite(nb_fichiers, sizeof(*nb_fichiers), sizeof(nb_fichiers), fichier);

    fclose(fichier);
    return sizeof(nb_fichiers) - n;
}

Valeur fusionner(int coup, int nb_fichiers) {
    if (coup < 0 || coup >= 16 || nb_fichiers < 1 || nb_fichiers > 200) return -1;

    char nom_fichier_destination[] = "solutions/coup00.bin";
    nom_fichier_destination[14] = (char)('0' + coup / 10);
    nom_fichier_destination[15] = (char)('0' + coup % 10);
    FILE *fichier_destination = fopen(nom_fichier_destination, "wb");
    if (fichier_destination == NULL) return -2;

    FILE *fichiers[nb_fichiers];
    Noeud noeuds[nb_fichiers];
    Valeur compteurs[nb_fichiers], taille_totale = 0;

    char nom_fichier_source[] = "temporaire/000.bin";
    for (int i = 0; i < nb_fichiers; i++) {
        nom_fichier_source[11] = (char)('0' + (i / 100) % 10);
        nom_fichier_source[12] = (char)('0' + (i / 10) % 10);
        nom_fichier_source[13] = (char)('0' + i % 10);

        fichiers[i] = fopen(nom_fichier_source, "rb");
        if (fichiers[i] == NULL) {
            for (i--; i >= 0; i--) fclose(fichiers[i]);
            fclose(fichier_destination);
            return -3;
        }

        char entete[6];
        fread(entete, sizeof(char), sizeof(entete), fichiers[i]);
        if (entete[0] != 'Q' || entete[1] != 'U' || entete[2] != 'A' ||
            entete[3] != 'R' || entete[4] != 'T' || entete[5] != 'O') {
            for (; i >= 0; i--) fclose(fichiers[i]);
            fclose(fichier_destination);
            return -4;
        }

        fread(&compteurs[i], sizeof(compteurs[i]), 1, fichiers[i]);
        fread(&noeuds[i], sizeof(noeuds[i]), 1, fichiers[i]);
        //compteurs[i]--;
        taille_totale += compteurs[i];
    }

    fwrite("QUARTO", sizeof(char), 6, fichier_destination);
    fwrite(&taille_totale, sizeof(taille_totale), 1, fichier_destination);

    int nb_fichiers_ouverts = nb_fichiers;
    while (nb_fichiers_ouverts > 1) {
        int min = 0;
        while (fichiers[min] == NULL) min++;
        for (int i = min + 1; i < nb_fichiers; i++) {
            if (fichiers[i] == NULL) continue;

            int resultat = comparer_noeuds(&noeuds[min], &noeuds[i]);
            if (resultat == 0) {
                noeuds[min].victoire |= noeuds[i].victoire;
                compteurs[i]--;
                if (compteurs[i] == 0) {
                    fclose(fichiers[i]);
                    fichiers[i] = NULL;
                    nb_fichiers_ouverts--;
                }
                else {
                    fread(&noeuds[i], sizeof(noeuds[i]), 1, fichiers[i]);
                    i--;
                }
            }
            else if (resultat > 0) {
                min = i;
            }
        }

        fwrite(&noeuds[min], sizeof(noeuds[min]), 1, fichier_destination);
        compteurs[min]++;
        if (compteurs[min] == 0) {
            fclose(fichiers[min]);
            fichiers[min] = NULL;
            nb_fichiers_ouverts--;
        }
        else fread(&noeuds[min], sizeof(noeuds[min]), 1, fichiers[min]);
    }

    if (nb_fichiers_ouverts == 1) {
        int i = 0;
        while (fichiers[i] == NULL) i++;
        fwrite(&noeuds[i], sizeof(noeuds[i]), 1, fichier_destination);

        Valeur nb_noeuds_restants = compteurs[i] - 1;
        if (nb_noeuds_restants > 0) {
            Noeud *noeuds_restants = malloc(nb_noeuds_restants * sizeof(Noeud));
            if (noeuds_restants == NULL) {
                fclose(fichiers[i]);
                fclose(fichier_destination);
                return -5;
            }

            fread(noeuds_restants, sizeof(*noeuds_restants), nb_noeuds_restants, fichiers[i]);
            fwrite(noeuds_restants, sizeof(*noeuds_restants), nb_noeuds_restants, fichier_destination);

            free(noeuds_restants);
        }

        fclose(fichiers[i]);
    }
    fclose(fichier_destination);

    return 0;
}

Valeur separer_fichier(int coup) {
    if (coup < 0 || coup >= 16) return -1;

    char nom_fichier_source[] = "solutions/coup00.bin";
    nom_fichier_source[14] = (char)('0' + coup / 10);
    nom_fichier_source[15] = (char)('0' + coup % 10);

    FILE *fichier_source = fopen(nom_fichier_source, "rb");
    if (fichier_source == NULL) return -2;

    char entete[6];
    fread(entete, sizeof(char), sizeof(entete), fichier_source);
    if (entete[0] != 'Q' || entete[1] != 'U' || entete[2] != 'A' ||
        entete[3] != 'R' || entete[4] != 'T' || entete[5] != 'O') {
        fclose(fichier_source);
        return -3;
    }

    Valeur nb_noeuds;
    fread(&nb_noeuds, sizeof(nb_noeuds), 1, fichier_source);

    int nb_fichiers = 1 + (int)(nb_noeuds * 20 * sizeof(Element) / MAX_RAM);
    Valeur taille = nb_noeuds / nb_fichiers;
    int additionnels = (int)(nb_noeuds % nb_fichiers);

    Noeud *noeuds = malloc((taille + 1) * sizeof(*noeuds));
    if (noeuds == NULL) {
        fclose(fichier_source);
        return -4;
    }

    char nom_fichier_destination[] = "solutions/coup00.000.bin";
    nom_fichier_destination[14] = (char)('0' + coup / 10);
    nom_fichier_destination[15] = (char)('0' + coup % 10);

    Valeur nb_erreurs = 0;
    for (int i = 0; i < nb_fichiers; i++) {
        Valeur taille_actuelle = taille + (i < additionnels ? 1 : 0);

        nom_fichier_destination[17] = (char)('0' + (i / 100) % 10);
        nom_fichier_destination[18] = (char)('0' + (i / 10) % 10);
        nom_fichier_destination[19] = (char)('0' + i % 10);
        FILE *fichier_destination = fopen(nom_fichier_destination, "wb");
        if (fichier_destination == NULL) {
            fclose(fichier_source);
            free(noeuds);
            return -5;
        }
        fwrite(entete, sizeof(char), sizeof(entete), fichier_destination);
        fwrite(&taille_actuelle, sizeof(taille_actuelle), 1, fichier_destination);

        Valeur n = fread(noeuds, sizeof(*noeuds), taille_actuelle, fichier_source);
        if (n != taille_actuelle) {
            fclose(fichier_destination);
            fclose(fichier_source);
            free(noeuds);
            return -6;
        }
        n = fwrite(noeuds, sizeof(*noeuds), taille_actuelle, fichier_destination);
        nb_erreurs += taille_actuelle - n;

        fclose(fichier_destination);
    }
    fclose(fichier_source);
    free(noeuds);

    FILE *fichier_sommaire = fopen("solutions/sommaire.bin", "rb+");
    if (fichier_sommaire == NULL) return -7;

    fread(entete, sizeof(char), sizeof(entete), fichier_sommaire);
    if (entete[0] != 'Q' || entete[1] != 'U' || entete[2] != 'A' ||
        entete[3] != 'R' || entete[4] != 'T' || entete[5] != 'O') {
        fclose(fichier_sommaire);
        return -8;
    }

    if (fseek(fichier_sommaire, (long)(coup * sizeof(int)), SEEK_CUR) != 0) {
        fclose(fichier_sommaire);
        return -9;
    }

    fwrite(&nb_fichiers, sizeof(nb_fichiers), 1, fichier_sommaire);

    fclose(fichier_sommaire);
    return nb_erreurs;
}

int lire_sommaire(int coup) {
    FILE *fichier = fopen("solutions/sommaire.bin", "rb");
    if (fichier == NULL) return -1;

    char entete[6];
    fread(entete, sizeof(char), sizeof(entete), fichier);
    if (entete[0] != 'Q' || entete[1] != 'U' || entete[2] != 'A' ||
        entete[3] != 'R' || entete[4] != 'T' || entete[5] != 'O') {
        fclose(fichier);
        return -2;
    }

    if (fseek(fichier, (long)(coup * sizeof(int)), SEEK_CUR) != 0) {
        fclose(fichier);
        return -3;
    }

    int nb_fichiers;
    if (fread(&nb_fichiers, sizeof(nb_fichiers), 1, fichier) != 1) {
        fclose(fichier);
        return -4;
    }

    fclose(fichier);
    return nb_fichiers;
}