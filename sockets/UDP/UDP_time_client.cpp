#include <stdio.h>
#include "UDP_time_lib.cpp"
#include <stdlib.h>
#include <arpa/inet.h>
#include <time.h>

int main(int argc, char* argv[])
{
    if(argc < 4)
    {
        printf("Usage: %s [port of client] [port of server] [address send to]\n", argv[0]);
        return 1;
    }
    
    Client client(atoi(argv[1]));
    if(!(client.ok))
        return 1;
    
    //struct sockaddr_in addr_receiver = {AF_INET, htons(atoi(argv[1])), {inet_addr(argv[2])}};
    struct sockaddr_in addr_receiver;
    (addr_receiver).sin_family = AF_INET;
    (addr_receiver).sin_port = htons(atoi(argv[2]));
    inet_aton(argv[3], &(addr_receiver.sin_addr));
    memset(&(addr_receiver.sin_zero), '\0', 8);
    
    struct tm* timeinfo = nullptr;
    time_t clock;
    size_t sufficient_lenght = 100;
    char* time_buf = new char[sufficient_lenght]{};
    
    while(true)
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
        
        if(client.send_str(time_buf, addr_receiver) == 1)
            return 1;
        
        sleep(60);
    }
    
    return 0;
}
