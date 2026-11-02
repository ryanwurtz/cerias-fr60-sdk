/* IETF ChaCha20 block function. RFC 8439 sections 2.1.1 and 2.3.2.
 * https://www.rfc-editor.org/rfc/rfc8439.html
 * Test fixture only; the block counter is supplied explicitly.
 */
#include <stdint.h>

uint8_t chacha_result[64];
uint32_t chacha_quarter_result[4];

static uint32_t load_le32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
        ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static uint32_t rotate_left(uint32_t x, unsigned int n)
{
    return (x << n) | (x >> (32 - n));
}

void chacha_quarter_round(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d)
{
    *a += *b; *d = rotate_left(*d ^ *a, 16);
    *c += *d; *b = rotate_left(*b ^ *c, 12);
    *a += *b; *d = rotate_left(*d ^ *a, 8);
    *c += *d; *b = rotate_left(*b ^ *c, 7);
}

void chacha20_block(uint8_t output[64], const uint8_t key[32],
    uint32_t counter, const uint8_t nonce[12])
{
    uint32_t initial[16], state[16];
    unsigned int i, round;

    initial[0] = UINT32_C(0x61707865);
    initial[1] = UINT32_C(0x3320646e);
    initial[2] = UINT32_C(0x79622d32);
    initial[3] = UINT32_C(0x6b206574);
    for (i = 0; i < 8; i++) {initial[4 + i] = load_le32(key + 4 * i);}
    initial[12] = counter;
    for (i = 0; i < 3; i++) {initial[13 + i] = load_le32(nonce + 4 * i);}
    for (i = 0; i < 16; i++) {state[i] = initial[i];}

    for (round = 0; round < 10; round++) {
        chacha_quarter_round(&state[0], &state[4], &state[8], &state[12]);
        chacha_quarter_round(&state[1], &state[5], &state[9], &state[13]);
        chacha_quarter_round(&state[2], &state[6], &state[10], &state[14]);
        chacha_quarter_round(&state[3], &state[7], &state[11], &state[15]);
        chacha_quarter_round(&state[0], &state[5], &state[10], &state[15]);
        chacha_quarter_round(&state[1], &state[6], &state[11], &state[12]);
        chacha_quarter_round(&state[2], &state[7], &state[8], &state[13]);
        chacha_quarter_round(&state[3], &state[4], &state[9], &state[14]);
    }
    for (i = 0; i < 16; i++) {
        uint32_t word = state[i] + initial[i];
        output[4 * i] = (uint8_t)word;
        output[4 * i + 1] = (uint8_t)(word >> 8);
        output[4 * i + 2] = (uint8_t)(word >> 16);
        output[4 * i + 3] = (uint8_t)(word >> 24);
    }
}

int main(void)
{
    const uint8_t nonce[12] = {0,0,0,9, 0,0,0,0x4a, 0,0,0,0};
    const uint8_t expected[64] = {
        0x10,0xf1,0xe7,0xe4,0xd1,0x3b,0x59,0x15,0x50,0x0f,0xdd,0x1f,0xa3,0x20,0x71,0xc4,
        0xc7,0xd1,0xf4,0xc7,0x33,0xc0,0x68,0x03,0x04,0x22,0xaa,0x9a,0xc3,0xd4,0x6c,0x4e,
        0xd2,0x82,0x64,0x46,0x07,0x9f,0xaa,0x09,0x14,0xc2,0xd7,0x05,0xd9,0x8b,0x02,0xa2,
        0xb5,0x12,0x9c,0xd1,0xde,0x16,0x4e,0xb9,0xcb,0xd0,0x83,0xe8,0xa2,0x50,0x3c,0x4e
    };
    uint8_t key[32];
    unsigned int i;

    chacha_quarter_result[0] = UINT32_C(0x11111111);
    chacha_quarter_result[1] = UINT32_C(0x01020304);
    chacha_quarter_result[2] = UINT32_C(0x9b8d6f43);
    chacha_quarter_result[3] = UINT32_C(0x01234567);
    chacha_quarter_round(&chacha_quarter_result[0], &chacha_quarter_result[1],
        &chacha_quarter_result[2], &chacha_quarter_result[3]);
    if (chacha_quarter_result[0] != UINT32_C(0xea2a92f4) ||
        chacha_quarter_result[1] != UINT32_C(0xcb1cf8ce) ||
        chacha_quarter_result[2] != UINT32_C(0x4581472e) ||
        chacha_quarter_result[3] != UINT32_C(0x5881c4bb)) {return 1;}
    for (i = 0; i < 32; i++) {key[i] = (uint8_t)i;}
    chacha20_block(chacha_result, key, 1, nonce);
    for (i = 0; i < 64; i++) {
        if (chacha_result[i] != expected[i]) {return 2;}
    }
    return 0;
}
