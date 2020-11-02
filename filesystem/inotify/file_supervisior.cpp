#include <stdio.h>
#include <unistd.h>

#include <sys/inotify.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>


char* get_new_pathname(const char* directory_name, const char* file_name);
void handling_of_event(int file_with_changes);


int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("Usage: %s [path to directory]\n", argv[0]);
        return -1;
    }
    
    int file_with_changes = inotify_init1(IN_NONBLOCK);
    if(file_with_changes < 0)
    {
        perror("Bad return in inotify_init");
        return -1;
    }
    
    int watching_file = inotify_add_watch(file_with_changes, argv[1],  IN_CREATE | IN_DELETE | IN_MOVE);
    if(watching_file < 0)
    {
        perror("Bad return in inotify_add_watch");
        return -1;
    }
    
    
    while(true)
        handling_of_event(file_with_changes);
}



void handling_of_event(int file_with_changes)
{
    char buffer[4096];
    
    while(true)
    {
        ssize_t length_of_event = read(file_with_changes, buffer, sizeof(buffer));
        if(length_of_event <= 0)
            break;
        
        for(char* pointer = buffer; pointer < buffer + length_of_event; pointer += sizeof(struct inotify_event))
        {
            struct inotify_event* event = (struct inotify_event*) pointer;
            pointer += length_of_event;
            
            if(event -> mask & IN_CREATE)
            {
                printf("Created: %s\n", event -> name);
                break;
            }
            
            if(event -> mask & IN_DELETE)
            {
                printf("Deleted: %s\n", event -> name);
                break;
            }
            
            if(event -> mask & IN_CREATE)
            {
                printf("Created: %s\n", event -> name);
                break;
            }
        }
    }
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
