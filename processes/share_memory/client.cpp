#include <stdio.h>
#include "shmem_lib.cpp"

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("Usage: %s [name_of_shared_memory]\n", argv[0]);
        return 1;
    }
    
    Client client(argv[1]);
    while(true)
    {
        if (client.ok)
        {
            if(client.print_str() == 1)
                return 1;
        }
    }
    
    return 0;
}
