#include "snake.h"
#include <stdio.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 320
#define SEGMENT_SIZE 10
#define BORDER_SIZE 10

SDL_Texture* load_texture(SDL_Renderer *renderer, const char *file) {
    SDL_Surface *surface = IMG_Load(file);
    if (!surface) {
        printf("Erreur IMG_Load: %s\n", IMG_GetError());
        return NULL;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void init_sdl(SDL_Window **window, SDL_Renderer **renderer, SDL_Texture **textures) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        exit(1);
    }
    if (TTF_Init() != 0) {
        printf("Erreur TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        exit(1);
    }
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        printf("Erreur IMG_Init: %s\n", IMG_GetError());
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

    // Charger les textures
    textures[0] = load_texture(*renderer, "img/apple.png");
    textures[1] = load_texture(*renderer, "img/body_bottomleft.png");
    textures[2] = load_texture(*renderer, "img/body_bottomright.png");
    textures[3] = load_texture(*renderer, "img/body_horizontal.png");
    textures[4] = load_texture(*renderer, "img/body_vertical.png");
    textures[5] = load_texture(*renderer, "img/body_topleft.png");
    textures[6] = load_texture(*renderer, "img/body_topright.png");
    textures[7] = load_texture(*renderer, "img/head_up.png");
    textures[8] = load_texture(*renderer, "img/head_down.png");
    textures[9] = load_texture(*renderer, "img/head_left.png");
    textures[10] = load_texture(*renderer, "img/head_right.png");
    textures[11] = load_texture(*renderer, "img/tail_up.png");
    textures[12] = load_texture(*renderer, "img/tail_down.png");
    textures[13] = load_texture(*renderer, "img/tail_left.png");
    textures[14] = load_texture(*renderer, "img/tail_right.png");

    for (int i = 0; i < 15; i++) {
        if (!textures[i]) {
            printf("Erreur de chargement de la texture %d\n", i);
            SDL_Quit();
            exit(1);
        }
    }
}

void render_game(SDL_Renderer *renderer, Snake *snake, Objective *objective, int score, SDL_Texture **textures, int dx, int dy) {
    if (!renderer || !snake || !snake->head) {
        printf("Erreur: renderer, snake ou snake->head est NULL\n");
        return;
    }

    // Dessiner le quadrillage
    for (int y = 0; y < WINDOW_HEIGHT / SEGMENT_SIZE; y++) {
        for (int x = 0; x < WINDOW_WIDTH / SEGMENT_SIZE; x++) {
            if ((x + y) % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255); // Vert foncé
            } else {
                SDL_SetRenderDrawColor(renderer, 64, 189, 64, 255); // Vert plus clair
            }
            SDL_Rect rect = { x * SEGMENT_SIZE, y * SEGMENT_SIZE, SEGMENT_SIZE, SEGMENT_SIZE };
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    // Dessiner la bordure
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect top_border = {0, 0, WINDOW_WIDTH, BORDER_SIZE};
    SDL_Rect bottom_border = {0, WINDOW_HEIGHT - BORDER_SIZE, WINDOW_WIDTH, BORDER_SIZE};
    SDL_Rect left_border = {0, 0, BORDER_SIZE, WINDOW_HEIGHT};
    SDL_Rect right_border = {WINDOW_WIDTH - BORDER_SIZE, 0, BORDER_SIZE, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer, &top_border);
    SDL_RenderFillRect(renderer, &bottom_border);
    SDL_RenderFillRect(renderer, &left_border);
    SDL_RenderFillRect(renderer, &right_border);

    // Dessiner le serpent
    SnakeSegment *segment = snake->head;
    int is_head = 1;
    while (segment) {
        SDL_Rect rect = { segment->x * SEGMENT_SIZE, segment->y * SEGMENT_SIZE, SEGMENT_SIZE, SEGMENT_SIZE };
        if (is_head) {
            if (dx == 1) {
                SDL_RenderCopy(renderer, textures[10], NULL, &rect); // head_right.png
            } else if (dx == -1) {
                SDL_RenderCopy(renderer, textures[9], NULL, &rect); // head_left.png
            } else if (dy == 1) {
                SDL_RenderCopy(renderer, textures[8], NULL, &rect); // head_down.png
            } else if (dy == -1) {
                SDL_RenderCopy(renderer, textures[7], NULL, &rect); // head_up.png
            }
            is_head = 0;
        } else if (segment->next == NULL) {
            // Dernier segment (queue)
            SnakeSegment *prev = snake->head;
            while (prev->next != segment) {
                prev = prev->next;
            }
            if (prev->x < segment->x) {
                SDL_RenderCopy(renderer, textures[14], NULL, &rect); // tail_right.png
            } else if (prev->x > segment->x) {
                SDL_RenderCopy(renderer, textures[13], NULL, &rect); // tail_left.png
            } else if (prev->y < segment->y) {
                SDL_RenderCopy(renderer, textures[12], NULL, &rect); // tail_down.png
            } else if (prev->y > segment->y) {
                SDL_RenderCopy(renderer, textures[11], NULL, &rect); // tail_up.png
            }
        } else {
            // Vérifier les virages
            SnakeSegment *next = segment->next;
            SnakeSegment *prev = snake->head;
            while (prev->next != segment) {
                prev = prev->next;
            }

            if ((prev->x < segment->x && next->y > segment->y) || (next->x < segment->x && prev->y > segment->y)) {
                SDL_RenderCopy(renderer, textures[1], NULL, &rect); // body_bottomleft.png
            } else if ((prev->x > segment->x && next->y > segment->y) || (next->x > segment->x && prev->y > segment->y)) {
                SDL_RenderCopy(renderer, textures[2], NULL, &rect); // body_bottomright.png
            } else if ((prev->x < segment->x && next->y < segment->y) || (next->x < segment->x && prev->y < segment->y)) {
                SDL_RenderCopy(renderer, textures[5], NULL, &rect); // body_topleft.png
            } else if ((prev->x > segment->x && next->y < segment->y) || (next->x > segment->x && prev->y < segment->y)) {
                SDL_RenderCopy(renderer, textures[6], NULL, &rect); // body_topright.png
            } else if (segment->x == next->x) {
                SDL_RenderCopy(renderer, textures[4], NULL, &rect); // body_vertical.png
            } else {
                SDL_RenderCopy(renderer, textures[3], NULL, &rect); // body_horizontal.png
            }
        }
        segment = segment->next;
    }

    // Dessiner l'objectif
    SDL_Rect objective_rect = { objective->x * SEGMENT_SIZE, objective->y * SEGMENT_SIZE, SEGMENT_SIZE, SEGMENT_SIZE };
    SDL_RenderCopy(renderer, textures[0], NULL, &objective_rect); // Utiliser la texture de l'objectif

    // Afficher le score avec une taille de police plus petite
    TTF_Font *font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", 16); // Taille de police réduite
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