#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL3/SDL_events.h>

#include "fichiers.h"
#include "quarto.h"
#include "tests.h"

int main(void) {
    time_t t;
    srand(time(&t));
    start_timer();

    struct tm *t_local = localtime(&t);
    printf("Lancement a %02d:%02d:%02d\n\n", t_local->tm_hour, t_local->tm_min, t_local->tm_sec);


    creer_sommaire();
    for (int i = 0; i < 6; i++) {
        int nb_fichiers = lire_sommaire(i);
        printf("Nombre de fichiers : %d\n", nb_fichiers);
        for (int j = 0; j < nb_fichiers; j++) {
            test_solution_coup(i, j);
        }
    }

    double duree = get_time();
    time(&t);
    t_local = localtime(&t);
    printf("Temps d'execution : %f s\n", duree);
    printf("Fin a %02d:%02d:%02d\n", t_local->tm_hour, t_local->tm_min, t_local->tm_sec);

    getchar();
    return 0;
}