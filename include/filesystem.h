#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifndef VOLUME_SIZE
// Size in bytes (8-bit unsigned integers)
#define VOLUME_SIZE 1073741824
#endif

#ifndef NAME_SIZE
// Size in bytes (8-bit unsigned integers)
#define NAME_SIZE 128
#endif

struct file
{
    char name[NAME_SIZE];
    uint64_t size;
    uint8_t *data;
};

void print_disk();
void print_file(struct file *file);

struct file *file_create(char *restrict name, int size, uint8_t *restrict data);
void file_write(struct file *file);

int64_t file_find(char * restrict name);
struct file* file_read(char *restrict name);
void file_destroy(char *restrict name);
void file_defrag();

uint8_t __initialize_filesystem();
void __destroy_filesystem();
bool __is_little_endian();
void __write_file(const char *restrict filename);