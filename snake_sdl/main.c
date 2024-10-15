#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "snake.h"
#include <SDL2/SDL_ttf.h>  // Ajout de l'inclusion de SDL_ttf.h

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define SEGMENT_SIZE 10

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

int show_game_over_dialog(SDL_Window *window) {
    const SDL_MessageBoxButtonData buttons[] = {
        { /* .flags, .buttonid, .text */        0, 0, "Recommencer" },
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Quitter" },
    };
    const SDL_MessageBoxColorScheme colorScheme = {
        { /* .colors (.r, .g, .b) */
            /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
            { 255, 0, 0 },
            /* [SDL_MESSAGEBOX_COLOR_TEXT] */
            { 0, 255, 0 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
            { 255, 255, 0 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
            { 0, 0, 255 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
            { 255, 0, 255 }
        }
    };
    const SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION, /* .flags */
        window, /* .window */
        "Game-Over", /* .title */
        "Le serpent a touché la bordure ou s'est touché lui-même. Que voulez-vous faire ?", /* .message */
        SDL_arraysize(buttons), /* .numbuttons */
        buttons, /* .buttons */
        &colorScheme /* .colorScheme */
    };
    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
        SDL_Log("Erreur lors de l'affichage de la boîte de dialogue : %s", SDL_GetError());
        return 1; // Quitter par défaut en cas d'erreur
    }
    return buttonid;
}

int main() {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    Snake snake;
    Objective objective;
    int running = 1, dx = 1, dy = 0, score = 0;

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

    printf("Initialisation du serpent...\n");
    init_snake(&snake);
    generate_objective(&objective, &snake);

    while (running) {
        handle_events(&running, &dx, &dy, dx, dy);
        move_snake(&snake, dx, dy);

        // Vérification des bordures
        if (snake.head->x < 0 || snake.head->x >= WINDOW_WIDTH / SEGMENT_SIZE ||
            snake.head->y < 0 || snake.head->y >= WINDOW_HEIGHT / SEGMENT_SIZE) {
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

        render_game(renderer, &snake, &objective, score);
        SDL_Delay(100);

        if (!running) {
            int result = show_game_over_dialog(window);
            if (result == 0) {
                // Recommencer le jeu
                printf("Recommencer le jeu.\n");
                free_snake(&snake);
                init_snake(&snake);
                generate_objective(&objective, &snake);
                running = 1;
                dx = 1;
                dy = 0;
                score = 0;
            } else {
                // Quitter le jeu
                printf("Quitter le jeu.\n");
                break;
            }
        }
    }

    printf("Libération des ressources...\n");
    free_snake(&snake);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    printf("Fin du programme.\n");
    return 0;
}