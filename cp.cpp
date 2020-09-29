#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include <string.h>

#include <sys/dir.h>

#include <dirent.h>
#include <errno.h>
#include <cstdlib>
#include <fcntl.h>

#define ERRPOINTER -1


//#define RELEASE


#ifndef RELEASE
	#define DEBUG printf("\x1b[35m>> debug from <%s::%d>\n\x1b[0m", __FILE__, __LINE__);
    #define SWITCH if (true)
#else
	#define DEBUG
    #define SWITCH if (false)
#endif

#define MB * 1048576 //bytes in Megabyte
#define KB * 1024
#define BLOCK_SIZE 4 KB


// #define _FILE_OFFSET_BITTS 64 //for lseek(without define may be short - 32) in man lseek64

char* get_new_pathname(const char* directory_name, const char* file_name);
int copy_directory(const char* src_pathname, const char* dst_name);
int copy_file(const char* src_name, const char* dst_name);
int copy_links(const char* src_link_name, const char* dst_name);


int main(int argc, char* argv[])
{    
	if (argc != 3)
	{
		printf("Usage: %s src dst\n", argv[0]);
		return 1;
	}
	
	int status = 0;
	
	struct stat src_stats = {};
    status = lstat(argv[1], &src_stats);
    if(status == -1)
    {
        perror("Failed to get stat for source");
        return ERRPOINTER;
    }
    
    if(S_ISREG(src_stats.st_mode))
    {
        struct stat dst_stats = {};
        status = lstat(argv[2], &dst_stats);
        if(status == -1 or S_ISREG(dst_stats.st_mode))
        {
            copy_file(argv[1], argv[2]);
            return 0;
        }
        
        else if(S_ISDIR(dst_stats.st_mode))
        {
            DIR* dst_dir = opendir(argv[2]);
            if(dst_dir == nullptr)
            {
                perror("Failed to open destination source");
                return 1;
            }
            
            char* dst_pathname = get_new_pathname(argv[2], argv[1]);
            
            copy_file(argv[1], dst_pathname);
            
            closedir(dst_dir);
            
            return 0;
        } 
        
    }
    
    else if(S_ISDIR(src_stats.st_mode))
    {
        copy_directory(argv[1], argv[2]);
        
        return 0;
    }
    
    else if (S_ISLNK(src_stats.st_mode))
    {
        copy_links(argv[1], argv[2]);
    }
    
    else
    {
        printf("Something strande with source or directory\n");
        return 1;
    }
    
    return 0;
}


int copy_file(const char* src_name, const char* dst_name)
{
    int src_file = open(src_name, O_RDONLY);
    if(src_file < 0)
    {
        perror("Failed to open source");
        return 1;
    }
    
    int dst_file = open(dst_name, O_WRONLY | O_TRUNC | O_CREAT, 0600);
    if(dst_file < 0)
    {
        perror("Failed to open destination");
        return 1;
    }
    
    struct stat src_stats = {};
    int status = lstat(src_name, &src_stats);
    if (status < 0)
    {
        perror("Cannot read file stats");
        return 1;
    }


    char* readed = new char [BLOCK_SIZE]{};
        
    if (readed == nullptr)
    {
        perror("Bad allocation");
        return ERRPOINTER;
    }

    ssize_t n_readed = 0;
    while ((n_readed = read(src_file, readed, BLOCK_SIZE)) > 0)
    {
        ssize_t n_writed = 0;
        while (n_writed < n_readed)
        {
            ssize_t writed = 0;
            if ((writed = write(dst_file, readed + n_writed, n_readed - n_writed)) == -1)
            {
                perror("Writing error");
                exit(errno);
            }
            
            n_writed += writed;
        }
        
    }
    
   
    fchmod(dst_file, src_stats.st_mode); 
    struct timespec dst_times[2] = {src_stats.st_atim, src_stats.st_mtim};
    futimens(dst_file, dst_times);
    
    delete[] readed;
    
    close(src_file);
    close(dst_file);
	
	return 0;
}

