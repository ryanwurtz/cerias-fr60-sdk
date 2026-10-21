#include "uart.h"

const usart_basic_t usart = {
    .basic = { usart0, NULL, usart2, usart3}
};
const usart_fifo_t usart_fifo = {
    .fifo = { usart4, usart5, usart6, usart7}
};

bool usart_exists(usart_basic_hw_t *ch) {
    return ch != NULL;
}

int usart_putc(usart_basic_hw_t *ch, char c) {

}

int usart_getc(usart_basic_hw_t *ch, char *dest) {

}