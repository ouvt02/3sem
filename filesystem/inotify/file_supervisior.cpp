#include <stdio.h>
#include <unistd.h>

#include <sys/inotify.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>


char* get_new_pathname(const char* directory_name, const char* file_name);

int main(int argc, char* argv[])
{
    if (argc == 2)
        chdir(get_new_pathname(get_current_dir_name(), argv[1]));
    
    int file_changes = inotify_init();
    
    inotify_add_watch(file_changes, get_current_dir_name(), 
                          IN_CREATE | IN_DELETE | IN_MOVE);
        
    struct inotify_event event;
    char buf[4096];
    
    while (true)
    {
        read(file_changes, buf, 4097);
        printf("%s\n", event.name);
    }
    
    close(file_changes);
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
