#pragma once

#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>
#include <string.h>

const static int DEFAULT_LENGTH = 100;

class Client
{
  private:
    size_t length = 0;
    int counter = 0;
    char* common_memory_name = nullptr;
    int common_memory = 0;
    void* buf_common_memory = nullptr;
    
  public:
    bool ok = true;
    Client(const char* memory_name);
    ~Client();
    int print_str();
};

class Server
{
  private:
    size_t length = 0;
    char* common_memory_name = nullptr;
    int common_memory = 0;
    void* buf_common_memory = nullptr;
    
  public:
    bool ok = true;
    Server(const char* memory_name);
    ~Server();
    int send_str(const char* text);
};


Server::Server(const char* memory_name)
{    
    this -> common_memory_name = new char[strlen(memory_name) + 2]{'/'};
    strcpy(memory_name[0] == '/' ? this -> common_memory_name : 
                                    (this -> common_memory_name) + 1, memory_name);
    
    this -> common_memory = shm_open(this -> common_memory_name, O_RDWR | O_TRUNC| O_CREAT, 0600);
    if (this -> common_memory == -1)
    {
        perror("Failed to create common_memory in Server");
        
        delete[] this -> common_memory_name;
        shm_unlink(this -> common_memory_name);
        
        this -> ok = false;
    }
    
    this -> length = DEFAULT_LENGTH;
    
    if (ftruncate(this -> common_memory, (off_t) this -> length) == -1)
    {
        perror("Failed to allocate shared_memory");
        this -> ok = false;
    }


    if ((this -> buf_common_memory = mmap(nullptr, this -> length, PROT_WRITE | PROT_READ, MAP_SHARED_VALIDATE, this -> common_memory, 0)) == MAP_FAILED)
    {
        perror("Failed to mmap in Server");
        
        shm_unlink(this -> common_memory_name);
        close(this -> common_memory);
        
        this -> ok = false;
    }
    
    *((int*) this -> buf_common_memory) = 0;
}



Server::~Server()
{
    delete[] this -> common_memory_name;
    
    shm_unlink(this -> common_memory_name);
    close(this -> common_memory);
    
    munmap(this -> buf_common_memory, this -> length);
}


Client::Client(const char* memory_name)
{ 
    this -> length = DEFAULT_LENGTH;
    
    this -> common_memory_name = new char[strlen(memory_name) + 2]{'/'};
    strcpy(memory_name[0] == '/' ? this -> common_memory_name : 
                                    (this -> common_memory_name) + 1, memory_name);
    
    this -> common_memory = shm_open(this -> common_memory_name, O_RDWR, 0);
    if (this -> common_memory == -1)
    {
        perror("Failed to create common_memory in Client");
        
        delete[] this -> common_memory_name;
        shm_unlink(this -> common_memory_name);
        
        this -> ok = false;
    }
    
    if (ftruncate(this -> common_memory, (off_t) this -> length) == -1)
    {
        perror("Failed to allocate shared_memory");
        this -> ok = false;
    }
    
    if ((this -> buf_common_memory = mmap(nullptr, this -> length, PROT_READ | PROT_WRITE, MAP_SHARED, this -> common_memory, 0)) == MAP_FAILED)
    {
        perror("Failed to mmap int client");
        this -> ok = false;
    }
    
    *((int*) this -> buf_common_memory) = 0;
    this -> counter = 0;
}

Client::~Client()
{
    delete[] this -> common_memory_name;
    
    shm_unlink(this -> common_memory_name);
    close(this -> common_memory);
    
    munmap(this -> buf_common_memory, this -> length);
}


int Server::send_str(const char* text)
{    
    if(this -> ok == false)
        return 1;
    
    strcpy((char*) (this -> buf_common_memory) + (int) sizeof(int), text);
    
    (*((int*)this -> buf_common_memory))++;
    
    return 0;
}

int Client::print_str()
{
    if(this -> ok == false)
        return 1;
    
    if (this -> counter < *((int*) this -> buf_common_memory))        
        printf("%s\n", (char*) (this -> buf_common_memory) + (int) sizeof(int));
    
    this -> counter = *((int*) this -> buf_common_memory);
    
    return 0;
}


