#include "basic_types.h"

#define USART0_BASE_OFFSET 0x00000040

typedef struct {
    io_rw_8 scr;
    io_rw_8 smr;
    io_rw_8 ssr;
    io_rw_8 dr;
    io_rw_8 escr;
    io_rw_8 eccr;
    io_rw_8 fsr;
    io_rw_8 fcr;
} uart_hw_t;

#define usart0 ((uart_hw_t *)USART0_BASE_OFFSET)