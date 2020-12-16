#include <errno.h>
#include <stdio.h>

#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/fiemap.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int src_fd = open(argv[1], O_RDONLY);
    int n = 0;
    
    fiemap* src_fiemap = (fiemap*) calloc(sizeof(fiemap), 1);
    int test = ioctl(src_fd, FS_IOC_FIEMAP, src_fiemap);
    
    while(test == -1)
    {
        //printf("%d\n", n);
        n++;
        src_fiemap = (fiemap*) realloc(src_fiemap, 1 * (32 + 40 * n));
        test = ioctl(src_fd, FS_IOC_FIEMAP, src_fiemap);
    }
    
    printf("%d\n", src_fiemap -> fm_extent_count);
    close(src_fd);
    
    return 0;
}
