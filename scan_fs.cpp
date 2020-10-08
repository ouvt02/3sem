#include <stdio.h>
#include <sys/vfs.h>
#include <sys/statfs.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MB * 1048576
#define KB * 1024
#define GB * 1073741824

#define SIZE_OF_BLOCK 4096

int main(int argc, char* argv[])
{
    int root_fd = open("/", O_RDONLY);
    struct statfs root_statfs;
    fstatfs(root_fd, &root_statfs);
    
    printf("Total memory: %lu Gb,\nFree memory: %lu Gb,\nAvailable memory: %lu Gb\n", 
            root_statfs.f_blocks * SIZE_OF_BLOCK / (1 GB), root_statfs.f_bfree * SIZE_OF_BLOCK / (1 GB),
            root_statfs.f_bavail * SIZE_OF_BLOCK / (1 GB));
    
    return 0;
}
