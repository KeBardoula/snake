#include <stdio.h>
#include "snake.h"

void handle_events(int *running, int *dx, int *dy, int current_dx, int current_dy) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = 0;
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    if (current_dy == 0) { // Empêche le retour arrière
                        *dx = 0;
                        *dy = -1;
                    }
                    break;
                case SDLK_DOWN:
                    if (current_dy == 0) { // Empêche le retour arrière
                        *dx = 0;
                        *dy = 1;
                    }
                    break;
                case SDLK_LEFT:
                    if (current_dx == 0) { // Empêche le retour arrière
                        *dx = -1;
                        *dy = 0;
                    }
                    break;
                case SDLK_RIGHT:
                    if (current_dx == 0) { // Empêche le retour arrière
                        *dx = 1;
                        *dy = 0;
                    }
                    break;
            }
        }
    }
}