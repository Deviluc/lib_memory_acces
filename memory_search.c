//
// Created by bugs on 01.05.17.
//

#include <sys/stat.h>
#include "memory_search.h"
long search_initial(void *value, size_t value_byte_length, bool (*comparator)(void *, void *), Vector *memory_ranges,
                    int memory_alignment, size_t buffer_size) {
    long values_found = 0;

    for (size_t i = 0; i < memory_ranges->size; i++) {
        MemoryRange* range = vector_get(memory_ranges, i);
        long range_size =  range->end_address - range->start_address;

        struct stat st = {0};

        if (stat("search", &st) == -1) {
            mkdir("search", 0700);
        }

        string result_file_name = malloc(100);
        sprintf(result_file_name, "search/0_%lx-%lx", range->start_address, range->end_address);
        FILE* result_file = fopen(result_file_name, "w");

        size_t current_offset = 0;
        size_t current_buffer_size = 0;
        for (size_t bytes_left = (size_t) range_size; bytes_left > 0;) {
            if (bytes_left < value_byte_length) {
                break;
            } else if (bytes_left <= buffer_size) {
                current_buffer_size = bytes_left;
            } else {
                current_buffer_size = buffer_size;
            }

            void* buffer = dump_memory((void*) range->start_address + current_offset, current_buffer_size);
            size_t max_offset;

            if (value_byte_length <= memory_alignment) {
                max_offset = current_buffer_size - (current_buffer_size % (size_t) memory_alignment);
            } else {
                max_offset = current_buffer_size - (current_buffer_size % value_byte_length);
                max_offset = max_offset - (max_offset % memory_alignment);

            }


            for (size_t offset = 0; offset < max_offset; offset = offset + memory_alignment) {
                if (comparator(value, buffer + offset)) {
                    fprintf(result_file, "%lx:", current_offset + offset);
                    for (size_t n = 0; n < value_byte_length; n++) {
                        fprintf(result_file, "%x", *((char*) buffer + offset + n));
                    }
                    fprintf(result_file, "\n");
                    fflush(result_file);
                    values_found++;
                }
            }

            free(buffer);
            bytes_left = bytes_left - max_offset;
            current_offset = current_offset + max_offset;
        }

        fclose(result_file);
        free(result_file_name);
    }

    return values_found;
}


