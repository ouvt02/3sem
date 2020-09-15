#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include <string.h>

#include <sys/dir.h>

#include <dirent.h>

#define ERRPOINTER -1


#ifndef RELEASE
	#define DEBUG printf("\x1b[35m>> debug from <%s::%d>\n\x1b[0m", __FILE__, __LINE__);
#else
	#define DEBUG
#endif


char* get_new_pathname(char* directory_name, char* file_name);
int copy_directory(char* src_pathname, char* dst_name);
int copy_file(int src_file, int dst_file);


int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		printf("Usage: %s src dst\n", argv[0]);
		return 1;
	}
	
	int status = 0;
	
	struct stat* src_stats = new struct stat;
    status = lstat(argv[1], src_stats);
    if(status == -1)
    {
        perror("Failed to get stat for source");
        return ERRPOINTER;
    }
    
    if(S_ISREG(src_stats -> st_mode))
    {
        int src_file = open(argv[1], O_RDONLY);
        if(src_file == 0)
        {
            perror("Failed to open source");
            return 1;
        }
        
        struct stat* dst_stats = new struct stat;
        status = lstat(argv[2], dst_stats);
        if(status == -1 or S_ISREG(dst_stats -> st_mode))
        {
            int dst_file = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0600);
            if(dst_file == 0)
            {
                perror("Failed to open destination");
                return 1;
            }
            
            copy_file(src_file, dst_file);
            
            close(src_file);
            close(dst_file);
            return 0;
        }
        
        else if(S_ISDIR(dst_stats -> st_mode))
        {
            DIR* dst_dir = opendir(argv[2]);
            if(dst_dir == nullptr)
            {
                perror("Failed to open destination");
                return 1;
            }
            
            char* dst_pathname = get_new_pathname(argv[2], argv[1]);
            int dst_file = open(dst_pathname, O_WRONLY | O_TRUNC | O_CREAT, 0600);
            
            copy_file(src_file, dst_file);
            
            close(src_file);
            closedir(dst_dir);
            
            return 0;
        } 
        
    }
    
    else if(S_ISDIR(src_stats -> st_mode))
    {
        copy_directory(argv[1], argv[2]);
        
        return 0;
    }
    
    else
    {
        printf("Something strande with source or directory\n");
        return 1;
    }
    
    return 0;
}


int copy_file(int src_file, int dst_file)
{
	int size_of_file = lseek(src_file, 0, SEEK_END);
	lseek(src_file, 0, SEEK_DATA);
    
	char* readed = new char [size_of_file]{};
    if (readed == nullptr)
    {
        perror("Bad allocation");
        return ERRPOINTER;
    }
    
	read(src_file, readed, size_of_file);
	
	write(dst_file, readed, size_of_file);
    
    struct stat* src_stats = new struct stat;
	fstat(src_file, src_stats);
	fchmod(dst_file, src_stats -> st_mode);
    
	delete[] readed;
	delete src_stats;
	
	return 0;
}

int copy_directory(char* src_name, char* dst_name)
{
    dirent64* entry = nullptr;
    char* dst_pathname = nullptr;
    char* src_pathname = nullptr;
    
    int dst_file = 0;
    int src_file = 0;
    
    struct stat* src_file_stats = new struct stat;
    
    DIR* src_dir = opendir(src_name);
    DIR* dst_dir = opendir(dst_name);
    
    while((entry = readdir64(src_dir)) != nullptr)
    {
        if(!strcmp(entry -> d_name, ".") or !strcmp(entry -> d_name, ".."))
            continue;
        
        src_pathname = get_new_pathname(src_name, entry -> d_name);
        lstat(src_pathname, src_file_stats);
        
        if(S_ISDIR(src_file_stats -> st_mode))
        {
            src_pathname = get_new_pathname(src_name, entry -> d_name);
            dst_pathname = get_new_pathname(dst_name, entry -> d_name);
            mkdir(dst_pathname, src_file_stats -> st_mode);
            copy_directory(src_pathname, dst_pathname);
            
            continue;
        }
        
        else if(S_ISREG(src_file_stats -> st_mode))
        {
            dst_pathname = get_new_pathname(dst_name, entry -> d_name);
            dst_file = open(dst_pathname, O_WRONLY | O_TRUNC | O_CREAT, 0600);
            
            src_pathname = get_new_pathname(src_name, entry -> d_name);
            src_file = open(src_pathname, O_RDONLY);
            
            copy_file(src_file, dst_file);
            continue;
        }
        
        else
        {
            printf("Something strange in src_dir\n");
            return 1;
        }
        
    }
    
    closedir(dst_dir);
    closedir(src_dir);
    
    return 0;
}

char* get_new_pathname(char* directory_name, char* file_name)
{
    char* pathname = new char[strlen(file_name) + strlen(directory_name) + 2];
    if (pathname == nullptr)
        perror("Bad allocation");
            
    strcpy(pathname, directory_name);
    strcat(pathname, "/");
    strcat(pathname, file_name);
    
    return pathname;
}





