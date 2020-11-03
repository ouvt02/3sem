#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>


int main()
{
    pid_t child_id = fork();
    if(child_id == -1)
    {
        perror("Failed to fork");
        return -1;
    }
    
    if(child_id == 0)
    {
        execl("proc_info.out", "child_info", "child", NULL);
        perror("child: Failed to execute proc_info.out");//тк execl не возвращает управления
        return -1;
    }
    
    int child_status;
    waitpid(child_id, &child_status, 1);
    
    if (WIFEXITED(child_status))   
        printf("child has exited, status  = %d\n", WEXITSTATUS(child_status));//короче чем блок, поэтому остается в буффере для лучших времен, в файл ее не печатают => fflash
    
    else if (WIFSIGNALED(child_status))
        printf("child was killed by signall '%s' (%d)\n", strsignal(WTERMSIG(child_status)), WTERMSIG(child_status));
    
    else
        printf("unexpected child status : %d\n", child_status);
    
    fflush(stdout);
    
    execl("proc_info.out", "parent_info", "parent", NULL);
    perror("parent: Failed to execute proc_info.out");
    
    return -1;

    
}
