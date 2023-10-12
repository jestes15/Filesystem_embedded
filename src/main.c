#include "filesystem.h"

int main()
{
    uint8_t init_ret = __initialize_filesystem();

    if (init_ret != 0)
    {
        printf("Error: Could not initialize filesystem.\n");
        return EXIT_FAILURE;
    }
    
    struct file * file = file_create("test.txt", 13, "Hello world");
    struct file_record* file_rec = __create_file_record(file);

    free(file);

    __add_file_record_to_mft(file_rec);

    free(file_rec);

    __destroy_filesystem();

    return EXIT_SUCCESS;
}