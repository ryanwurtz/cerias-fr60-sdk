#include <stdint.h>

int32_t signed_quotient;
int32_t signed_remainder;
uint32_t unsigned_quotient;
uint32_t unsigned_remainder;

int32_t divide_signed(int32_t dividend, int32_t divisor)
{
    return dividend / divisor;
}

int32_t modulo_signed(int32_t dividend, int32_t divisor)
{
    return dividend % divisor;
}

uint32_t divide_unsigned(uint32_t dividend, uint32_t divisor)
{
    return dividend / divisor;
}

uint32_t modulo_unsigned(uint32_t dividend, uint32_t divisor)
{
    return dividend % divisor;
}

int main(void)
{
    signed_quotient = divide_signed(-17, 5);
    signed_remainder = modulo_signed(-17, 5);
    unsigned_quotient = divide_unsigned(UINT32_C(0xffffffff), 13);
    unsigned_remainder = modulo_unsigned(UINT32_C(0xffffffff), 13);
    if (signed_quotient != -3 || signed_remainder != -2) {return 1;}
    if (unsigned_quotient != UINT32_C(330382099) || unsigned_remainder != 8) {return 2;}
    if (modulo_signed(17, -5) != 2) {return 3;}
    if (modulo_signed(-17, -5) != -2) {return 4;}
    if (modulo_signed(-20, 5) != 0) {return 5;}
    return 0;
}
