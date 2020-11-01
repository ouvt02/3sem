#include <stdio.h>

#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/fiemap.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
    int src_fd = open(argv[1], O_RDONLY);
    fiemap src_fiemap;
    ioctl(src_fd, FS_IOC_FIEMAP, &src_fiemap);
    
    printf("%d\n", src_fiemap.fm_extent_count);
    
    return 0;
}
