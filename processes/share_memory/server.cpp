#include <stdio.h>
#include "shmem_lib.cpp"

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s [name_of_shared_memory] [text of string]\n", argv[0]);
        return 1;
    }
    
    Server server(argv[1]);
    if (server.ok)
        server.send_str("");
}
