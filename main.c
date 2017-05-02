#include <stdio.h>
#include "memory_search.h"

bool equal(void* a, void* b) {
    return *((int*) a) == *((int*) b);
}

void main() {
    if (attach_thread(15022)) {
        printf("Attached! errno: %i\n", errno);
        //printf("Heap:\n%s\nErrno: %i\n", (string) dump_memory((void*) get_memory_map()->stack->start_address, 512 * 80), getErrno());

        int value = 3675;

        long values_found = search_initial(&value, sizeof(int), equal, get_memory_map()->memory_ranges, 4, 4096 * _SC_IOV_MAX);
        printf("Values found: %li\n", values_found);

        detach_thread();
    } else {
        printf("Not attached, errno: %i\n", getErrno());
    }
}
