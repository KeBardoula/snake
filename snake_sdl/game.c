#include <stdio.h>
#include "snake.h"

void handle_events(int *running, int *next_dx, int *next_dy, int current_dx, int current_dy) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = 0;
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    if (current_dy == 0) { // Empêche le retour arrière
                        *next_dx = 0;
                        *next_dy = -1;
                    }
                    break;
                case SDLK_DOWN:
                    if (current_dy == 0) { // Empêche le retour arrière
                        *next_dx = 0;
                        *next_dy = 1;
                    }
                    break;
                case SDLK_LEFT:
                    if (current_dx == 0) { // Empêche le retour arrière
                        *next_dx = -1;
                        *next_dy = 0;
                    }
                    break;
                case SDLK_RIGHT:
                    if (current_dx == 0) { // Empêche le retour arrière
                        *next_dx = 1;
                        *next_dy = 0;
                    }
                    break;
            }
        }
    }
}

void update_snake(Snake *snake, int *dx, int *dy, int *next_dx, int *next_dy) {
    if (*next_dx != *dx || *next_dy != *dy) {
        *dx = *next_dx;
        *dy = *next_dy;

        // Mettre à jour la position du serpent
        SnakeSegment *new_head = malloc(sizeof(SnakeSegment));
        new_head->x = snake->head->x + *dx;
        new_head->y = snake->head->y + *dy;
        new_head->next = snake->head;
        snake->head = new_head;

        // Supprimer le dernier segment du serpent
        SnakeSegment *current = snake->head;
        while (current->next->next != NULL) {
            current = current->next;
        }
        free(current->next);
        current->next = NULL;
    }
}