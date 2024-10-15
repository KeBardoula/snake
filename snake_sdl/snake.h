#ifndef SNAKE_H
#define SNAKE_H

#include <SDL2/SDL.h>

typedef struct SnakeSegment {
    int x, y;
    struct SnakeSegment *next;
} SnakeSegment;

typedef struct {
    SnakeSegment *head;
    SnakeSegment *tail;
    int length;
    int grow;
} Snake;

typedef struct {
    int x, y;
} Objective;

void init_snake(Snake *snake);
void move_snake(Snake *snake, int dx, int dy);
void grow_snake(Snake *snake);
void free_snake(Snake *snake);
void handle_events(int *running, int *dx, int *dy, int current_dx, int current_dy);
void render_game(SDL_Renderer *renderer, Snake *snake, Objective *objective, int score);
void init_sdl(SDL_Window **window, SDL_Renderer **renderer);
void generate_objective(Objective *objective, Snake *snake);

#endif