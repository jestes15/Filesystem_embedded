#include "filesystem.h"

struct disk
{
    uint8_t *disk_volume;
    uint64_t empty_section_start;
} disk;

void __write_file(const char *filename)
{
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        printf("Error: Could not open file.\n");
        return;
    }

    fwrite(disk.disk_volume, sizeof(uint8_t), VOLUME_SIZE, fp);
    fclose(fp);
}

void print_disk()
{
    uint64_t current_ptr = 0;
    while (current_ptr < disk.empty_section_start)
    {
        int i;
        uint64_t file_size = 0;
        char filename[NAME_SIZE] = {0};

        printf("File Name:\n");
        for (i = 0; i < NAME_SIZE; i++)
        {
            printf("%02X ", disk.disk_volume[i + current_ptr]);
            filename[i] = disk.disk_volume[i + current_ptr];
        }
        printf("(%s)\n", filename);

        printf("File Size:\n");
        for (i = 0; i < 8; i++)
        {
            printf("%02X ", disk.disk_volume[current_ptr + NAME_SIZE + i]);
            file_size |= ((uint64_t)disk.disk_volume[current_ptr + NAME_SIZE + i]) << (8 * i);
        }
        printf("(%ld)\n", file_size);
        char *file_data = (char *)malloc(file_size * sizeof(char));

        printf("File Data:\n");
        for (i = 0; i < file_size - 1; i++)
        {
            file_data[i] = disk.disk_volume[current_ptr + NAME_SIZE + 8 + i];
            printf("%02X ", disk.disk_volume[current_ptr + NAME_SIZE + 8 + i]);
        }
        printf("(%s)\n", file_data);

        current_ptr += file_size - 1 + 8 + NAME_SIZE;
    }
}

void print_file(struct file *file)
{
    int i;

    printf("File Name:\n");
    for (i = 0; i < NAME_SIZE; i++)
        printf("%02X ", file->name[i]);
    printf("(%s)\n", file->name);

    printf("File Size:\n");
    for (i = 0; i < 8; i++)
        printf("%02X ", ((uint8_t *)&file->size)[i]);
    printf("(%ld)\n", file->size);

    printf("File Data:\n");
    for (i = 0; i < file->size - 1; i++)
        printf("%02X ", file->data[i]);
    printf("(%s)\n", file->data);
}

struct file *file_create(char * name, int size, uint8_t * data)
{
    struct file *file = (struct file *)malloc(sizeof(struct file));

    if (file == NULL)
    {
        printf("Error: Could not allocate memory for file.\n");
        return NULL;
    }

    if (strlen(name) > NAME_SIZE)
    {
        printf("Error: File name is too long.\n");
        return NULL;
    }

    strcpy(file->name, name);
    file->size = size;
    file->data = (uint8_t *)malloc(size * sizeof(uint8_t));

    if (file->data == NULL)
    {
        printf("Error: Could not allocate memory for file data.\n");
        return NULL;
    }

    if (data != NULL)
    {
        void *ret = memcpy(file->data, data, size * sizeof(uint8_t));

        if ((uint8_t *)ret != file->data)
        {
            printf("Error: Could not copy file data.\n");
            return NULL;
        }
    }

    return file;
}

void file_write(struct file *file)
{
    int i;
    uint8_t *file_size = (uint8_t *)&file->size;

    for (i = 0; i < NAME_SIZE; i++)
        disk.disk_volume[disk.empty_section_start + i] = file->name[i];

    disk.empty_section_start += NAME_SIZE;

    for (i = 0; i < 8; i++)
        disk.disk_volume[disk.empty_section_start + i] = file_size[i];

    disk.empty_section_start += 8;

    for (i = 0; i < file->size - 1; i++)
        disk.disk_volume[disk.empty_section_start + i] = file->data[i];

    disk.empty_section_start += file->size - 1;
}

int64_t file_find(char * name)
{
    int i;
    int64_t current_ptr = 0;
    while (current_ptr < disk.empty_section_start)
    {
        char filename[NAME_SIZE] = {0};
        for (i = 0; i < NAME_SIZE; i++)
        {
            filename[i] = disk.disk_volume[i + current_ptr];
        }

        if (strcmp(filename, name) == 0)
        {
            return current_ptr;
        }

        uint64_t file_size = 0;
        for (i = 0; i < 8; i++)
            file_size |= ((uint64_t)disk.disk_volume[current_ptr + NAME_SIZE + i]) << (8 * i);

        current_ptr += NAME_SIZE + 8 + file_size - 1;
    }
    return -1;
}

struct file *file_read(char * name)
{
    uint64_t current_ptr = 0;
    while (current_ptr < disk.empty_section_start)
    {
        int i;
        char filename[NAME_SIZE] = {0};
        for (i = 0; i < NAME_SIZE; i++)
        {
            filename[i] = disk.disk_volume[i + current_ptr];
        }

        if (strcmp(filename, name) == 0)
        {
            uint64_t file_size = 0;
            for (i = 0; i < 8; i++)
            {
                file_size |= ((uint64_t)disk.disk_volume[current_ptr + NAME_SIZE + i]) << (8 * i);
            }
            char *file_data = (char *)malloc(file_size * sizeof(char));
            for (i = 0; i < file_size - 1; i++)
            {
                file_data[i] = disk.disk_volume[current_ptr + NAME_SIZE + 8 + i];
            }
            return file_create(filename, file_size, (uint8_t *)file_data);
        }
    }
    return NULL;
}

void file_destroy(char * name)
{
    uint64_t file = file_find(name);

    // for (int i = 0; i < 8; i++)
    // file_size |= ((uint64_t)disk.disk_volume[file + NAME_SIZE + i]) << (8 * i);
}

uint8_t __initialize_filesystem()
{
    disk.disk_volume = (uint8_t *)malloc(VOLUME_SIZE * sizeof(uint8_t));

    if (disk.disk_volume == NULL)
    {
        printf("Error: Could not allocate memory for disk volume.\n");
        return 1;
    }

    void *ret = memset(disk.disk_volume, 0x00, VOLUME_SIZE * sizeof(uint8_t));

    if ((uint8_t *)ret != disk.disk_volume)
    {
        printf("Error: Could not initialize disk volume.\n");
        return 2;
    }
    return 0;
}

void __destroy_filesystem()
{
    free(disk.disk_volume);
}

bool __is_little_endian()
{
    volatile uint32_t i = 0x01234567;
    return (*((uint8_t *)(&i))) == 0x67;
}