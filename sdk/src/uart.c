#include "uart.h"

int usart_putc(usart_basic_t *ch, char c) {
    while (!usart_tx_empty(ch)) {
        //wait
    }
    ch->dr = c;
    return 1;
}

int usart_getc(usart_basic_t *ch, char *dest) {
    while (!usart_rx_full(ch)) {
        //wait
    }
    *dest = ch->dr;
    return 1;
}

