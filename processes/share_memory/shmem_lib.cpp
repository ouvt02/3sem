#pragma once

#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>
#include <string.h>

class Client
{
  private:
    int* counter = nullptr;
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
    int* counter = nullptr;
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
    this -> counter = (int*) this -> buf_common_memory;
    *(this -> counter) = 0;
    
    this -> common_memory_name = new char[strlen(memory_name) + 1];
    strcpy(this -> common_memory_name, memory_name);
    
    this -> common_memory = shm_open(this -> common_memory_name, O_WRONLY | O_CREAT, 0400);
    if (this -> common_memory == -1)
    {
        perror("Failed to create common_memory");
        this -> ok = false;
    }
    
}

Server::~Server()
{    
    delete[] this -> common_memory_name;
    
    shm_unlink(this -> common_memory_name);
    close(this -> common_memory);
}


Client::Client(const char* memory_name)
{   
    this -> counter = (int*) this -> buf_common_memory;
    *(this -> counter) = 0;
    
    this -> common_memory_name = new char[strlen(memory_name) + 1];
    strcpy(this -> common_memory_name, memory_name);
    
    this -> common_memory = shm_open(this -> common_memory_name, O_RDONLY, 0);
    if (this -> common_memory == -1)
    {
        perror("Failed to create common_memory");
        this -> ok = false;
    }
}

Client::~Client()
{    
    delete[] this -> common_memory_name;
    
    shm_unlink(this -> common_memory_name);
    close(this -> common_memory);
}

int Server::send_str(const char* text)
{
    size_t length = sizeof(int) + strlen(text) + 1;
    
    if (ftruncate(this -> common_memory, (off_t) length) == -1)
    {
        perror("Failed to allocate shared_memory");
        return 1;
    }
    
    this -> buf_common_memory = new char[length];
    if (mmap(this -> buf_common_memory, length, PROT_WRITE, MAP_SHARED, this -> common_memory, 0) == MAP_FAILED)
    {
        perror("Failed to mmap");
        return 1;
    }
    
    strcpy((char*) (this -> buf_common_memory) + (int) sizeof(int), text);
    
    (*(this -> counter))++;
    
    munmap(buf_common_memory, length);
    
    return 0;
}

int Client::print_str()
{
    struct stat client_shm_stats = {};
    if (fstat(this -> common_memory, &client_shm_stats) == -1)
    {
        perror("Failed to get stats fo shared memory");
        return 1;
    }
    
    size_t length = (size_t) client_shm_stats.st_size;
    
    if (ftruncate(this -> common_memory, (off_t) length) == -1)
    {
        perror("Failed to allocate shared_memory");
        return 1;
    }
    
    this -> buf_common_memory = new char[length];
    if (mmap(this -> buf_common_memory, length, PROT_WRITE, MAP_SHARED, this -> common_memory, 0) == MAP_FAILED)
    {
        perror("Failed to mmap");
        return 1;
    }
    
    if (*(this -> counter) < *((int*) this -> buf_common_memory))
        printf("%s\n", (char*) (this -> buf_common_memory) + (int) sizeof(int)); 
    
    *(this -> counter) = *((int*) this -> buf_common_memory);
    
    return 0;
}






