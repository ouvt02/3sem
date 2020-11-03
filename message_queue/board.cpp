#include "board_lib.cpp"

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("Usage: %s [name_of_board]\n", argv[0]);
        return 1;
    }
    
    Server board("Test");
    if (board.ok)
        board.polling();
    
    return 0;
}
