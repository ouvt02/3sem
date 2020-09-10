#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

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
	
	int size_of_file = lseek(src_fd, 0, SEEK_END);
	lseek(src_fd, 0, SEEK_DATA);
	char* readed = new char [size_of_file]{};
	read(src_fd, readed, size_of_file);
	
	write(dst_fd, readed, size_of_file);
	delete[] readed;
	close(src_fd);
	close(dst_fd);
	
	return 0;
}


