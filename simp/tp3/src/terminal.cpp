#include "terminal.hpp"

Terminal::Terminal() : connectedCard(NULL) {
}

Terminal::~Terminal() {
}

myerror_t Terminal::connect(Card *card) {
    myerror_t res = ERROR_UNEXPECTED;

    if (card == NULL)
        res = ERROR_PARAM;
    if (res != ERROR_PARAM) {
        connectedCard	= card;
        res				= NO_ERROR;
    }
    return res;
}

#ifdef __i386
extern __inline__ uint64_t rdtsc(void) {
    uint64_t x;

    __asm__ volatile ("rdtsc" : "=A" (x));
    return x;
}

#elif defined __amd64
extern __inline__ uint64_t rdtsc(void) {
    uint64_t a, d;

    __asm__ volatile ("rdtsc" : "=a" (a), "=d" (d));
    return (d << 32) | a;
}

#endif

myerror_t Terminal::measurePinResponse(uint64_t *time, uint8_t pin[4]) {
    myerror_t res = ERROR_UNEXPECTED;

    if ((time == NULL) || (pin == NULL) || (connectedCard == NULL))
        res = ERROR_PARAM;
    if (res != ERROR_PARAM) {
        uint64_t tic = rdtsc();
        res = connectedCard->unlock(pin);
        uint64_t tac = rdtsc();
        *time = tac - tic;
    }
    return res;
}

myerror_t Terminal::computeAES(uint8_t plaintext[16], uint8_t ciphertext[16]) {
    myerror_t res = ERROR_UNEXPECTED;

    if (connectedCard == NULL)
        res = ERROR_PARAM;
    else
        res = connectedCard->AES(plaintext, ciphertext);
    return res;
}

myerror_t Terminal::timeAES(uint8_t plaintext[16], uint8_t ciphertext[16], uint64_t *time) {
    myerror_t res = ERROR_UNEXPECTED;

    if (connectedCard == NULL || time == NULL)
        res = ERROR_PARAM;
    else{
        uint64_t tic = rdtsc();
        res = connectedCard->AES(plaintext, ciphertext);
        uint64_t tac = rdtsc();
        *time = tac - tic;
    }
    return res;
}

myerror_t Terminal::measureAES(uint8_t plaintext[16], uint8_t ciphertext[16], uint8_t trace[16]) {
    myerror_t res = ERROR_UNEXPECTED;

    if (connectedCard == NULL)
        res = ERROR_PARAM;
    else
        res = connectedCard->leakyAES(plaintext, ciphertext, trace);
    return res;
}

myerror_t Terminal::tamperAES(uint8_t plaintext[16], uint8_t ciphertext[16]) {
    myerror_t res = ERROR_UNEXPECTED;

    if (connectedCard == NULL)
        res = ERROR_PARAM;
    else
        res = connectedCard->faultyAES(plaintext, ciphertext);
    return res;
}
