#include "snake.h"
#include "game_mode.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 320
#define SEGMENT_SIZE 10
#define BORDER_SIZE 10

void init_snake(Snake *snake) {
    snake->head = (SnakeSegment *)malloc(sizeof(SnakeSegment));
    if (!snake->head) {
        printf("Erreur d'allocation de mémoire pour le serpent\n");
        exit(1);
    }
    snake->head->x = WINDOW_WIDTH / (2 * SEGMENT_SIZE);
    snake->head->y = WINDOW_HEIGHT / (2 * SEGMENT_SIZE);
    snake->head->next = NULL;
    snake->length = 1;
}

void move_snake(Snake *snake, int dx, int dy) {
    SnakeSegment *new_head = (SnakeSegment *)malloc(sizeof(SnakeSegment));
    if (!new_head) {
        printf("Erreur d'allocation de mémoire pour le nouveau segment du serpent\n");
        exit(1);
    }
    new_head->x = snake->head->x + dx;
    new_head->y = snake->head->y + dy;
    new_head->next = snake->head;
    snake->head = new_head;

    // Supprimer le dernier segment si le serpent n'a pas mangé d'objectif
    SnakeSegment *current = snake->head;
    while (current->next->next) {
        current = current->next;
    }
    free(current->next);
    current->next = NULL;
}

void grow_snake(Snake *snake) {
    SnakeSegment *new_segment = (SnakeSegment *)malloc(sizeof(SnakeSegment));
    if (!new_segment) {
        printf("Erreur d'allocation de mémoire pour un nouveau segment du serpent.\n");
        exit(1);
    }

    // Ajouter le nouveau segment à la fin du serpent
    SnakeSegment *current = snake->head;
    while (current->next) {
        current = current->next;
    }

    new_segment->x = current->x;
    new_segment->y = current->y;
    new_segment->next = NULL;
    current->next = new_segment;
    snake->length++;
}

void free_snake(Snake *snake) {
    SnakeSegment *current = snake->head;
    while (current) {
        SnakeSegment *next = current->next;
        free(current);
        current = next;
    }
    snake->head = NULL;
    snake->length = 0;
}

void generate_objective(Objective *objective, Snake *snake) {
    int valid_position = 0;
    while (!valid_position) {
        objective->x = (rand() % ((WINDOW_WIDTH - 2 * BORDER_SIZE) / SEGMENT_SIZE)) + (BORDER_SIZE / SEGMENT_SIZE);
        objective->y = (rand() % ((WINDOW_HEIGHT - 2 * BORDER_SIZE) / SEGMENT_SIZE)) + (BORDER_SIZE / SEGMENT_SIZE);

        // Vérifier que l'objectif n'apparaît pas sur le serpent
        valid_position = 1;
        SnakeSegment *current = snake->head;
        while (current) {
            if (current->x == objective->x && current->y == objective->y) {
                valid_position = 0;
                break;
            }
            current = current->next;
        }
    }
}