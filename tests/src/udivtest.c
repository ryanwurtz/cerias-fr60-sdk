#include "uart.h"

unsigned int divide(unsigned int b, unsigned int c)
{
    return b / c;
}

int main () {
    unsigned int a = 0;
    int b = 10;
    int c = 5;

    a = divide(b, c);
    usart0->dr = 'A';

    return a;
}