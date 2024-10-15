#include "snake.h"
#include <stdio.h>
#include <SDL2/SDL_ttf.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define SEGMENT_SIZE 10

void init_sdl(SDL_Window **window, SDL_Renderer **renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        exit(1);
    }
    if (TTF_Init() != 0) {
        printf("Erreur TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        exit(1);
    }
    *window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!*window) {
        printf("Erreur SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (!*renderer) {
        printf("Erreur SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        exit(1);
    }
}

void render_game(SDL_Renderer *renderer, Snake *snake, Objective *objective, int score) {
    if (!renderer || !snake || !snake->head) {
        printf("Erreur: renderer, snake ou snake->head est NULL\n");
        return;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    // Dessiner le serpent
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SnakeSegment *segment = snake->head;
    while (segment) {
        printf("Rendu du segment à la position (%d, %d)\n", segment->x, segment->y);
        SDL_Rect rect = { segment->x * SEGMENT_SIZE, segment->y * SEGMENT_SIZE, SEGMENT_SIZE, SEGMENT_SIZE };
        SDL_RenderFillRect(renderer, &rect);
        segment = segment->next;
    }

    // Dessiner l'objectif
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_Rect objective_rect = { objective->x * SEGMENT_SIZE, objective->y * SEGMENT_SIZE, SEGMENT_SIZE, SEGMENT_SIZE };
    SDL_RenderFillRect(renderer, &objective_rect);

    // Afficher le score
    TTF_Font *font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", 24);
    if (!font) {
        printf("Erreur TTF_OpenFont: %s\n", TTF_GetError());
        return;
    }
    SDL_Color color = {255, 255, 255, 255};
    char score_text[50];
    sprintf(score_text, "Score: %d", score);
    SDL_Surface *surface = TTF_RenderText_Solid(font, score_text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect text_rect = {10, 10, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &text_rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);

    SDL_RenderPresent(renderer);
}