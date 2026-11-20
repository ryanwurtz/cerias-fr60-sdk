#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "uart.h"

static void print(const char *text)
{
    while (*text != '\0') {
        usart_putc(usart0, *text++);
    }
}

int main(void)
{
    char line[128];
    size_t length = 0;
    bool previous_was_cr = false;

    print("echo> ");
    for (;;) {
        char input;
        usart_getc(usart0, &input);

        // Treat CRLF as one Enter, and also accept CR or LF on its own.
        if (input == '\n' && previous_was_cr) {
            previous_was_cr = false;
            continue;
        }
        previous_was_cr = input == '\r';

        if (input == '\r' || input == '\n') {
            line[length] = '\0';
            if (strcmp(line, "exit") == 0) {
                return 0;
            }
            print(line);
            print("\necho> ");
            length = 0;
        } else if (input == '\b' || input == '\x7f') {
            if (length > 0) {
                --length;
            }
        } else if (input >= ' ' && input <= '~' && length < sizeof(line) - 1) {
            line[length++] = input;
        }
    }
}
