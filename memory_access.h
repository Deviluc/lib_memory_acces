//
// Created by bugs on 28.04.17.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
#ifndef DEBUGGER_MEMORY_ACCESS_H
#define DEBUGGER_MEMORY_ACCESS_H

#define _GNU_SOURCE

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ptrace.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include "vector.h"

#ifndef string
#define string char*
#endif

#define MemoryRange struct MemoryRange
#define MemoryMap struct MemoryMap

#define memory_page_byte_size 4096

MemoryRange {
    long start_address;
    long end_address;
    bool read;
    bool write;
    bool execute;
    bool private;
    string file_name;
};

MemoryMap {
    Vector* memory_ranges;
    MemoryRange* heap;
    MemoryRange* stack;
    MemoryRange* executable;
};



/**
 * Returns the current value of errno, it is then set back to zero.
 * @return int errno
 */
int getErrno();
bool is_attached();
bool attach_thread(pid_t thread_id);
bool detach_thread();
MemoryMap* get_memory_map();
void* dump_memory(void* start_address, size_t size);

#endif //DEBUGGER_MEMORY_ACCESS_H
#pragma clang diagnostic pop