#include "snake.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define SEGMENT_SIZE 10
#define BORDER_SIZE 10

void init_snake(Snake *snake) {
    snake->head = malloc(sizeof(SnakeSegment));
    if (!snake->head) {
        printf("Erreur d'allocation mémoire pour le serpent\n");
        exit(1);
    }
    // Initialiser le serpent à une position aléatoire à l'intérieur des bordures
    snake->head->x = (rand() % ((WINDOW_WIDTH - 2 * BORDER_SIZE) / SEGMENT_SIZE)) + BORDER_SIZE / SEGMENT_SIZE;
    snake->head->y = (rand() % ((WINDOW_HEIGHT - 2 * BORDER_SIZE) / SEGMENT_SIZE)) + BORDER_SIZE / SEGMENT_SIZE;
    snake->head->next = NULL;
    snake->tail = snake->head;
    snake->length = 1;
    snake->grow = 0; // Initialisation du membre grow
    printf("Serpent initialisé avec succès à la position (%d, %d).\n", snake->head->x, snake->head->y);
}

void move_snake(Snake *snake, int dx, int dy) {
    if (!snake || !snake->head) {
        printf("Erreur: snake ou snake->head est NULL\n");
        return;
    }

    SnakeSegment *new_head = malloc(sizeof(SnakeSegment));
    if (!new_head) {
        printf("Erreur: allocation de mémoire pour new_head a échoué\n");
        return;
    }

    new_head->x = snake->head->x + dx;
    new_head->y = snake->head->y + dy;
    new_head->next = snake->head;
    snake->head = new_head;

    // Supprimer le dernier segment si le serpent n'a pas mangé d'objectif
    if (!snake->grow) {
        SnakeSegment *current = snake->head;
        while (current->next && current->next->next) {
            current = current->next;
        }
        free(current->next);
        current->next = NULL;
    } else {
        snake->grow = 0;
    }
}

void grow_snake(Snake *snake) {
    printf("Croissance du serpent...\n");
    snake->grow = 1;
}

void free_snake(Snake *snake) {
    SnakeSegment *current = snake->head;
    while (current) {
        SnakeSegment *next = current->next;
        free(current);
        current = next;
    }
    snake->head = NULL;
    snake->tail = NULL;
    snake->length = 0;
    printf("Mémoire du serpent libérée.\n");
}

void generate_objective(Objective *objective, Snake *snake) {
    int valid_position = 0;
    while (!valid_position) {
        valid_position = 1;
        objective->x = (rand() % ((WINDOW_WIDTH - 2 * BORDER_SIZE) / SEGMENT_SIZE)) + BORDER_SIZE / SEGMENT_SIZE;
        objective->y = (rand() % ((WINDOW_HEIGHT - 2 * BORDER_SIZE) / SEGMENT_SIZE)) + BORDER_SIZE / SEGMENT_SIZE;

        SnakeSegment *current = snake->head;
        while (current) {
            if (current->x == objective->x && current->y == objective->y) {
                valid_position = 0;
                break;
            }
            current = current->next;
        }
    }
    printf("Objectif généré à la position (%d, %d).\n", objective->x, objective->y);
}