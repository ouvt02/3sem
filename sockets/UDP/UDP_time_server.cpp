#include "UDP_time_lib.cpp"
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("Usage: %s [port]\n", argv[0]);
        return 1;
    }
    
    Server server(atoi(argv[1]));
    if(!(server.ok))
        return 1;
    
    
    while(true)
        server.receive_str();
    
    return 0;
}

