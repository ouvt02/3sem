#include "board_lib.cpp"

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s [text of message] [name_of_board]\n", argv[0]);
        return 1;
    }
    
    Client sender(argv[2]);
    if (sender.ok)
        sender.sendMessage(argv[1], 0);
    
    return 0;
}
