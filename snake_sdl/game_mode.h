#ifndef GAME_MODE_H
#define GAME_MODE_H

#include <SDL2/SDL.h>

// Déclaration des fonctions pour les différents modes de jeu
void classic_mode(SDL_Renderer *renderer, SDL_Texture **textures);
void time_attack_mode(SDL_Renderer *renderer, SDL_Texture **textures);
void endless_mode(SDL_Renderer *renderer, SDL_Texture **textures);

#endif // GAME_MODES_H