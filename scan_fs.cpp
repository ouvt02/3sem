#include <stdio.h>
#include <sys/vfs.h>
#include <sys/statfs.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#define _FILE_OFFSET_BITTS 64

#define MB * 1048576
#define KB * 1024
#define GB * 1073741824

#define SIZE_OF_BLOCK 4096

#ifndef RELEASE
	#define DEBUG printf("\x1b[35m>> debug from <%s::%d>\n\x1b[0m", __FILE__, __LINE__);
#else
	#define DEBUG
#endif

struct info_file
{
    char** words;
    int number_of_spaces;
};

info_file* read_file(const char* name_of_file);
int show_partision_info(const char* path, const char* mount_point);

int main()
{
    info_file* information = read_file("/proc/self/mountinfo");
    
    //printf("%d\n", information -> number_of_spaces);
    for(int i = 0; i < information -> number_of_spaces; i++)
        printf("%d: %s ", i, (information -> words)[i]);
    
    for(int i = 0; i < information -> number_of_spaces; i++)
        show_partision_info((information -> words)[i * 9], (information -> words)[i * 5]);
    
    
    return 0;
}

int show_partision_info(const char* path, const char* mount_point)
{
    int src_fd = open(path, O_RDONLY);
    if(src_fd < 0)
    {
        perror("Failed to open source\n");
        return 1;
    }
    
    struct statfs src_statfs;
    int status = fstatfs(src_fd, &src_statfs);
    if(status < -1)
    {
        perror("Failed to get statfs for source\n");
        close(src_fd);
        return 1;
    }
    
    printf("Total memory: %lu Gb,\nFree memory: %lu Gb,\nAvailable memory: %lu Gb\n", 
            src_statfs.f_blocks * SIZE_OF_BLOCK / (1 GB), src_statfs.f_bfree * SIZE_OF_BLOCK / (1 GB),
            src_statfs.f_bavail * SIZE_OF_BLOCK / (1 GB));
    
    return 0;
    
}

info_file* read_file(const char* name_of_file)
{
    info_file* information = new info_file;
    
    int info_fd = open(name_of_file, O_RDONLY);
    if(info_fd < 0)
    {
        perror("Failed to open information fd");
        return nullptr;
    }
    
    int status = 0;
    
    struct stat file_status;
    status = fstat(info_fd, &file_status);
    if(status < 0)
    {
        perror("Failed to get stat for file");
        return nullptr;
    }
    
    off_t size_of_file = file_status.st_size;
    
    printf("%ld\n", file_status.st_size); //prints 0!!!
    
	char* readed = new char [size_of_file + 1]{};
	read(info_fd, readed, size_of_file);
    
    
    int number_of_str = 0;
    information -> number_of_spaces = 0;
    int max_lenght_of_word = 0;
    int lenght_of_word = 0;
    
    for(off_t i = 0; i < size_of_file; i++)
    {
        lenght_of_word++;
        if (readed[i] == '\n')
        {
            number_of_str++;
            if(lenght_of_word > max_lenght_of_word)
                max_lenght_of_word = lenght_of_word;
            lenght_of_word = 0;
        }
        
        else if (readed[i] == ' ')
        {
            DEBUG
            information -> number_of_spaces++;
            if(lenght_of_word > max_lenght_of_word)
                max_lenght_of_word = lenght_of_word;
            lenght_of_word = 0;
        }
    }
    
    information -> words = new char*[number_of_str];
    for(int i = 0; i < number_of_str; i++)
        (information -> words)[i] = new char[max_lenght_of_word + 1];
    
    int in_words = 0;
    int in_words_i = 0;
    int in_readed = 0;
    
    for(in_readed = 0; in_readed < size_of_file; in_readed++)
    {
        while(readed[in_readed] != ' ')
            (information -> words)[in_words++][in_words_i++] = readed[in_readed++];
    }
    
    return information;
}

// /proc/self/mountinfo
