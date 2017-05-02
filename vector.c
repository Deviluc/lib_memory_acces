#include "vector.h"

void allocate_memory(Vector* vector) {
	vector->memory_pointer = calloc(vector->capacity, sizeof(void*));
}

void reallocate_memory(Vector* vector) {
	size_t new_capacity = (vector->size + vector->reallocation_amount);
	vector->memory_pointer = realloc(vector->memory_pointer, new_capacity * sizeof(void*));
	vector->capacity = new_capacity;
}

Vector* vector_create(size_t initial_capacity, size_t reallocation_amount) {
	Vector* vec = malloc(sizeof(Vector));
	vec->size = 0;
	vec->capacity = initial_capacity;
	vec->reallocation_amount = reallocation_amount;
	allocate_memory(vec);
	return vec;
}

void vector_delete(Vector* vector, bool free_items) {
    if (vector->delete_function != NULL) {
        void (*f)(void*) = vector->delete_function; // more performant (less ASM ops)
        for (int i = 0; i < vector->size; i++) {
            f(vector->memory_pointer[i]);
        }
    }

	if (free_items) {
		for (int i = 0; i < vector->size; i++) {
			free(vector->memory_pointer[i]);
		}
	}

	free(vector->memory_pointer);
	free(vector);
}

void vector_add(Vector* vector, void* item) {
	if (vector->size == vector->capacity) {
		reallocate_memory(vector);
	}

	vector->memory_pointer[vector->size] = item;
	vector->size = vector->size + 1;
}

void* vector_get(Vector* vector, size_t index) {
	return vector->memory_pointer[index];
}

void vector_remove(Vector* vector, size_t index) {
	for (size_t i = index; i < vector->size - 1; i++) {
		vector->memory_pointer[i] = vector->memory_pointer[i+1];
	}

	vector->memory_pointer[vector->size-1] = NULL;
	vector->size = vector->size - 1;
}

void vector_reduce(Vector* vector, void* value_pointer, void (*reduction_function)(void*, void*)) {
	for (int i = 0; i < vector->size; i++) {
		reduction_function(value_pointer, vector->memory_pointer[i]);
	}
}

void vector_set_delete_hook(Vector *vector, void (*delete_function)(void *)) {
    vector->delete_function = delete_function;
}
