#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

struct file_record
{
    char name[NAME_SIZE];
    uint64_t size;
    uint64_t location;
};

void print_disk();
void print_file(struct file *file);

struct file *file_create(char *name, int size, uint8_t *data);
void file_write(struct file *file);

int64_t file_find(char *name);
struct file *file_read(char *name);
void file_destroy(char *name);
void file_defrag();

uint8_t __initialize_filesystem();
void __destroy_filesystem();
bool __is_little_endian();
void __write_file(const char *filename);
uint8_t __add_file_record_to_mft(struct file_record *file_record);
struct file_record *__create_file_record(struct file *new_file);