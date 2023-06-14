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

    char *data_1 = "Hello, world!";
    char *data_2 = "Hselo, wonau!";

    struct file *file_1 = file_create("test.txt", 15, data_1);
    struct file *file_2 = file_create("test345.txt", 15, data_2);

    file_write(file_1);
    file_write(file_2);

    __write_file("post_filewrite.bin");

    print_disk();

    int64_t file_3 = file_find("test345.txt");

    if (file_3 == -1) {
        printf("Error: Could not find file.\n");
        __destroy_filesystem();
        return EXIT_FAILURE;
    }

    // print_file(file_3);

    __destroy_filesystem();

    return EXIT_SUCCESS;
}