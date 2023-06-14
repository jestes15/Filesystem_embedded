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
    char name[128];
    uint64_t size;
    uint8_t *data;
};

void print_disk();
struct file *file_create(char *name, int size, uint8_t *data);
void file_write(struct file *file);

struct file *file_find(char *name);
struct file* file_read(char* name);
void file_destroy(struct file *file);
void file_defrag();

uint8_t __initialize_filesystem();
void __destroy_filesystem();
bool __is_little_endian();
void __write_file();