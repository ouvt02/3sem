#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include <string.h>

#define ERRPOINTER -1

int cp(const int src_fd, const int dst_fd);

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		printf("Usage: %s src dst\n", argv[0]);
		return 1;
	}
	
	int src_fd = open(argv[1], O_RDONLY);
	if(src_fd < 0)
	{
		printf("Failed to open source");
		return 1;
	}
	
	int dst_fd = open(argv[2], O_WRONLY | O_TRUNC);
	if(dst_fd < 0)
		dst_fd = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0600); 
    
    struct stat* dst_st = new struct stat;
    if(dst_st == nullptr)
    {
        perror("Failed to allocate memory for statbuf");
        return ERRPOINTER;
    }
    
    lstat(argv[2], dst_st);
    mode_t dst_mode = dst_st -> st_mode;
    
    if(S_ISDIR(dst_mode))
    {
        char* pathname = new char[strlen(argv[1]) + strlen(argv[2]) + 2];
        strcpy(pathname, argv[2]);
        strcat(pathname, "/");
        strcat(pathname, argv[1]);
        dst_fd = open(pathname, O_WRONLY | O_TRUNC | O_CREAT, 0600);
    }
	
	cp(src_fd, dst_fd);
	
	return 0;
}


int cp(const int src_fd, const int dst_fd)
{
    struct stat* src_st = new struct stat;
    if(src_st == nullptr)
	{
		perror("Failed to allocate memory for statbuf");
		return ERRPOINTER;
	}    
	
	fstat(src_fd, src_st);
	mode_t src_mode = src_st -> st_mode;
	fchmod(dst_fd, src_mode);
    
	int size_of_file = lseek(src_fd, 0, SEEK_END);
	lseek(src_fd, 0, SEEK_DATA);
	char* readed = new char [size_of_file]{};
	read(src_fd, readed, size_of_file);
	
	write(dst_fd, readed, size_of_file);
    
	delete[] readed;
	close(src_fd);
	close(dst_fd);
	delete src_st;
	
	return 0;
}















