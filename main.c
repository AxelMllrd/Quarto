#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL3/SDL_events.h>

#include "quarto.h"
#include "tests.h"

int main(void) {
    time_t t;
    srand(time(&t));
    start_timer();

    struct tm *t_local = localtime(&t);
    printf("Lancement a %02d:%02d:%02d\n\n", t_local->tm_hour, t_local->tm_min, t_local->tm_sec);

    test_solution_coup(7);
    //test_trier();

    double duree = get_time();
    time(&t);
    t_local = localtime(&t);
    printf("Temps d'execution : %f s\n", duree);
    printf("Fin a %02d:%02d:%02d\n", t_local->tm_hour, t_local->tm_min, t_local->tm_sec);

    getchar();
    return 0;
}