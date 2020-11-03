#include <stdio.h>
#include <string.h>
#include <mqueue.h>

class Client
{
  private:
    char* queue_name;
    mqd_t queue;
    
  public:
    bool ok = true;
    Client(const char* board_name);
    ~Client();
    int sendMessage(const char* text, int priority);
};

class Server
{
  private:
    char* queue_name;
    mqd_t queue;
    char* readMessage(unsigned int priority);
    
  public:
    bool ok = true;
    Server(const char* board_name);
    ~Server();
    void polling();
};


Client::Client(const char* board_name)
{
    this -> queue_name = new char[strlen(board_name) + 2]{'/'};
    strcpy(board_name[0] == '/' ? this -> queue_name : (this -> queue_name) + 1, board_name);
    
    this -> queue = mq_open(this -> queue_name, O_WRONLY, NULL);
    if(this -> queue == -1)
        this -> ok = false;
}

Client::~Client()
{
    delete[] this -> queue_name;
    mq_close(this -> queue);
}


int Client::sendMessage(const char* text, int priority)
{
    int status = mq_send(this -> queue, text, strlen(text), priority);
    return status;
}

Server::Server(const char* board_name)
{
    this -> queue_name = new char[strlen(board_name) + 2]{'/'};
    strcpy(board_name[0] == '/' ? this -> queue_name : (this -> queue_name) + 1, board_name);
    
    this -> queue = mq_open(this -> queue_name, O_RDONLY | O_CREAT, 0620, NULL);
    if(this -> queue == -1)
        this -> ok = false;
}

Server::~Server()
{
    delete[] this -> queue_name;
    mq_close(this -> queue);
}

char* Server::readMessage(unsigned int priority)
{
    struct mq_attr attr_of_mq;
    
    int status = mq_getattr(this -> queue, &attr_of_mq);
    if(status == -1)
        this -> ok = false;
    
    char* text = new char[attr_of_mq.mq_msgsize];
    
    ssize_t received = mq_receive(this -> queue, text, attr_of_mq.mq_msgsize, &priority);
    if(received == -1)
        this -> ok = false;
    
    return text;
}

void Server::polling()
{
    char* text_of_message = nullptr;
    while(true)
    {
        text_of_message = this -> readMessage(0);
        printf("%s\n", text_of_message);
        delete[] text_of_message;
    }
}







