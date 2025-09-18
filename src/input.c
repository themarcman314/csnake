#include <stdio.h>
#include <termios.h>
#include <unistd.h> // needed for fileno of stdin
#include "input.h"


void init_termios(void)
{
    struct termios term1;

    if (tcgetattr(STDIN_FILENO, &term1) != 0)
    perror("tcgetattr() error");
    else 
    {
    printf("the original c_lflag flag is x'%02x'\n",
            term1.c_lflag);
            
    term1.c_lflag &= ~ICANON; // disable buffering stdin
    term1.c_lflag &= ~ECHO;  // turn off echo
    term1.c_cc[VMIN] = 0; // receive one character
    term1.c_cc[VTIME] = 0; // block until one char is received

    if (tcsetattr(STDIN_FILENO, TCSANOW, &term1) != 0)
        perror("tcsetattr() error");
    }
}

// TODO: add support for wasd keys?
int read_arrow_key_in(void)
{
    char c[3];
    if(read(STDIN_FILENO, c, 3) > 0)
    {
        if(c[0] == 0x1b &&c[1] == 0x5b ) // ansi escape sequence for arrows
        {
            //printf("character : %c\n", c[2]);
            switch (c[2])
            {
            case 'A': return KEY_UP;
            case 'B': return KEY_DOWN;
            case 'C': return KEY_RIGHT;
            case 'D': return KEY_LEFT;
            }
        }
    }
    return -1;
}