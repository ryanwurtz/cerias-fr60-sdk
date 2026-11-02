/* XTEA: 32 rounds, 64-bit block, 128-bit key. Test fixture only. */
#include <stdint.h>

uint32_t xtea_result[2];
uint32_t xtea_recovered[2];

void xtea_encrypt(uint32_t block[2], const uint32_t key[4])
{
    uint32_t v0 = block[0], v1 = block[1], sum = 0;
    unsigned int i;

    for (i = 0; i < 32; i++) {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
        sum += UINT32_C(0x9e3779b9);
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
    }
    block[0] = v0;
    block[1] = v1;
}

void xtea_decrypt(uint32_t block[2], const uint32_t key[4])
{
    uint32_t v0 = block[0], v1 = block[1], sum = UINT32_C(0xc6ef3720);
    unsigned int i;

    for (i = 0; i < 32; i++) {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
        sum -= UINT32_C(0x9e3779b9);
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
    }
    block[0] = v0;
    block[1] = v1;
}

int main(void)
{
    const uint32_t key[4] = {0, 0, 0, 0};
    uint32_t block[2] = {0, 0};

    xtea_encrypt(block, key);
    xtea_result[0] = block[0];
    xtea_result[1] = block[1];
    /* Zero-key, zero-plaintext known answer, expressed as words. */
    if (block[0] != UINT32_C(0xdee9d4d8) || block[1] != UINT32_C(0xf7131ed9)) {return 1;}
    xtea_decrypt(block, key);
    xtea_recovered[0] = block[0];
    xtea_recovered[1] = block[1];
    if (block[0] != 0 || block[1] != 0) {return 2;}
    return 0;
}
