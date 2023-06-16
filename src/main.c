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

    __write_file("post_initialization.bin");

    char data_1[] = "Hello, World!";
    char data_2[] = "This is a test.";

    struct file *file_1 = file_create("test1.txt", sizeof(data_1), data_1);
    struct file *file_2 = file_create("test2.txt", sizeof(data_2), data_2);

    file_write(file_1);
    file_write(file_2);

    __write_file("post_filewrite.bin");

    file_destroy("test1.txt");

    __write_file("post_filedestroy.bin");

    __destroy_filesystem();

    return EXIT_SUCCESS;
}