#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>


int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("Usage: %s [name_of_message_queue]\n", argv[0]);
        return 1;
    }
    
    if(argv[1][0] != '/')
    {
        printf("\x1b[1;31merror:\x1b[0m Incorect name of the message queue [%s], it must begin with '/'\n", argv[1]);
        return 1;
    }
    
    mqd_t mess_queue = mq_open(argv[1], O_RDWR | O_CREAT, 0600, NULL);
    if(mess_queue == -1)
    {
        perror("Failed to open message queue");
        return 1;
    }
    
    struct mq_attr attr_of_mq;
    
    int status = mq_getattr(mess_queue, &attr_of_mq);
    if(status == -1)
    {
        perror("Failed to get attr for mess_queue");
        return 1;
    }
    
    printf("For message queue [%s]:\nFlags: %ld\nMax number of message in queu: %ld\nMax size of message: %ld\nCurrent number of message in the queue: %ld\n",
        argv[1], attr_of_mq.mq_flags, attr_of_mq.mq_maxmsg, attr_of_mq.mq_msgsize, attr_of_mq.mq_curmsgs);
    
    mq_close(mess_queue);
    mq_unlink(argv[1]);
    
    return 0;
}
