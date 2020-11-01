#include <errno.h>
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
    
    int counter_fd = -1;
    while (counter_fd == -1) 
    {
        counter_fd = open(name_of_file, O_RDWR);
        if (errno == 2)
            counter_fd = open(name_of_file, O_RDWR | O_CREAT);
    }

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
    fclose(file_counter);
    
    flock(counter_fd, LOCK_UN);
    close(counter_fd);
    
	return 0;
}
