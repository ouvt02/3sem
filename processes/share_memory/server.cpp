#include <stdio.h>
#include <time.h>
#include "shmem_lib.cpp"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s [name_of_shared_memory]\n", argv[0]);
        return 1;
    }
    
    struct tm* timeinfo = nullptr;
    time_t clock;
    size_t sufficient_lenght = 100;
    char* time_buf = new char[sufficient_lenght]{};
    
    Server server(argv[1]);
    while(true)
    {
        if(server.ok)
        {
            if(time(&clock) == (time_t) -1)
            {
                perror("Failed to find out the time");
                return 1;
            }
            
            if((timeinfo = localtime(&clock)) == NULL)
            {
                perror("Failed to set struct tm");
                return 1;
            }
            
            strftime(time_buf, sufficient_lenght, "%c", timeinfo);
            if(server.send_str(time_buf) == 1)
                return 1;
            sleep(1);
        }
    }
    
    return 0;
}






