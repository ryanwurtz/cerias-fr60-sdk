/* SHA-256, FIPS 180-4. One-shot hash with explicit big-endian I/O.
 * https://csrc.nist.gov/pubs/fips/180-4/upd1/final
 * Test fixture only. No libc or allocation required.
 */
#include <stddef.h>
#include <stdint.h>

uint8_t sha256_result[32];

static const uint32_t round_constants[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

static uint32_t rotate_right(uint32_t x, unsigned int n)
{
    return (x >> n) | (x << (32 - n));
}

void sha256_compress(uint32_t state[8], const uint8_t block[64])
{
    uint32_t w[64], a,b,c,d,e,f,g,h;
    unsigned int i;

    for (i = 0; i < 16; i++) {
        w[i] = ((uint32_t)block[4*i] << 24) | ((uint32_t)block[4*i+1] << 16) |
            ((uint32_t)block[4*i+2] << 8) | (uint32_t)block[4*i+3];
    }
    for (i = 16; i < 64; i++) {
        uint32_t s0 = rotate_right(w[i-15],7) ^ rotate_right(w[i-15],18) ^ (w[i-15] >> 3);
        uint32_t s1 = rotate_right(w[i-2],17) ^ rotate_right(w[i-2],19) ^ (w[i-2] >> 10);
        w[i] = w[i-16] + s0 + w[i-7] + s1;
    }
    a=state[0]; b=state[1]; c=state[2]; d=state[3];
    e=state[4]; f=state[5]; g=state[6]; h=state[7];
    for (i = 0; i < 64; i++) {
        uint32_t s1 = rotate_right(e,6) ^ rotate_right(e,11) ^ rotate_right(e,25);
        uint32_t choose = (e & f) ^ (~e & g);
        uint32_t t1 = h + s1 + choose + round_constants[i] + w[i];
        uint32_t s0 = rotate_right(a,2) ^ rotate_right(a,13) ^ rotate_right(a,22);
        uint32_t majority = (a & b) ^ (a & c) ^ (b & c);
        uint32_t t2 = s0 + majority;
        h=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
    }
    state[0]+=a; state[1]+=b; state[2]+=c; state[3]+=d;
    state[4]+=e; state[5]+=f; state[6]+=g; state[7]+=h;
}

void sha256_hash(uint8_t output[32], const uint8_t *message, size_t length)
{
    uint32_t state[8] = {
        0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
        0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19
    };
    uint8_t block[64];
    uint64_t bits = (uint64_t)length * 8;
    size_t remaining = length;
    unsigned int i, used;

    while (remaining >= 64) {
        sha256_compress(state, message);
        message += 64;
        remaining -= 64;
    }
    used = (unsigned int)remaining;
    for (i = 0; i < used; i++) {block[i] = message[i];}
    block[used++] = 0x80;
    if (used > 56) {
        while (used < 64) {block[used++] = 0;}
        sha256_compress(state, block);
        used = 0;
    }
    while (used < 56) {block[used++] = 0;}
    for (i = 0; i < 8; i++) {block[63-i] = (uint8_t)(bits >> (8*i));}
    sha256_compress(state, block);
    for (i = 0; i < 32; i++) {output[i] = (uint8_t)(state[i/4] >> (24-8*(i%4)));}
}

int main(void)
{
    const uint8_t empty_digest[32] = {
        0xe3,0xb0,0xc4,0x42,0x98,0xfc,0x1c,0x14,0x9a,0xfb,0xf4,0xc8,0x99,0x6f,0xb9,0x24,
        0x27,0xae,0x41,0xe4,0x64,0x9b,0x93,0x4c,0xa4,0x95,0x99,0x1b,0x78,0x52,0xb8,0x55
    };
    const uint8_t abc_digest[32] = {
        0xba,0x78,0x16,0xbf,0x8f,0x01,0xcf,0xea,0x41,0x41,0x40,0xde,0x5d,0xae,0x22,0x23,
        0xb0,0x03,0x61,0xa3,0x96,0x17,0x7a,0x9c,0xb4,0x10,0xff,0x61,0xf2,0x00,0x15,0xad
    };
    const uint8_t long_message[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    const uint8_t long_digest[32] = {
        0x24,0x8d,0x6a,0x61,0xd2,0x06,0x38,0xb8,0xe5,0xc0,0x26,0x93,0x0c,0x3e,0x60,0x39,
        0xa3,0x3c,0xe4,0x59,0x64,0xff,0x21,0x67,0xf6,0xec,0xed,0xd4,0x19,0xdb,0x06,0xc1
    };
    unsigned int i;

    sha256_hash(sha256_result, (const uint8_t *)"", 0);
    for (i = 0; i < 32; i++) {if (sha256_result[i] != empty_digest[i]) {return 1;}}
    sha256_hash(sha256_result, (const uint8_t *)"abc", 3);
    for (i = 0; i < 32; i++) {if (sha256_result[i] != abc_digest[i]) {return 2;}}
    sha256_hash(sha256_result, long_message, sizeof(long_message)-1);
    for (i = 0; i < 32; i++) {if (sha256_result[i] != long_digest[i]) {return 3;}}
    return 0;
}
