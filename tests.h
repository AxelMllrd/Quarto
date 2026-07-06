#ifndef TESTS_H
#define TESTS_H

typedef struct Plateau Plateau;

void start_timer();
double get_time();

Plateau rand_plateau();
void test_hachage();
void test_table();
void test_solution_coup(int nb_coups);
void test_trier();

#endif //TESTS_H
