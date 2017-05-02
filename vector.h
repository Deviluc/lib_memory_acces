#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#define Vector struct Vector

#ifndef bool
	#define bool char
#endif

Vector {
	size_t size;
	size_t capacity;
	size_t reallocation_amount;
	void** memory_pointer;
    void (*delete_function)(void*);
};

Vector* vector_create(size_t initial_capacity, size_t reallocation_amount);
void vector_delete(Vector* vector, bool free_items);

void vector_set_delete_hook(Vector* vector, void (*delete_function)(void*));

void vector_add(Vector* vector, void* item);
void vector_remove(Vector* vector, size_t index);
void* vector_get(Vector* vector, size_t index);
void vector_reduce(Vector* vector, void* value_pointer, void (*reduction_function)(void*, void*));

#endif //VECTOR_H