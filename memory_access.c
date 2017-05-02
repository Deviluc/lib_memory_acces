//
// Created by bugs on 28.04.17.
//

#include "memory_access.h"

static pid_t pid;
static bool attached = false;
static MemoryMap* memory_map;
static string memory_path;

static int get_digit_count(pid_t number) {
    pid_t tmp = number;
    int digit_count = 1;

    while (tmp > 9) {
        digit_count++;
        tmp = tmp / 10;
    }

    return digit_count;
}

static MemoryMap* create_memory_map(pid_t thread_id) {
    char memory_map_path[12 + get_digit_count(pid)];
    sprintf(memory_map_path, "/proc/%i/maps", pid);
    FILE* map_file = fopen(memory_map_path, "r");

    if (map_file != NULL) {
        int bytes_read = 0;

        MemoryMap* map = malloc(sizeof(MemoryMap));
        map->memory_ranges = vector_create(10, 5);

        while (true) {
            MemoryRange* range = malloc(sizeof(MemoryRange));
            bytes_read = fscanf(map_file, "%lx-%lx ", &range->start_address, &range->end_address);

            if (bytes_read == EOF) {
                free(range);
                break;
            }

            char r,w,x,p;
            fscanf(map_file, "%c%c%c%c ", &r, &w, &x, &p);
            range->read = r == 'r';
            range->write = w == 'w';
            range->execute = x == 'x';
            range->private = p == 'p';

            char line_rest[512];
            fgets(line_rest, sizeof(line_rest), map_file);

            int space_count = 0;
            int file_start = -1;
            int file_end = -1;

            for (int i = 0; line_rest[i] != '\0'; i++) {
                char c = line_rest[i];

                if (c == ' ') {
                    space_count++;
                } else if (space_count >= 4 && file_start == -1) {
                    file_start = i;
                } else if (c == '\n') {
                    file_end = i - 1;
                    break;
                }
            }

            if (file_start != -1 && file_end != -1) {
                size_t filename_size = (size_t) (2 + file_end - file_start);
                string filename = malloc(filename_size);
                for (int i = 0; i < (filename_size - 1); i++) {
                    filename[i] = line_rest[i + file_start];
                }

                filename[filename_size - 1] = '\0';
                range->file_name = filename;

                if (strcmp(range->file_name, "[heap]") == 0) {
                    map->heap = range;
                } else if (strcmp(range->file_name, "[stack]") == 0) {
                    map->stack = range;
                }
            }

            vector_add(map->memory_ranges, range);
            //TODO: check if memory range is program code
        }

        fclose(map_file);
        return map;
    }

    return NULL;
}

int getErrno() {
    int err = errno;
    errno = 0;
    return err;
}

bool is_attached() {
    return attached;
}

bool attach_thread(pid_t thread_id) {
    if (ptrace(PTRACE_SEIZE, thread_id, 0, 0) != -1) {
        pid = thread_id;
        
        memory_map = create_memory_map(pid);

        memory_path = malloc((size_t) (11 + get_digit_count(pid)));
        sprintf(memory_path, "/proc/%i/mem", pid);

        attached = true;
    }

    return attached;
}

bool detach_thread() {
    if (attached) {
        attached = ptrace(PTRACE_DETACH, pid, 0, 0) ? false : true;
    }

    return attached;
}

MemoryMap* get_memory_map() {
    return memory_map;
}

bool write_memory(void* source, size_t size, void* destination) {
    //TODO: do something!
}

void* dump_memory(void* start_address, size_t size) {
    void* memory = malloc(size);

    size_t vec_count = (size / memory_page_byte_size) + ((size % memory_page_byte_size) == 0 ? 0 : 1);
    //struct iovec* local_dest_vecs = malloc(vec_count * sizeof(struct iovec));
    struct iovec* remote_source_vecs = malloc(vec_count * sizeof(struct iovec));

    size_t bytes_left = size;
    size_t current_offset = 0;
    size_t current_size = 0;
    size_t current_index = 0;

    while (bytes_left > 0) {
        if (bytes_left < memory_page_byte_size) {
            current_size = bytes_left;
            bytes_left = 0;
        } else {
            current_size = memory_page_byte_size;
            bytes_left = bytes_left - memory_page_byte_size;
        }

        /*struct iovec local_destination;
        local_destination.iov_base = memory + current_offset;
        local_destination.iov_len = current_size;*/

        struct iovec remote_source;
        remote_source.iov_base = start_address + current_offset;
        remote_source.iov_len = current_size;

        //local_dest_vecs[current_index] = local_destination;
        remote_source_vecs[current_index] = remote_source;

        current_offset = current_offset + current_size;
        current_index = current_index + 1;
    }

    struct iovec local_dest_vec;
    local_dest_vec.iov_base = memory;
    local_dest_vec.iov_len = size;

    ssize_t bytes_dumped = process_vm_readv(pid, &local_dest_vec, 1, remote_source_vecs, vec_count, 0);

    free(remote_source_vecs);

    if (bytes_dumped == -1) {
        free(memory);
        return NULL;
    }

    return memory;
}