#ifndef SDK_UART_H
#define SDK_UART_H

#include "basic_types.h"
#include <stdbool.h>
#include <stddef.h>

#define USART0_BASE_OFFSET 0x00000040
#define USART2_BASE_OFFSET 0x00000050
#define USART3_BASE_OFFSET 0x00000058
#define USART4_BASE_OFFSET 0x00000060
#define USART5_BASE_OFFSET 0x00000068
#define USART6_BASE_OFFSET 0x00000070
#define USART7_BASE_OFFSET 0x00000078

typedef struct {
    io_rw_8 scr;
    io_rw_8 smr;
    io_rw_8 ssr;
    io_rw_8 dr;
    io_rw_8 escr;
    io_rw_8 eccr;
} usart_basic_t;

typedef struct {
    io_rw_8 scr;
    io_rw_8 smr;
    io_rw_8 ssr;
    io_rw_8 dr;
    io_rw_8 escr;
    io_rw_8 eccr;
    io_ro_8 fsr;
    io_rw_8 fcr;
} usart_fifo_t;

#define usart0 ((usart_basic_t *)USART0_BASE_OFFSET)
#define usart2 ((usart_basic_t *)USART2_BASE_OFFSET)
#define usart3 ((usart_basic_t *)USART3_BASE_OFFSET)
#define usart4 ((usart_fifo_t *)USART4_BASE_OFFSET)
#define usart5 ((usart_fifo_t *)USART5_BASE_OFFSET)
#define usart6 ((usart_fifo_t *)USART6_BASE_OFFSET)
#define usart7 ((usart_fifo_t *)USART7_BASE_OFFSET)

static inline bool usart_exists(usart_basic_t *ch) { return ch != NULL; }
static inline bool usart_rx_full(usart_basic_t *ch) { return (ch->ssr & 0x10u) != 0; }
static inline bool usart_tx_empty(usart_basic_t *ch) { return (ch->ssr & 0x08u) != 0; }

int usart_putc(usart_basic_t *ch, char c);
int usart_getc(usart_basic_t *ch, char *dest);

#endif
