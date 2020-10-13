#include <stdio.h>
#include <sys/vfs.h>
#include <unistd.h>

#define MB * 1048576
#define KB * 1024
#define GB * 1073741824

#define SIZE_OF_BLOCK 4096

int show_partision_info(const char* path);

int main(int argc, char* argv[])
{
    if (argc == 2)
        show_partision_info(argv[1]);
    
    if (argc < 2)
    {
        char* name_of_cur_dir = get_current_dir_name();
        show_partision_info(name_of_cur_dir);
    }
    
    return 0;
}

int show_partision_info(const char* path)
{    
    struct statfs src_statfs;;
    if (statfs(path, &src_statfs) < -1)
    {
        perror("Failed to get statfs for source\n");
        return 1;
    }
    
    printf("For %s\nTotal space: %lu Gb,\nFree space: %lu Gb,\nAvailable space: %lu Gb\n", path,
            src_statfs.f_blocks * SIZE_OF_BLOCK / (1 GB), src_statfs.f_bfree * SIZE_OF_BLOCK / (1 GB),
            src_statfs.f_bavail * SIZE_OF_BLOCK / (1 GB));
    
    return 0;
    
}


