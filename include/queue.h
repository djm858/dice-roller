#pragma once

#include <stdlib.h>
#include <stddef.h>

typedef struct Queue {
	size_t count;
	size_t capacity;
	char **data;
} Queue;

void queue_free(struct Queue *queue);
char *queue_pop(struct Queue *queue);
void queue_push(struct Queue *queue, char *obj);
struct Queue *queue_new(size_t capacity);
