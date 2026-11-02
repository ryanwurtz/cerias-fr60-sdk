#include "uart.h"
#include <stdio.h>

int multiply(int b, int c)
{
    return b * c;
}

int main () {
    int a = 0;
    int b = 10;
    int c = 5;

    a = multiply(b, c);

    printf("%d\n", a);

    if (a > 0) {return 0;}
    else {return 1;}
}