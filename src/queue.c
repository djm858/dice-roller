#include "queue.h"

void queue_free(struct Queue *queue)
{
	if (queue == NULL) {
		return;
	}

	if (queue->data != NULL) {
		free(queue->data);
	}

	free(queue);
}

char *queue_pop(struct Queue *queue)
{
	char *ptr_to_pop;
	size_t i;

	if (queue->count == 0) {
		return NULL;
	}

	ptr_to_pop = queue->data[0];
	for (i = 1; i < queue->count; i++) {
		queue->data[i - 1] = queue->data[i];
	}

	queue->count--;
	return ptr_to_pop;
}

void queue_push(struct Queue *queue, char *obj)
{
	if (queue->count == queue->capacity) {
		char **temp;

		queue->capacity *= 2;
		temp = realloc(queue->data, queue->capacity * sizeof(char *));
		if (temp == NULL) {
			queue->capacity /= 2;
			exit(1);
		}
		queue->data = temp;
	}
	queue->data[queue->count] = obj;
	queue->count++;
	return;
}

struct Queue *queue_new(size_t capacity)
{
	struct Queue *queue = malloc(sizeof(struct Queue));
	if (queue == NULL) {
		return NULL;
	}

	queue->count = 0;
	queue->capacity = capacity;
	queue->data = calloc(queue->capacity, sizeof(char *));
	if (queue->data == NULL) {
		free(queue);
		return NULL;
	}

	return queue;
}
