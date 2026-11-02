#include "uart.h"

int a = 0;
int b = 10;


int divide(int x, int y)
{
    return x / y;
}

int main () {
    int c = 5;

    a = divide(b, c);
    usart0->dr = 'A';

    return a;
}