#include "uart.h"

static void integer_to_string(int32_t value, char *text) {
    char digits[10];
    unsigned int count = 0;
    uint32_t magnitude = (uint32_t)value;

    if (value < 0) {
        *text++ = '-';
        magnitude = 0u - magnitude;
    }

    do {
        digits[count++] = (char)('0' + magnitude % 10u);
        magnitude /= 10u;
    } while (magnitude != 0);

    while (count != 0) {
        *text++ = digits[--count];
    }
    *text = '\0';
}

int factorial(int n) {
    if (n == 1) {
        return n;
    }
    return n * factorial(n-1);
}

int main(void) {
    int num = 10;
    char text[12];


    integer_to_string(factorial(num), text);

    for (unsigned int i = 0; text[i] != '\0'; ++i) {
        usart0->dr = (uint8_t)text[i];
    }

    return 0;
}
