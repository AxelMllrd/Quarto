#include <stdio.h>
#include <SDL3/SDL.h>

#include "affichage.h"
#include "quarto.h"

void aff_octet(Octet o) {
    char str[8];
    for (int i = 0; i < 8; i++) str[7 - i] = (char)('0' + bit(o, i));
    printf("%s", str);
}

void aff_bitboard(Bitboard bitboard) {
    for (int i = 3; i >= 0; i--) {
        for (int j = 3; j >= 0; j--) {
            printf("%d ", bit(bitboard, 4 * i + j));
        }
        printf("\n");
    }
}

void aff_plateau(const Plateau *plateau) {
    const Bitboard *cara = plateau->caracteristique;
    const Bitboard placements = plateau->placements;

    printf("    A    B    C    D");
    for (int i = 3; i >= 0; i--) {
        printf("\n%d  ", 4 - i);
        for (int j = 3; j >= 0; j--) {
            int shift = 4 * i + j;
            if (!bit(placements, shift)) printf("---- ");
            else printf("%d%d%d%d ", bit(cara[3], shift), bit(cara[2], shift), bit(cara[1], shift), bit(cara[0], shift));
        }
    }
    printf("\n");
}

//--- SDL ---//
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int init_quarto() {
    SDL_Init(SDL_INIT_VIDEO);
    if (SDL_CreateWindowAndRenderer("Quarto", 800, 400, 0, &window, &renderer))
        return -1;
    return 0;
}

void destroy_quarto() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void dessiner_piece(Octet piece, float x, float y) {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_FRect rect = {x, y, 60, 60};
    SDL_FRect cara = {x + 5, y + 5, 20, 20};
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
    for (int i = 0; i < 4; i++) {
        if (bit(piece, i)) SDL_RenderFillRect(renderer, &cara);
        cara.x += 30;
        if (i == 1) {
            cara.y += 30;
            cara.x = x + 5;
        }
    }
}

void frame() {
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_RenderLine(renderer, 400, 15, 400, 385);
    SDL_FRect rect = {10, 10, 80, 80};
    for (int i = 0; i < 4; i++) {
        rect.x = 10;
        for (int j = 0; j < 4; j++) {
            SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
            SDL_RenderFillRect(renderer, &rect);
            dessiner_piece(4 * i + j, 420 + 100 * j, 20 + 100 * i);
            rect.x += 100;
        }
        rect.y += 100;
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(16);
}