#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dlfcn.h>
#include <stdlib.h>

const double ACCURACY = 0.00001;

int main(int argc, char* argv[])
{
    if(argc != 4)
    {
        printf("Usage: %s [functino(x)] [lower limit] [higher limit]\n", argv[0]);
        return 1;
    }
    
    int gcc_pipe[2];
    if(pipe(gcc_pipe) < 0)
    {
        perror("Failed to create pipe");
        return 1;
    }
    
    pid_t child_id;
    if((child_id = fork()) == 0)
    {
        close(gcc_pipe[1]);
        
        if(dup2(gcc_pipe[0], STDIN_FILENO) < 0)
        {
            perror("Failed in dup2");
            close(gcc_pipe[0]);
            return 1;
        }
        close(gcc_pipe[0]);
        
        execlp("clang++", "clang++", "-Wall", "-Wextra", "-fPIC", "-fPIE", "-shared", 
                "-xc", "-", "-lm", "-o", "tmp.so", NULL);
        perror("Execlp returned handling");
        
        return 1;
    }
    
    FILE* file_function = fdopen(gcc_pipe[1], "wb");
    fprintf(file_function, 
            "#include <math.h>\n"
            "extern double function(double x){\n"
                "return %s;\n"
            "}\n",
            argv[1]);
    fclose(file_function);
    
    int status;
    waitpid(child_id, &status, 0);
    if (WEXITSTATUS(status) != 0)
    {
        printf("Compilation error, gcc exit code: %d\n", WEXITSTATUS(status));
        return 1;
    }
    
    void* tmplib = dlopen("./tmp.so", RTLD_LAZY);
    if(tmplib == NULL)
    {
        perror(dlerror());
        return 1;
    }
    
    double(*tmp_function)(double);
    
    if ((*(void**)(&tmp_function) = dlsym(tmplib, "function")) == NULL)
    {
        perror(dlerror());
        return 1;
    }
    
    double dx = ACCURACY;
    double integral = 0;
    
    for (double x = atof(argv[2]); x < atof(argv[3]); x += dx)
    {
        double y = (tmp_function(x) + tmp_function(x + dx))/2;
        integral += (y * dx);
    }
    
    printf("%f\n", integral);
    
    return 0;    
}

