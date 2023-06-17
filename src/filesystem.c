#include "filesystem.h"

struct disk
{
    uint8_t *disk_volume;
    uint64_t empty_section_start;

    uint64_t master_file_table_size;
    struct file_record *master_file_table;
} disk;

void print_disk()
{
    uint64_t current_ptr = 0;
    while (current_ptr < disk.empty_section_start)
    {
        uint64_t file_size = 0;
        char filename[NAME_SIZE] = {0};

        printf("File Name:\n");
        for (int i = 0; i < NAME_SIZE; i++)
        {
            printf("%02X ", disk.disk_volume[i + current_ptr]);
            filename[i] = disk.disk_volume[i + current_ptr];
        }
        printf("(%s)\n", filename);

        printf("File Size:\n");
        for (int i = 0; i < 8; i++)
        {
            printf("%02X ", disk.disk_volume[current_ptr + NAME_SIZE + i]);
            file_size |= ((uint64_t)disk.disk_volume[current_ptr + NAME_SIZE + i]) << (8 * i);
        }
        printf("(%ld)\n", file_size);
        char *file_data = (char *)malloc(file_size * sizeof(char));

        printf("File Data:\n");
        for (int i = 0; i < file_size - 1; i++)
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
    printf("File Name:\n");
    for (int i = 0; i < NAME_SIZE; i++)
        printf("%02X ", file->name[i]);
    printf("(%s)\n", file->name);

    printf("File Size:\n");
    for (int i = 0; i < 8; i++)
        printf("%02X ", ((uint8_t *)&file->size)[i]);
    printf("(%ld)\n", file->size);

    printf("File Data:\n");
    for (int i = 0; i < file->size - 1; i++)
        printf("%02X ", file->data[i]);
    printf("(%s)\n", file->data);
}

struct file *file_create(char *name, int size, uint8_t *data)
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
    file->data = data;

    return file;
}

void file_write(struct file *file)
{
    uint8_t *file_size = (uint8_t *)&file->size;

    for (int i = 0; i < NAME_SIZE; i++)
        disk.disk_volume[disk.empty_section_start + i] = file->name[i];

    disk.empty_section_start += NAME_SIZE;

    for (int i = 0; i < 8; i++)
        disk.disk_volume[disk.empty_section_start + i] = file_size[i];

    disk.empty_section_start += 8;

    for (int i = 0; i < file->size - 1; i++)
        disk.disk_volume[disk.empty_section_start + i] = file->data[i];

    disk.empty_section_start += file->size - 1;
}

int64_t file_find(char *name)
{
    int64_t current_ptr = 0;
    while (current_ptr < disk.empty_section_start)
    {
        char filename[NAME_SIZE] = {0};
        for (int i = 0; i < NAME_SIZE; i++)
        {
            filename[i] = disk.disk_volume[i + current_ptr];
        }

        if (strcmp(filename, name) == 0)
        {
            return current_ptr;
        }

        uint64_t file_size = 0;
        for (int i = 0; i < 8; i++)
            file_size |= ((uint64_t)disk.disk_volume[current_ptr + NAME_SIZE + i]) << (8 * i);

        current_ptr += NAME_SIZE + 8 + file_size - 1;
    }
    return -1;
}

struct file *file_read(char *name)
{
    uint64_t current_ptr = 0;
    while (current_ptr < disk.empty_section_start)
    {
        char filename[NAME_SIZE] = {0};
        for (int i = 0; i < NAME_SIZE; i++)
        {
            filename[i] = disk.disk_volume[i + current_ptr];
        }

        if (strcmp(filename, name) == 0)
        {
            uint64_t file_size = 0;
            for (int i = 0; i < 8; i++)
            {
                file_size |= ((uint64_t)disk.disk_volume[current_ptr + NAME_SIZE + i]) << (8 * i);
            }
            char *file_data = (char *)malloc(file_size * sizeof(char));
            for (int i = 0; i < file_size - 1; i++)
            {
                file_data[i] = disk.disk_volume[current_ptr + NAME_SIZE + 8 + i];
            }
            return file_create(filename, file_size, (uint8_t *)file_data);
        }
    }
    return NULL;
}

void file_destroy(char *name)
{
    int i;
    uint64_t offset = file_find(name);
    uint64_t file_size = 0;

    for (i = 0; i < 8; i++)
        file_size |= ((uint64_t)disk.disk_volume[offset + NAME_SIZE + i]) << (8 * i);

    memset(disk.disk_volume + offset, 0x00, NAME_SIZE + 8 + file_size - 1);
}

void file_defrag()
{
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
    free(disk.master_file_table);
}

bool __is_little_endian()
{
    volatile uint32_t i = 0x01234567;
    return (*((uint8_t *)(&i))) == 0x67;
}

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

struct file_record* __create_file_record(struct file * new_file)
{
    struct file_record *file_record = (struct file_record *)malloc(sizeof(struct file_record));

    if (file_record == NULL)
    {
        printf("Error: Could not allocate memory for file record.\n");
        return NULL;
    }

    strcpy(file_record->name, new_file->name);
    file_record->size = NAME_SIZE + 8 + new_file->size - 1;
    file_record->location = disk.empty_section_start;

    return file_record;
}

uint8_t __add_file_record_to_mft(struct file_record *file_record)
{
    struct file_record *new_mft =
        (struct file_record *)malloc(sizeof(struct file_record) * (disk.master_file_table_size + 1));
    if (new_mft == NULL)
    {
        printf("Error: Could not allocate memory for master file table.\n");
        return 1;
    }

    void *ret = memcpy(new_mft, disk.master_file_table, sizeof(struct file_record) * disk.master_file_table_size);
    if ((struct file_record *)ret != new_mft)
    {
        printf("Error: Could not copy master file table.\n");
        return 2;
    }

    new_mft[disk.master_file_table_size] = *file_record;
    disk.master_file_table_size++;

    free(disk.master_file_table);
    disk.master_file_table = new_mft;

    return 0;
}