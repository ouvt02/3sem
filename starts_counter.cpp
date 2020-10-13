#include <stdio.h>
#include <unistd.h>
#include <linux/can/error.h>
#include <error.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>


int main()
{	
    const char name_of_file[] = "counter.txt";
        
    pid_t child_id = fork();
    if (child_id == -1)
    {
        perror("Failed to fork");
        return 1;
    }
    
    if (child_id == 0)
        child_id = fork();
    
    if (child_id == 0)
        child_id = fork();
    
    
    int counter_fd = -1;
    while (counter_fd == -1) 
        counter_fd = open(name_of_file, O_RDWR);

    flock(counter_fd, LOCK_EX);
    
 	FILE* file_counter = fopen(name_of_file, "r");
    int number_of_starts = 0;

    if (file_counter != NULL) 
    {
        fscanf(file_counter, "%d", &number_of_starts);
        fclose(file_counter);
    }
    
    file_counter = fopen(name_of_file, "w");
    fprintf(file_counter, "%d", ++number_of_starts);
    
    flock(counter_fd, LOCK_UN);
    
	return 0;
}
