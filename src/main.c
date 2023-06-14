#include "filesystem.h"
#include <immintrin.h>

int main()
{
    uint8_t init_ret = __initialize_filesystem();

    if (init_ret != 0)
    {
        printf("Error: Could not initialize filesystem.\n");
        return EXIT_FAILURE;
    }

    char *data_1 = "Hello, world!";
    char *data_2 = "Hselo, wonau!";

    struct file *file_1 = file_create("test.txt", 15, data_1);
    struct file *file_2 = file_create("test345.txt", 15, data_2);

    file_write(file_1);
    file_write(file_2);

    __write_file();

    print_disk();

    __destroy_filesystem();

    return EXIT_SUCCESS;
}

void test()
{
    uint64_t number = __builtin_bswap64(0x123456789ABCDEF0);
    uint64_t number_copy = 0;
    uint8_t *number_bytes = (uint8_t *)&number;

    if (__is_little_endian())
    {
        for (int i = 0; i < 8; i++)
            number_copy |= ((uint64_t)number_bytes[i]) << (8 * (7 - i));
    }
    else
    {
        for (int i = 0; i < 8; i++)
            number_copy |= ((uint64_t)number_bytes[i]) << (8 * i);
    }

    printf("%lX ", number_copy);
    printf("\n");
}