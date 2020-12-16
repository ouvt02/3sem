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

#include <utime.h>

#define ERROR -1

#define MB * 1048576 //bytes in Megabyte
#define KB * 1024
#define BLOCK_SIZE 4 KB

char* get_new_pathname(const char* directory_name, const char* file_name);
int copy_directory(const char* src_pathname, const char* dst_name);
int copy_file(const char* src_name, const char* dst_name);
int copy_links(const char* src_link_name, const char* dst_name);
int copy_FIFO(const char* src_name, const char* dst_name);


int main(int argc, char* argv[])
{    
	if (argc != 3)
	{
		printf("Usage: %s src dst\n", argv[0]);
		return ERROR;
	}
	
	int status = 0;
	
	struct stat src_stats = {};
    status = lstat(argv[1], &src_stats);
    if(status == -1)
    {
        perror("Failed to get stat for source");
        return ERROR;
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
                return ERROR;
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
        return 0;
    }
    
    else if (S_ISFIFO(src_stats.st_mode))
    {
        copy_FIFO(argv[1], argv[2]);
        return 0;
    }
    
    else
    {
        printf("Something strande with source or directory\n");
        return ERROR;
    }
    
    return 0;
}


int copy_file(const char* src_name, const char* dst_name)
{
    int src_file = open(src_name, O_RDONLY);
    if(src_file < 0)
    {
        perror("Failed to open source");
        return ERROR;
    }
    
    int dst_file = open(dst_name, O_WRONLY | O_TRUNC | O_CREAT, 0600);
    if(dst_file < 0)
    {
        perror("Failed to open destination");
        return ERROR;
    }
    
    struct stat src_stats = {};
    int status = lstat(src_name, &src_stats);
    if (status < 0)
    {
        perror("Cannot read file stats");
        return ERROR;
    }


    char* readed = new char [BLOCK_SIZE]{};
        
    if (readed == nullptr)
    {
        perror("Bad allocation");
        return ERROR;
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
    lchown(dst_name, src_stats.st_uid, src_stats.st_gid);
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
        return ERROR;
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
        return ERROR;
    }
    
    char* new_pathname = get_new_pathname(current_directory, dst_name);
    
    if(S_ISDIR(dst_stats.st_mode))
    {
        new_pathname = get_new_pathname(new_pathname, src_link_name);
        symlink(link_pathname, new_pathname);
    }
    
    else if(S_ISLNK(dst_stats.st_mode) or status == -1)
        symlink(link_pathname, new_pathname);
    
    else
    {
        printf("Something strange with dst\n");
        return ERROR;
    }
    
    
    lchown(new_pathname, src_stats.st_uid, src_stats.st_gid);
    chmod(new_pathname, src_stats.st_mode);
    struct timespec dst_times[2] = {src_stats.st_atim, src_stats.st_mtim};
    int dst_link = open(new_pathname, O_RDONLY);
    futimens(dst_link, dst_times);

    delete[] link_pathname;
    delete[] current_directory;
    
    return 0;
}


int copy_FIFO(const char* src_name, const char* dst_name)
{
    int status = 0;
	
	struct stat src_stats = {};
    status = lstat(src_name, &src_stats);
    if(status == -1)
    {
        perror("Failed to get stat for source");
        return ERROR;
    }
    
    status = mkfifo(dst_name, src_stats.st_mode);
    if(status != 0)
    {
        perror("Failed to create FIFO");
        return ERROR;
    }
    
    utimbuf src_times;
    src_times.actime = src_stats.st_atim.tv_sec;
    src_times.modtime = src_stats.st_mtim.tv_sec;
    utime(dst_name, &src_times);
    lchown(dst_name, src_stats.st_uid, src_stats.st_gid);   
    
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
        return ERROR;
    }
    
    DIR* dst_dir = opendir(dst_name);
    if(dst_dir == nullptr)
    {
        perror("Failed to open destination source");
        return ERROR;
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
            lchown(dst_pathname, src_file_stats.st_uid, src_file_stats.st_gid);
            
            struct timespec dst_times[2] = {src_file_stats.st_atim, src_file_stats.st_mtim};
            
            copy_directory(src_pathname, dst_pathname);
            
            futimens(dst_poddir, dst_times);
            
            close(dst_poddir);
            
            continue;
        }
        
        else if (S_ISREG(src_file_stats.st_mode))
        {
            dst_pathname = get_new_pathname(dst_name, entry -> d_name);
            src_pathname = get_new_pathname(src_name, entry -> d_name);
            
            copy_file(src_pathname, dst_pathname);
            continue;
        }
        
        else if (S_ISLNK(src_file_stats.st_mode))
        {
            src_pathname = get_new_pathname(src_name, entry -> d_name);
            dst_pathname = get_new_pathname(dst_name, entry -> d_name);
            
            copy_links(src_pathname, dst_pathname);
            continue;
        }
        else if (S_ISFIFO(src_file_stats.st_mode))
        {
            src_pathname = get_new_pathname(src_name, entry -> d_name);
            dst_pathname = get_new_pathname(dst_name, entry -> d_name);
            
            copy_FIFO(src_pathname, dst_pathname);
            continue;
        }
        
        else
        {
            printf("Something strange in src_dir\n");
            return ERROR;
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


//mknod для файловых устройств

//если не удалось скопировать один файл, вывести что нельзя скопировать и продолжить копировать остальные

