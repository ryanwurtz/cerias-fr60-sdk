/* Poly1305 one-shot authenticator. RFC 8439 section 2.5.2 test vector.
 * https://www.rfc-editor.org/rfc/rfc8439.html
 * Test fixture only: deliberately simple base-2^13 limbs, not constant-time.
 */
#include <stddef.h>
#include <stdint.h>

uint8_t poly1305_result[16];

/* Unpack little-endian bytes into ten 13-bit limbs. */
static void unpack_limbs(uint32_t limbs[10], const uint8_t *bytes, unsigned int length)
{
    unsigned int i;
    for (i = 0; i < 10; i++) {limbs[i] = 0;}
    for (i = 0; i < length * 8; i++) {
        limbs[i/13] |= ((uint32_t)((bytes[i/8] >> (i%8)) & 1)) << (i%13);
    }
}

void poly1305_auth(uint8_t tag[16], const uint8_t *message, size_t length,
    const uint8_t key[32])
{
    uint8_t clamped[16];
    uint32_t r[10], h[10], block[10];
    unsigned int i, j;
    uint32_t carry;

    for (i = 0; i < 16; i++) {clamped[i] = key[i];}
    clamped[3] &= 15; clamped[7] &= 15;
    clamped[11] &= 15; clamped[15] &= 15;
    clamped[4] &= 252; clamped[8] &= 252; clamped[12] &= 252;
    unpack_limbs(r, clamped, 16);
    for (i = 0; i < 10; i++) {h[i] = 0;}

    while (length != 0) {
        unsigned int count = length < 16 ? (unsigned int)length : 16;
        uint64_t product[10];
        uint64_t overflow;

        unpack_limbs(block, message, count);
        /* Append the mandatory 1 bit, even for a full 16-byte block. */
        block[(count*8)/13] |= UINT32_C(1) << ((count*8)%13);
        for (i = 0; i < 10; i++) {
            h[i] += block[i];
            product[i] = 0;
        }
        for (i = 0; i < 10; i++) {
            for (j = 0; j < 10; j++) {
                uint64_t term = (uint64_t)h[i] * r[j];
                unsigned int n = i + j;
                /* 2^130 == 5 modulo 2^130 - 5. */
                if (n >= 10) {product[n-10] += term * 5;}
                else {product[n] += term;}
            }
        }
        do {
            for (i = 0; i < 9; i++) {
                product[i+1] += product[i] >> 13;
                product[i] &= 8191;
            }
            overflow = product[9] >> 13;
            product[9] &= 8191;
            product[0] += overflow * 5;
        } while (overflow != 0);
        for (i = 0; i < 10; i++) {h[i] = (uint32_t)product[i];}
        message += count;
        length -= count;
    }

    /* h is below 2^130: subtract p once if h >= p. */
    for (i = 9; i != 0 && h[i] == 8191; i--) {}
    if (i == 0 && h[0] >= 8187) {
        h[0] -= 8187;
        for (i = 1; i < 10; i++) {h[i] = 0;}
    }
    for (i = 0; i < 16; i++) {tag[i] = 0;}
    for (i = 0; i < 128; i++) {
        tag[i/8] |= (uint8_t)(((h[i/13] >> (i%13)) & 1) << (i%8));
    }
    carry = 0;
    for (i = 0; i < 16; i++) {
        uint32_t sum = (uint32_t)tag[i] + key[16+i] + carry;
        tag[i] = (uint8_t)sum;
        carry = sum >> 8;
    }
}

int main(void)
{
    const uint8_t key[32] = {
        0x85,0xd6,0xbe,0x78,0x57,0x55,0x6d,0x33,0x7f,0x44,0x52,0xfe,0x42,0xd5,0x06,0xa8,
        0x01,0x03,0x80,0x8a,0xfb,0x0d,0xb2,0xfd,0x4a,0xbf,0xf6,0xaf,0x41,0x49,0xf5,0x1b
    };
    const uint8_t message[] = "Cryptographic Forum Research Group";
    const uint8_t expected[16] = {
        0xa8,0x06,0x1d,0xc1,0x30,0x51,0x36,0xc6,0xc2,0x2b,0x8b,0xaf,0x0c,0x01,0x27,0xa9
    };
    const uint8_t zero_key[32] = {0};
    const uint8_t full_block[16] = {0};
    unsigned int i;

    poly1305_auth(poly1305_result, message, sizeof(message)-1, key);
    for (i = 0; i < 16; i++) {if (poly1305_result[i] != expected[i]) {return 1;}}
    /* Empty messages produce s, the second half of the key. */
    poly1305_auth(poly1305_result, message, 0, key);
    for (i = 0; i < 16; i++) {if (poly1305_result[i] != key[16+i]) {return 2;}}
    poly1305_auth(poly1305_result, full_block, sizeof(full_block), zero_key);
    for (i = 0; i < 16; i++) {if (poly1305_result[i] != 0) {return 3;}}
    /* Leave the RFC tag in the result buffer for inspection. */
    poly1305_auth(poly1305_result, message, sizeof(message)-1, key);
    return 0;
}
