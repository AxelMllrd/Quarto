cmake_minimum_required(VERSION 3.30)
project(Quarto C)

set(CMAKE_C_STANDARD 11)

add_executable(Quarto main.c
        quarto.h
        moteur.c
        affichage.c
        affichage.h
        precalcul.c
        canonique.c
        table.c
        tests.h
        tests.c
        solutions.c
        table.h
        solutions.h
        precalcul.h
        canonique.h
        fichiers.c
        fichiers.h)

target_link_libraries(Quarto
        SDL3
)