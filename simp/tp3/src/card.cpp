#include "card.hpp"
#include "aes.hpp"

#include <cstdio>
const unsigned Card::defaultPasswordCount = 50;

Card::Card() : isLocked(true), passwordCount(defaultPasswordCount) {
    srand(time(NULL));
    for (unsigned i = 0; i < 4; i++)
        pinCode[i] = rand() % 10;
    for (unsigned i = 0; i < 16; i++)
        aesKey[i] = rand() & 0xFF;
}

myerror_t Card::unlock(uint8_t code[4]) {
    myerror_t         err = ERROR_UNEXPECTED;
    volatile unsigned hack;

    if (passwordCount) {
        passwordCount--;
        err = NO_ERROR;
        for (unsigned i = 0; i < 4; i++) {
            for (hack = 0; hack < (1u << 10); hack++);
            if (code[i] != pinCode[i]) {
                err = ERROR_PARAM;
                break;
            }
        }
    }
    else
        err = ERROR_SEC;
    if (err == NO_ERROR) {
        passwordCount = defaultPasswordCount;
        isLocked      = false;
    }
    return err;
}

myerror_t Card::AES(const uint8_t plaintext[16], uint8_t ciphertext[16]) {
    myerror_t err = ERROR_UNEXPECTED;

    if ((plaintext == NULL) || (ciphertext == NULL))
        err = ERROR_PARAM;
    if (isLocked)
        err = ERROR_SEQ;
    if ((err != ERROR_PARAM) && (err != ERROR_SEQ)) {
        AES::encrypt(aesKey, plaintext, ciphertext);
        err = NO_ERROR;
    }
    return err;
}

myerror_t Card::leakyAES(const uint8_t plaintext[16], uint8_t ciphertext[16], uint8_t trace[16]) {
    myerror_t err = ERROR_UNEXPECTED;

    if ((plaintext == NULL) || (ciphertext == NULL) || (trace == NULL))
        err = ERROR_PARAM;
    if (isLocked)
        err = ERROR_SEQ;
    if ((err != ERROR_PARAM) && (err != ERROR_SEQ)) {
        AES::encrypt(aesKey, plaintext, ciphertext);
        for (unsigned i = 0; i < 16; i++)
            trace[i] = 10 * __builtin_popcount(AES::SB[plaintext[i] ^ aesKey[i]]) + (rand() % 40);
        err = NO_ERROR;
    }
    return err;
}

myerror_t Card::faultyAES(const uint8_t plaintext[16], uint8_t ciphertext[16]) {
    myerror_t err = ERROR_UNEXPECTED;

    if ((plaintext == NULL) || (ciphertext == NULL))
        err = ERROR_PARAM;
    if (isLocked)
        err = ERROR_SEQ;
    if ((err != ERROR_PARAM) && (err != ERROR_SEQ)) {
        unsigned i = 0, j = 0;
        unsigned round = 0;
        uint32_t subkeys[44];
        uint8_t  y[16], z[4];

        for (i = 0; i < 16; i++)
            ciphertext[i] = plaintext[i];
        AES::keySchedule(aesKey, subkeys);

        /* AddKey */
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                ciphertext[4 * i + j] ^= (subkeys[i] >> (8 * (3 - j))) & 0xFF;

        for (round = 1; round < 10; round++) {
            /* SubBytes */
            for (i = 0; i < 16; i++)
                ciphertext[i] = AES::SB[ciphertext[i]];

            /* ShiftRows */
            AES::SR(ciphertext);

            if (round == 9) {             // here a fault should occur
                unsigned byte_position = rand() & 0xF;
                unsigned byte_fault    = 1;             //rand() &0xFF; // no fault may occur
                ciphertext[byte_position] ^= byte_fault;
            }

            /* MixColumns */
            for (i = 0; i < 4; i++) {
                AES::MC(&(ciphertext[4 * i]), z);
                for (j = 0; j < 4; j++)
                    ciphertext[4 * i + j] = z[j];
            }

            /* AddKey */
            for (i = 0; i < 4; i++)
                for (j = 0; j < 4; j++)
                    ciphertext[4 * i + j] ^= (subkeys[4 * round + i] >> (8 * (3 - j))) & 0xFF;
        }

        /* Last Round */
        /* SubBytes */
        for (i = 0; i < 16; i++)
            ciphertext[i] = AES::SB[ciphertext[i]];
        /* ShiftRows */
        AES::SR(ciphertext);
        /* AddKey */
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                ciphertext[4 * i + j] ^= (subkeys[4 * round + i] >> (8 * (3 - j))) & 0xFF;
        err = NO_ERROR;
    }
    return err;
}
