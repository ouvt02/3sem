#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void print_proc_info(char* info, char* proc);

int main(int argc, char* argv[])
{
    char a[] = "default";
    if(argc != 2)
        argv[1] = a;
    
    print_proc_info(argv[0], argv[1]);
    
    return 0;

}


void print_proc_info(char* info, char* proc)
{
    printf("[%s] %s: PID = %d PPID = %d UID = %d GID = %d\n", 
           info, proc, getpid(), getppid(), getuid(), getgid());

}
