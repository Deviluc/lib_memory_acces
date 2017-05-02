//
// Created by bugs on 01.05.17.
//

#ifndef DEBUGGER_MEMORY_SEARCH_H
#define DEBUGGER_MEMORY_SEARCH_H

#include "memory_access.h"
#include "vector.h"

long search_initial(void* value, size_t value_byte_length, bool (*comparator)(void*,void*), Vector* memory_ranges, int memory_alignment, size_t buffer_size);

#endif //DEBUGGER_MEMORY_SEARCH_H