int copy_links(const char* src_link_name, const char* dst_name)
{
    struct stat src_stats = {};
    int status = lstat(src_link_name, &src_stats);
    if(status == -1)
    {
        perror("Failed to get stat for source");
        return ERRPOINTER;
    }
    
    char* link_pathname = new char[src_stats.st_size + 1]{};
    readlink(src_link_name, link_pathname, src_stats.st_size + 1);
    
    char* current_directory = nullptr;
    struct stat dst_stats = {};
    status = lstat(dst_name, &dst_stats);
    if(status == -1)
    {
        current_directory = new char[src_stats.st_size + 1]{};
        getcwd(current_directory, src_stats.st_size);
    }
    
    else
    {
        current_directory = new char[dst_stats.st_size + 1]{};
        getcwd(current_directory, dst_stats.st_size);
    }
    
    if(current_directory == nullptr)
    {
        printf("Bad findings of current_directory\n");
        return ERRPOINTER;
    }
    
    char* new_pathname = get_new_pathname(current_directory, dst_name);
    
    if(S_ISDIR(dst_stats.st_mode))
        symlink(link_pathname, get_new_pathname(new_pathname, src_link_name));
    
    else if(S_ISLNK(dst_stats.st_mode) or status == -1)
        symlink(link_pathname, new_pathname);
    
    else
    {
        printf("Something strange with dst\n");
        return ERRPOINTER;
    }
    
    delete[] link_pathname;
    delete[] current_directory;
    
    return 0;
}

int copy_directory(const char* src_name, const char* dst_name)
{
    dirent64* entry = nullptr;
    char* dst_pathname = nullptr;
    char* src_pathname = nullptr;
    
    struct stat src_file_stats = {};
    
    DIR* src_dir = opendir(src_name);
    if(src_dir == nullptr)
    {
        perror("Failed to open source");
        return 1;
    }
    
    DIR* dst_dir = opendir(dst_name);
    if(dst_dir == nullptr)
    {
        perror("Failed to open destination source");
        return 1;
    }
    
    int dst_poddir = 0;
        
    while((entry = readdir64(src_dir)) != nullptr)
    {
        if(!strcmp(entry -> d_name, ".") or !strcmp(entry -> d_name, ".."))
            continue;
        
        src_pathname = get_new_pathname(src_name, entry -> d_name);
        lstat(src_pathname, &src_file_stats);
        
        if(S_ISDIR(src_file_stats.st_mode))
        {
            src_pathname = get_new_pathname(src_name, entry -> d_name);
            dst_pathname = get_new_pathname(dst_name, entry -> d_name);
            mkdir(dst_pathname, src_file_stats.st_mode);
            
            dst_poddir = open(dst_pathname, O_RDONLY);
            fchmod(dst_poddir, src_file_stats.st_mode);
            
            struct timespec dst_times[2] = {src_file_stats.st_atim, src_file_stats.st_mtim};
            futimens(dst_poddir, dst_times);///////don't work for directories'
            
            close(dst_poddir);
            
            copy_directory(src_pathname, dst_pathname);
            
            continue;
        }
        
        else if(S_ISREG(src_file_stats.st_mode))
        {
            dst_pathname = get_new_pathname(dst_name, entry -> d_name);
            src_pathname = get_new_pathname(src_name, entry -> d_name);
            
            copy_file(src_pathname, dst_pathname);
            continue;
        }
        
        else if(S_ISLNK(src_file_stats.st_mode))
        {
            src_pathname = get_new_pathname(src_name, entry -> d_name);
            dst_pathname = get_new_pathname(dst_name, entry -> d_name);
            
            copy_links(src_pathname, dst_pathname);
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

char* get_new_pathname(const char* directory_name, const char* file_name)
{
    char* pathname = new char[strlen(file_name) + strlen(directory_name) + 2];
    if (pathname == nullptr)
        perror("Bad allocation");
            
    strcpy(pathname, directory_name);
    strcat(pathname, "/");
    strcat(pathname, file_name);
    
    return pathname;
}



//time of birth -- statx
//in stat different times
//add utime/futimes/futimens


