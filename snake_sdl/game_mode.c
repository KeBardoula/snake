#include "game_mode.h"
#include "snake.h"
#include "main.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <time.h>

#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 320
#define SEGMENT_SIZE 10
#define BORDER_SIZE 10
#define TIME_LIMIT 60
#define OBJECTIVE_TIMER 10

void classic_mode(SDL_Renderer *renderer, SDL_Texture **textures) {
    Snake snake;
    Objective objective;
    int running = 1, dx = 0, dy = 0, next_dx = 0, next_dy = 0, score = 0;

    srand(time(NULL));

    printf("Initialisation du serpent...\n");
    init_snake(&snake);
    generate_objective(&objective, &snake);

    // Vérifier les positions initiales
    printf("Position initiale du serpent: (%d, %d)\n", snake.head->x, snake.head->y);
    printf("Position initiale de l'objectif: (%d, %d)\n", objective.x, objective.y);

    // Calculer la taille maximale du serpent
    int max_snake_length = (WINDOW_WIDTH / SEGMENT_SIZE) * (WINDOW_HEIGHT / SEGMENT_SIZE);

    while (running) {
        handle_events(&running, &next_dx, &next_dy, dx, dy);

        // Mettre à jour la direction du serpent uniquement si le joueur a fait un mouvement
        if (next_dx != 0 || next_dy != 0) {
            dx = next_dx;
            dy = next_dy;
            move_snake(&snake, dx, dy);
        }

        // Vérification des bordures
        if ((snake.head->x < BORDER_SIZE / SEGMENT_SIZE || snake.head->x >= (WINDOW_WIDTH - BORDER_SIZE) / SEGMENT_SIZE) ||
            (snake.head->y < BORDER_SIZE / SEGMENT_SIZE || snake.head->y >= (WINDOW_HEIGHT - BORDER_SIZE) / SEGMENT_SIZE)) {
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

        // Vérification de la condition de victoire
        if (snake.length == max_snake_length) {
            printf("Le serpent a atteint la taille maximale. Vous avez gagné !\n");
            running = 0;
            show_game_over_dialog(NULL, renderer, "Vous avez gagné !");
        }

        render_game(renderer, &snake, &objective, score, textures, dx, dy);
        SDL_Delay(100);

        if (!running) {
            int result = show_game_over_dialog(NULL, renderer, "Game Over");
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
}

void time_attack_mode(SDL_Renderer *renderer, SDL_Texture **textures) {
    Snake snake;
    Objective objective;
    int running = 1, dx = 0, dy = 0, next_dx = 0, next_dy = 0, score = 0;
    time_t start_time, current_time, objective_start_time;

    srand(time(NULL));

    printf("Initialisation du serpent...\n");
    init_snake(&snake);
    generate_objective(&objective, &snake);

    // Vérifier les positions initiales
    printf("Position initiale du serpent: (%d, %d)\n", snake.head->x, snake.head->y);
    printf("Position initiale de l'objectif: (%d, %d)\n", objective.x, objective.y);

    // Calculer la taille maximale du serpent
    int max_snake_length = (WINDOW_WIDTH / SEGMENT_SIZE) * (WINDOW_HEIGHT / SEGMENT_SIZE);

    // Obtenir le temps de départ
    start_time = time(NULL);
    objective_start_time = start_time;

    while (running) {
        handle_events(&running, &next_dx, &next_dy, dx, dy);

        // Mettre à jour la direction du serpent uniquement si le joueur a fait un mouvement
        if (next_dx != 0 || next_dy != 0) {
            dx = next_dx;
            dy = next_dy;
            move_snake(&snake, dx, dy);
        }

        // Vérification des bordures
        if ((snake.head->x < BORDER_SIZE / SEGMENT_SIZE || snake.head->x >= (WINDOW_WIDTH - BORDER_SIZE) / SEGMENT_SIZE) ||
            (snake.head->y < BORDER_SIZE / SEGMENT_SIZE || snake.head->y >= (WINDOW_HEIGHT - BORDER_SIZE) / SEGMENT_SIZE)) {
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
            objective_start_time = time(NULL); // Réinitialiser le timer de l'objectif
            generate_objective(&objective, &snake);
        }

        // Vérification de la condition de victoire
        if (snake.length == max_snake_length) {
            printf("Le serpent a atteint la taille maximale. Vous avez gagné !\n");
            running = 0;
            show_game_over_dialog(NULL, renderer, "Vous avez gagné !");
        }

        // Vérification de la limite de temps
        current_time = time(NULL);
        if (difftime(current_time, start_time) >= TIME_LIMIT) {
            printf("Temps écoulé. Fin du jeu.\n");
            running = 0;
            char message[50];
            sprintf(message, "Temps écoulé ! Score: %d", score);
            show_game_over_dialog(NULL, renderer, message);
        }

        // Calculer le temps restant pour l'objectif
        int objective_time_left = OBJECTIVE_TIMER - difftime(current_time, objective_start_time);
        if (objective_time_left < 0) {
            objective_time_left = 0;
        }

        // Afficher le jeu et le timer
        render_game(renderer, &snake, &objective, score, textures, dx, dy);

        // Afficher le timer en haut à droite
        TTF_Font *font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", 16);
        if (!font) {
            printf("Erreur TTF_OpenFont: %s\n", TTF_GetError());
            return;
        }
        SDL_Color color = {255, 255, 255, 255};
        char timer_text[50];
        sprintf(timer_text, "Temps: %d", objective_time_left);
        SDL_Surface *surface = TTF_RenderText_Solid(font, timer_text, color);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect text_rect = {WINDOW_WIDTH - surface->w - 10, 10, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &text_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        TTF_CloseFont(font);

        SDL_RenderPresent(renderer);
        SDL_Delay(100);

        if (!running) {
            int result = show_game_over_dialog(NULL, renderer, "Game Over");
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
                start_time = time(NULL); // Réinitialiser le temps de départ
                objective_start_time = start_time; // Réinitialiser le timer de l'objectif
            } else {
                break;
            }
        }
    }

    free_snake(&snake);
}

void endless_mode(SDL_Renderer *renderer, SDL_Texture **textures) {
    Snake snake;
    Objective objective;
    int running = 1, dx = 0, dy = 0, next_dx = 0, next_dy = 0, score = 0;

    srand(time(NULL));

    printf("Initialisation du serpent...\n");
    init_snake(&snake);
    generate_objective(&objective, &snake);

    // Vérifier les positions initiales
    printf("Position initiale du serpent: (%d, %d)\n", snake.head->x, snake.head->y);
    printf("Position initiale de l'objectif: (%d, %d)\n", objective.x, objective.y);

    // Calculer la taille maximale du serpent
    int max_snake_length = (WINDOW_WIDTH / SEGMENT_SIZE) * (WINDOW_HEIGHT / SEGMENT_SIZE);

    while (running) {
        handle_events(&running, &next_dx, &next_dy, dx, dy);

        // Mettre à jour la direction du serpent uniquement si le joueur a fait un mouvement
        if (next_dx != 0 || next_dy != 0) {
            dx = next_dx;
            dy = next_dy;
            move_snake(&snake, dx, dy);
        }

        // Vérification des bordures
        if ((snake.head->x < BORDER_SIZE / SEGMENT_SIZE || snake.head->x >= (WINDOW_WIDTH - BORDER_SIZE) / SEGMENT_SIZE) ||
            (snake.head->y < BORDER_SIZE / SEGMENT_SIZE || snake.head->y >= (WINDOW_HEIGHT - BORDER_SIZE) / SEGMENT_SIZE)) {
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
            if (score < 4) {
                grow_snake(&snake);
            }
            score++;
            generate_objective(&objective, &snake);
        }

        // Vérification de la condition de victoire
        if (snake.length == max_snake_length) {
            printf("Le serpent a atteint la taille maximale. Vous avez gagné !\n");
            running = 0;
            show_game_over_dialog(NULL, renderer, "Vous avez gagné !");
        }

        render_game(renderer, &snake, &objective, score, textures, dx, dy);
        SDL_Delay(100);

        if (!running) {
            int result = show_game_over_dialog(NULL, renderer, "Game Over");
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
}