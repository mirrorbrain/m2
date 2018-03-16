#include "rc4.hpp"

#include <cstdio>

uint8_t RC4::state[256];
uint8_t RC4::index_i = 0;
uint8_t RC4::index_j = 0;

myerror_t RC4::test() {
    myerror_t err = ERROR_UNEXPECTED;
    unsigned  byte_idx = 0, tv_idx = 0;
    uint8_t   result[16];

    keySchedule(TV_keys[0]);

    for (tv_idx = 0; tv_idx < 6; tv_idx++) {
        while (byte_idx < TV_offsets[tv_idx]) {
            nextKSByte();
            byte_idx++;
        }

        unsigned matching = 0;
        for (unsigned i = 0; i < 16; i++) {
            if (nextKSByte() == TV_keystreams[tv_idx][i])
                matching++;
            byte_idx++;
        }
        if (matching != 16)
            goto end;
    }
    err = NO_ERROR;

end:
    return err;
}

void RC4::encrypt(const uint8_t key[16], const uint8_t *plaintext, uint8_t *ciphertext, size_t p_length) {
    keySchedule(key);
    for (unsigned i = 0; i < p_length; i++)
        ciphertext[i] = plaintext[i] ^ nextKSByte();
}

void RC4::decrypt(const uint8_t key[16], const uint8_t *ciphertext, uint8_t *plaintext, size_t c_length) {
    encrypt(key, ciphertext, plaintext, c_length);
}

void RC4::keySchedule(const uint8_t key[16]) {
    unsigned temp;

    index_i = 0;
    do {
        state[index_i] = index_i;
        index_i++;
    } while (index_i != 0);

    index_j = 0;
    do {
        index_j        = (index_j + state[index_i] + key[index_i % 16]) & 0xFF;
        temp           = state[index_i];
        state[index_i] = state[index_j];
        state[index_j] = temp;
        index_i++;
    } while (index_i != 0);
    index_j = 0;
}

uint8_t RC4::nextKSByte() {
    uint8_t res, temp;

    index_i        = (index_i + 1) & 0xFF;
    index_j        = (index_j + state[index_i]) & 0xFF;
    temp           = state[index_i];
    state[index_i] = state[index_j];
    state[index_j] = temp;
    res            = state[(state[index_i] + state[index_j]) & 0xFF];
    return res;
}

/* Test Vectors */

const uint8_t RC4::TV_keys[1][16] = {
    { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
      0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10 }
};

const uint8_t RC4::TV_keystreams[6][16] = {
    { 0x9a, 0xc7, 0xcc, 0x9a, 0x60, 0x9d, 0x1e, 0xf7,
      0xb2, 0x93, 0x28, 0x99, 0xcd, 0xe4, 0x1b, 0x97 },
    { 0x52, 0x48, 0xc4, 0x95, 0x90, 0x14, 0x12, 0x6a,
      0x6e, 0x8a, 0x84, 0xf1, 0x1d, 0x1a, 0x9e, 0x1c },
    { 0x06, 0x59, 0x02, 0xe4, 0xb6, 0x20, 0xf6, 0xcc,
      0x36, 0xc8, 0x58, 0x9f, 0x66, 0x43, 0x2f, 0x2b },
    { 0xd3, 0x9d, 0x56, 0x6b, 0xc6, 0xbc, 0xe3, 0x01,
      0x07, 0x68, 0x15, 0x15, 0x49, 0xf3, 0x87, 0x3f },
    { 0xb6, 0xd1, 0xe6, 0xc4, 0xa5, 0xe4, 0x77, 0x1c,
      0xad, 0x79, 0x53, 0x8d, 0xf2, 0x95, 0xfb, 0x11 },
    { 0xc6, 0x8c, 0x1d, 0x5c, 0x55, 0x9a, 0x97, 0x41,
      0x23, 0xdf, 0x1d, 0xbc, 0x52, 0xa4, 0x3b, 0x89 }
};

const uint32_t RC4::TV_offsets[6] = {
    0, 16, 240, 256, 496, 512
};
