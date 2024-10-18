#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "snake.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>  // Ajout de l'inclusion de SDL_image.h

#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 320
#define SEGMENT_SIZE 10
#define BORDER_SIZE 10

int check_self_collision(Snake *snake) {
    SnakeSegment *head = snake->head;
    SnakeSegment *current = head->next;
    while (current) {
        if (head->x == current->x && head->y == current->y) {
            return 1; // Collision détectée
        }
        current = current->next;
    }
    return 0; // Pas de collision
}

int show_game_over_dialog(SDL_Renderer *renderer) {
    const int dialog_width = 200;
    const int dialog_height = 100;
    const int button_width = 60;
    const int button_height = 25;

    SDL_Rect dialog_rect = { (WINDOW_WIDTH - dialog_width) / 2, (WINDOW_HEIGHT - dialog_height) / 2, dialog_width, dialog_height };
    SDL_Rect retry_button_rect = { dialog_rect.x + 10, dialog_rect.y + dialog_height - button_height - 10, button_width, button_height };
    SDL_Rect quit_button_rect = { dialog_rect.x + dialog_width - button_width - 10, dialog_rect.y + dialog_height - button_height - 10, button_width, button_height };

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200); // Fond semi-transparent
    SDL_RenderFillRect(renderer, &dialog_rect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Bordure blanche
    SDL_RenderDrawRect(renderer, &dialog_rect);

    TTF_Font *font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", 16);
    if (!font) {
        printf("Erreur TTF_OpenFont: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface *retry_surface = TTF_RenderText_Solid(font, "Recommencer", white);
    SDL_Surface *quit_surface = TTF_RenderText_Solid(font, "Quitter", white);

    SDL_Texture *retry_texture = SDL_CreateTextureFromSurface(renderer, retry_surface);
    SDL_Texture *quit_texture = SDL_CreateTextureFromSurface(renderer, quit_surface);

    SDL_FreeSurface(retry_surface);
    SDL_FreeSurface(quit_surface);

    SDL_RenderCopy(renderer, retry_texture, NULL, &retry_button_rect);
    SDL_RenderCopy(renderer, quit_texture, NULL, &quit_button_rect);

    SDL_DestroyTexture(retry_texture);
    SDL_DestroyTexture(quit_texture);
    TTF_CloseFont(font);

    SDL_RenderPresent(renderer);

    SDL_Event e;
    while (SDL_WaitEvent(&e)) {
        if (e.type == SDL_QUIT) {
            return 1;
        } else if (e.type == SDL_MOUSEBUTTONDOWN) {
            int x = e.button.x;
            int y = e.button.y;
            if (x >= retry_button_rect.x && x <= retry_button_rect.x + retry_button_rect.w &&
                y >= retry_button_rect.y && y <= retry_button_rect.y + retry_button_rect.h) {
                return 0; // Recommencer
            } else if (x >= quit_button_rect.x && x <= quit_button_rect.x + quit_button_rect.w &&
                       y >= quit_button_rect.y && y <= quit_button_rect.y + quit_button_rect.h) {
                return 1; // Quitter
            }
        }
    }
    return 1;
}

int main() {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *textures[15];
    Snake snake;
    Objective objective;
    int running = 1, dx = 0, dy = 0, next_dx = 0, next_dy = 0, score = 0;

    srand(time(NULL));

    printf("Initialisation de SDL...\n");
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    printf("Initialisation de SDL_ttf...\n");
    if (TTF_Init() != 0) {
        printf("Erreur TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    printf("Initialisation de SDL_image...\n");
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        printf("Erreur IMG_Init: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    printf("Création de la fenêtre...\n");
    window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) {
        printf("Erreur SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    printf("Création du renderer...\n");
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    printf("Chargement des textures...\n");
    init_sdl(&window, &renderer, textures);

    printf("Initialisation du serpent...\n");
    init_snake(&snake);
    generate_objective(&objective, &snake);

    while (running) {
        handle_events(&running, &next_dx, &next_dy, dx, dy);

        // Mettre à jour la direction du serpent uniquement si le joueur a fait un mouvement
        if (next_dx != 0 || next_dy != 0) {
            dx = next_dx;
            dy = next_dy;
            move_snake(&snake, dx, dy);
        }

        // Vérification des bordures
        if (snake.head->x < BORDER_SIZE / SEGMENT_SIZE || snake.head->x >= (WINDOW_WIDTH - BORDER_SIZE) / SEGMENT_SIZE ||
            snake.head->y < BORDER_SIZE / SEGMENT_SIZE || snake.head->y >= (WINDOW_HEIGHT - BORDER_SIZE) / SEGMENT_SIZE) {
            printf("Le serpent a touché la bordure. Fin du jeu.\n");
            running = 0;
        }

        // Vérification des collisions avec soi-même
        if (check_self_collision(&snake)) {
            printf("Le serpent s'est touché lui-même. Fin du jeu.\n");
            running = 0;
        }

        // Vérification si le serpent mange l'objectif
        if (snake.head->x == objective.x && snake.head->y == objective.y) {
            grow_snake(&snake);
            score++;
            generate_objective(&objective, &snake);
        }

        render_game(renderer, &snake, &objective, score, textures, dx, dy);
        SDL_Delay(150);

        if (!running) {
            int result = show_game_over_dialog(renderer);
            if (result == 0) {
                // Recommencer le jeu
                free_snake(&snake);
                init_snake(&snake);
                generate_objective(&objective, &snake);
                running = 1;
                dx = 0;
                dy = 0;
                next_dx = 0;
                next_dy = 0;
                score = 0;
            } else {
                break;
            }
        }
    }

    free_snake(&snake);
    for (int i = 0; i < 15; i++) {
        SDL_DestroyTexture(textures[i]);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}