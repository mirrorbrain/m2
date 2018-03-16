/** \file card.hpp
 * Card object
 *
 * Card class emulating a smart card under power monitoring.
 */

#ifndef _CARD_HPP_
#define _CARD_HPP_

#include <cstdint>
#include <cstdlib>
#include <ctime>
#include "error.hpp"

class Card
{
public:
  Card(); 
  ~Card() {}
  myerror_t unlock(uint8_t code[4]);
  myerror_t AES(const uint8_t plaintext[16], uint8_t ciphertext[16]);
  myerror_t leakyAES(const uint8_t plaintext[16], uint8_t ciphertext[16], uint8_t trace[16]);
  myerror_t faultyAES(const uint8_t plaintext[16], uint8_t ciphertext[16]);
private:
  bool isLocked;
  unsigned passwordCount;
  uint8_t pinCode[4];
  uint8_t aesKey[16];
  static const unsigned defaultPasswordCount;
};
  
#endif
