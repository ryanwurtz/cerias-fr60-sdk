#include "uart.h"

unsigned int a = 0;
unsigned int b = 10;


unsigned int divide(unsigned int x, unsigned int y)
{
    return x / y;
}

unsigned int main () {
    unsigned int c = 5;

    a = divide(b, c);
    usart0->dr = 'A';

    return a;
}