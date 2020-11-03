#include <stdio.h>
#include <unistd.h>

int main()
{
    int pipe_fd[2];
    
    if(pipe(pipe_fd) == -1)
    {
        perror("Failed to create pipe");
        return 1;
    }
    
    pid_t child_id = fork();
    if(child_id == -1)
    {
        perror("Failed to fork");
        return 1;
    }
    
    if(child_id == 0)
    {
        close(pipe_fd[0]);
        
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        
        execlp("last", "last", "-f", "/var/log/wtmp", NULL);
        
        perror("Failed to execute 'last");
        return 1;
    }
    
    close(pipe_fd[1]);
    
    dup2(pipe_fd[0], STDIN_FILENO);
    close(pipe_fd[0]);
    
    execlp("grep", "grep", "-v", "^user", NULL);
    
    perror("Failed to execute 'grep");
    return 1;
}
