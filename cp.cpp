#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

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
		perror("Failed to open source");
		return 1;
	}
	
	int dst_fd = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0600);
	if(dst_fd < 0)
	{
		close(src_fd);
		perror("Failed to open source");
		return 1;
	}
	
	cp(src_fd, dst_fd);
	
	return 0;
}


int cp(const int src_fd, const int dst_fd)
{
	
	int size_of_file = lseek(src_fd, 0, SEEK_END);
	lseek(src_fd, 0, SEEK_DATA);
	char* readed = new char [size_of_file]{};
	read(src_fd, readed, size_of_file);
	
	write(dst_fd, readed, size_of_file);
	delete[] readed;
	
	struct stat* st = new struct stat;
	
	if(st == nullptr)
	{
		perror("Failed to allocate memory for statbuf");
		return ERRPOINTER;
	}
	
	fstat(src_fd, st);
	mode_t mode = st -> st_mode;
	fchmod(dst_fd, mode);
	
	close(src_fd);
	close(dst_fd);
	delete st;
	
	return 0;
}
















