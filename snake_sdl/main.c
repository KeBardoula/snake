#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "snake.h"
#include "game_mode.h"
#include "main.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>

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

int show_game_over_dialog(SDL_Window *window, SDL_Renderer *renderer, const char *message) {
    const int dialog_width = 300;
    const int dialog_height = 200;
    const int button_width = 100;
    const int button_height = 30;

    SDL_Rect dialog_rect = { (WINDOW_WIDTH - dialog_width) / 2, (WINDOW_HEIGHT - dialog_height) / 2, dialog_width, dialog_height };
    SDL_Rect retry_button_rect = { dialog_rect.x + 10, dialog_rect.y + dialog_height - button_height - 10, button_width, button_height };
    SDL_Rect quit_button_rect = { dialog_rect.x + dialog_width - button_width - 10, dialog_rect.y + dialog_height - button_height - 10, button_width, button_height };

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200); // Fond semi-transparent
    SDL_RenderFillRect(renderer, &dialog_rect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Bordure blanche
    SDL_RenderDrawRect(renderer, &dialog_rect);

    // Charger l'image game_over.png
    SDL_Surface *game_over_surface = IMG_Load("img/game_over.png");
    if (!game_over_surface) {
        printf("Erreur IMG_Load: %s\n", IMG_GetError());
        return 1;
    }
    SDL_Texture *game_over_texture = SDL_CreateTextureFromSurface(renderer, game_over_surface);
    SDL_FreeSurface(game_over_surface);

    // Centrer l'image game_over.png
    int game_over_width, game_over_height;
    SDL_QueryTexture(game_over_texture, NULL, NULL, &game_over_width, &game_over_height);
    SDL_Rect game_over_rect = { dialog_rect.x + (dialog_width - game_over_width) / 2, dialog_rect.y + 10, game_over_width, game_over_height };

    TTF_Font *font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", 16);
    if (!font) {
        printf("Erreur TTF_OpenFont: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface *message_surface = TTF_RenderText_Solid(font, message, white);
    SDL_Surface *retry_surface = TTF_RenderText_Solid(font, "Recommencer", white);
    SDL_Surface *quit_surface = TTF_RenderText_Solid(font, "Quitter", white);

    SDL_Texture *message_texture = SDL_CreateTextureFromSurface(renderer, message_surface);
    SDL_Texture *retry_texture = SDL_CreateTextureFromSurface(renderer, retry_surface);
    SDL_Texture *quit_texture = SDL_CreateTextureFromSurface(renderer, quit_surface);

    SDL_FreeSurface(message_surface);
    SDL_FreeSurface(retry_surface);
    SDL_FreeSurface(quit_surface);

    SDL_Rect message_rect = { dialog_rect.x + 10, dialog_rect.y + game_over_height + 20, dialog_width - 20, 30 };
    SDL_RenderCopy(renderer, game_over_texture, NULL, &game_over_rect);
    SDL_RenderCopy(renderer, message_texture, NULL, &message_rect);
    SDL_RenderCopy(renderer, retry_texture, NULL, &retry_button_rect);
    SDL_RenderCopy(renderer, quit_texture, NULL, &quit_button_rect);

    SDL_DestroyTexture(game_over_texture);
    SDL_DestroyTexture(message_texture);
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

void show_main_menu(SDL_Renderer *renderer) {
    const int button_width = 200;
    const int button_height = 50;

    SDL_Rect play_button_rect = { (WINDOW_WIDTH - button_width) / 2, (WINDOW_HEIGHT - button_height) / 2, button_width, button_height };

    TTF_Font *font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", 24);
    if (!font) {
        printf("Erreur TTF_OpenFont: %s\n", TTF_GetError());
        return;
    }

    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface *play_surface = TTF_RenderText_Solid(font, "Jouer", white);
    SDL_Texture *play_texture = SDL_CreateTextureFromSurface(renderer, play_surface);

    SDL_FreeSurface(play_surface);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, play_texture, NULL, &play_button_rect);

    SDL_DestroyTexture(play_texture);
    TTF_CloseFont(font);

    SDL_RenderPresent(renderer);

    SDL_Event e;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
                exit(0);
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;
                if (x >= play_button_rect.x && x <= play_button_rect.x + play_button_rect.w &&
                    y >= play_button_rect.y && y <= play_button_rect.y + play_button_rect.h) {
                    running = 0;
                }
            }
        }
    }
}

void show_game_mode_menu(SDL_Renderer *renderer, SDL_Texture **textures) {
    const int button_width = 200;
    const int button_height = 50;

    SDL_Rect classic_button_rect = { (WINDOW_WIDTH - button_width) / 2, (WINDOW_HEIGHT - button_height) / 2 - 60, button_width, button_height };
    SDL_Rect time_attack_button_rect = { (WINDOW_WIDTH - button_width) / 2, (WINDOW_HEIGHT - button_height) / 2, button_width, button_height };
    SDL_Rect endless_button_rect = { (WINDOW_WIDTH - button_width) / 2, (WINDOW_HEIGHT - button_height) / 2 + 60, button_width, button_height };

    TTF_Font *font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", 24);
    if (!font) {
        printf("Erreur TTF_OpenFont: %s\n", TTF_GetError());
        return;
    }

    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface *classic_surface = TTF_RenderText_Solid(font, "Classique", white);
    SDL_Surface *time_attack_surface = TTF_RenderText_Solid(font, "Time Attack", white);
    SDL_Surface *endless_surface = TTF_RenderText_Solid(font, "Endless", white);

    SDL_Texture *classic_texture = SDL_CreateTextureFromSurface(renderer, classic_surface);
    SDL_Texture *time_attack_texture = SDL_CreateTextureFromSurface(renderer, time_attack_surface);
    SDL_Texture *endless_texture = SDL_CreateTextureFromSurface(renderer, endless_surface);

    SDL_FreeSurface(classic_surface);
    SDL_FreeSurface(time_attack_surface);
    SDL_FreeSurface(endless_surface);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, classic_texture, NULL, &classic_button_rect);
    SDL_RenderCopy(renderer, time_attack_texture, NULL, &time_attack_button_rect);
    SDL_RenderCopy(renderer, endless_texture, NULL, &endless_button_rect);

    SDL_DestroyTexture(classic_texture);
    SDL_DestroyTexture(time_attack_texture);
    SDL_DestroyTexture(endless_texture);
    TTF_CloseFont(font);

    SDL_RenderPresent(renderer);

    SDL_Event e;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
                exit(0);
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;
                if (x >= classic_button_rect.x && x <= classic_button_rect.x + classic_button_rect.w &&
                    y >= classic_button_rect.y && y <= classic_button_rect.y + classic_button_rect.h) {
                    classic_mode(renderer, textures);
                    running = 0;
                } else if (x >= time_attack_button_rect.x && x <= time_attack_button_rect.x + time_attack_button_rect.w &&
                           y >= time_attack_button_rect.y && y <= time_attack_button_rect.y + time_attack_button_rect.h) {
                    time_attack_mode(renderer, textures);
                    running = 0;
                } else if (x >= endless_button_rect.x && x <= endless_button_rect.x + endless_button_rect.w &&
                           y >= endless_button_rect.y && y <= endless_button_rect.y + endless_button_rect.h) {
                    endless_mode(renderer, textures);
                    running = 0;
                }
            }
        }
    }
}

int main() {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *textures[15];

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

    // Afficher le menu principal
    show_main_menu(renderer);

    // Afficher le menu de sélection du mode de jeu
    show_game_mode_menu(renderer, textures);

    // Libérer les ressources
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