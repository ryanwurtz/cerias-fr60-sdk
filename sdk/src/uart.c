#include "uart.h"

const usart_basic_t usart = {
    .basic = { usart0, NULL, usart2, usart3}
};
const usart_fifo_t usart_fifo = {
    .fifo = { usart4, usart5, usart6, usart7}
};

int usart_putc(usart_basic_hw_t *ch, char c) {
    while (!usart_tx_empty(ch)) {
        //wait
    }
    ch->dr = c;
    return 1;
}

int usart_getc(usart_basic_hw_t *ch, char *dest) {
    while (!usart_rx_full(ch)) {
        //wait
    }
    *dest = ch->dr;
    return 1;
}

