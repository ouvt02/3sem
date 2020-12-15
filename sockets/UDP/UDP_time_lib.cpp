#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

const static int DEFAULT_LENGTH = 1000;

class Client
{
  private:
    int socket_fd = 0;
    struct sockaddr_in addr_sender = {};
    size_t length = DEFAULT_LENGTH;
  
  public:
    bool ok = true;
    Client(int port);
    ~Client();
    int send_str(const char* text_of_msg, const struct sockaddr_in addr_receiver);
};

class Server
{
  private:
    int socket_fd = 0;
    struct sockaddr_in addr_receiver = {};
    size_t length = DEFAULT_LENGTH;
    char* message_buffer = nullptr;
    
  public:
    bool ok = true;
    Server(int port);
    ~Server();
    int receive_str();
};

Server::Server(int port)
{
    this -> socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if(this -> socket_fd == -1)
    {
        perror("Failed to create a socket");
        this -> ok = false;
    }
    
    //this -> addr_receiver = {AF_INET, htons(port), {htonl(INADDR_ANY)}};
    (this -> addr_receiver).sin_family = AF_INET;
    (this -> addr_receiver).sin_port = htons(port);
    (this -> addr_receiver).sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&((this -> addr_receiver).sin_zero), '\0', 8);
    
    
    if(bind(this -> socket_fd, (const struct sockaddr*) &(this -> addr_receiver), 
            (socklen_t) sizeof(this -> addr_receiver)) == -1)
    {
        perror("Failed to blind a name to a socket");
        close(this -> socket_fd);
        this -> ok = false;
    }
    
    (this -> message_buffer) = new char [this -> length]{};
}

Server::~Server()
{
    close(this -> socket_fd);
    delete[] this -> message_buffer;
}


int Server::receive_str()
{
    ssize_t status = recv(this -> socket_fd, (void*) (this -> message_buffer), 
                              this -> length, 0);
    if(status == -1)    
    {
        perror("Failed to receive message");
        return 1;
    }

    printf("%s\n", this -> message_buffer);

    return 0;
}




Client::Client(int port)
{
    this -> socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(this -> socket_fd == -1)
    {
        perror("Failed to create socket");
        this -> ok = false;
    }
    
    //this -> addr_sender = {AF_INET, htons(7542), {htonl(INADDR_ANY)}};
    (this -> addr_sender).sin_family = AF_INET;
    (this -> addr_sender).sin_port = htons(port);
    (this -> addr_sender).sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&((this -> addr_sender).sin_zero), '\0', 8);
    
    if(bind(this -> socket_fd, (const struct sockaddr*) &(this -> addr_sender), 
            (socklen_t) sizeof(this -> addr_sender)) == -1)
    {
        perror("Failed to blind a name to a socket");
        close(this -> socket_fd);
        this -> ok = false;
    }
}

Client::~Client()
{
    close(this -> socket_fd);
}


int Client::send_str(const char* text_of_msg, const struct sockaddr_in addr_receiver)
{
    ssize_t status = sendto(this -> socket_fd, (const void*) text_of_msg, this -> length, MSG_DONTROUTE, 
           (const struct sockaddr*) &addr_receiver, (socklen_t) sizeof(addr_receiver));
    
    if(status == -1)
    {
        perror("Failed to send message");
        return 1;
    }
    
    return 0;
}




