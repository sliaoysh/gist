#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
using namespace std;

int main()
{
    winsize termsize;
    while(true)
    {
        cout << "\033[1;1H\033[2J"; // clear screen
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &termsize);
        cout << "Height: " << termsize.ws_row << " rows" << endl;
        cout << "Width: " << termsize.ws_col << " columns" << endl;
        sleep(1);
    }
    return 0;
}
