/** \file aes.hpp
 * AES symmetric encryption
 *
 * Functions for encrypting using AES-128 blockcipher.
 */

#ifndef _AES_HPP_
#define _AES_HPP_

#include <cstdint>
#include <cstdio>
#include "error.hpp"

/** \brief AES class */
class AES
{
public:
  /** \brief Function used to encrypt a plaintext into a ciphertext using AES. 
   *
   * This function encrypts the provided plaintext using the given key in AES-128.
   * <HR>
   \param key is a 16-byte long array containing the key.
   \param plaintext is a 16-byte long array containing the plaintext to encrypt.
   \param ciphertext is a 16-byte long array that will contain the encrypted message.
   *
   * <HR>
   */
  static void encrypt(const uint8_t key[16], const uint8_t plaintext[16], uint8_t ciphertext[16]);
  /** \brief Function used to decrypt a ciphertext into a plaintext using AES. 
   *
   * This function decrypts the provided ciphertext using the given key in AES-128.
   * <HR>
   \param key is a 16-byte long array containing the key.
   \param ciphertext is a 16-byte long array containing the ciphertext to decrypt.
   \param plaintext is a 16-byte long array that will contain the decrypted message.
   *
   * <HR>
   */
  static void decrypt(const uint8_t key[16], const uint8_t ciphertext[16], uint8_t plaintext[16]);
  /** \brief AES-128 key schedule function for encryption */
  static void keySchedule(const uint8_t key[16], uint32_t subkeys[44]);
  /** \brief AES-128 key schedule function for decryption */
  static void keySchedule_inv(const uint8_t key[16], uint32_t subkeys[44]);
  /** \brief AES-128 reverse key schedule : from last subkey returns the master key */
  static void keySchedule_rev(const uint8_t lastkey[16], uint8_t masterkey[16]);
  /** \brief Auto test function to test AES implementation w.r. to few test vectors (from NIST). 
   *
   * This function uses test vectors defined in the class and test implementation versus them.
   * <HR>
   * \return 
   * <ul>
   * <li> #ERROR_UNEXPECTED if a mismatch is found
   * <li> #NO_ERROR if everything matches
   * </ul>
   *
   * <HR>
   */
  static myerror_t test();
  /** \brief AES state pretty printing */
  static void printState(const uint8_t state[16]);
  /** \brief AES state pretty printing */
  static void printState(const uint32_t state[4]);
  /** \brief AES-128 MixColumn operation */
  static void MC(const uint8_t x[4], uint8_t y[4]);
  /** \brief AES-128 inverse MixColumn operation */
  static void MCinv(const uint8_t x[4], uint8_t y[4]);
  /** \brief AES-128 ShiftRows operation */
  static void SR(uint8_t state[16]);
  /** \brief AES-128 inverse ShiftRows operation */
  static void SRinv(uint8_t state[16]);
  /** \brief AES Sbox */
  static const uint8_t SB[256];
  /** \brief AES inverse Sbox */
  static const uint8_t SBinv[256];
private:
  /** \brief AES round function based on big tables */
  static void round(const uint32_t subkey[4], uint32_t state[4]);
  /** \brief AES inverse round function based on big tables */
  static void round_inv(const uint32_t subkey[4], uint32_t state[4]);
  /** \brief GF(2^8) multiplication */
  static uint8_t times(uint8_t a, uint8_t b);
    
  /* Constants */
  /** \brief Big table 0 for encryption: line 0 */
  static const uint32_t Te0[256];
  /** \brief Big table 1 for encryption: line 1 */
  static const uint32_t Te1[256];
  /** \brief Big table 2 for encryption: line 2 */
  static const uint32_t Te2[256];
  /** \brief Big table 3 for encryption: line 3 */
  static const uint32_t Te3[256];
  /** \brief Big table 4 for encryption: used in last round and key schedule */
  static const uint32_t Te4[256];
  
  /** \brief Big table 0 for decryption: line 0 */
  static const uint32_t Td0[256];
  /** \brief Big table 1 for decryption: line 1 */
  static const uint32_t Td1[256];
  /** \brief Big table 2 for decryption: line 2 */
  static const uint32_t Td2[256];
  /** \brief Big table 3 for decryption: line 3 */
  static const uint32_t Td3[256];
  /** \brief Big table 4 for decryption: used in last round and key schedule */
  static const uint32_t Td4[256];
  
  /** \brief Round constants for key schedule */
  static const uint32_t rcon[10];

  /* Test vectors */
  /** \brief Keys used in test vectors */
  static const uint8_t TV_keys[2][16];
  /** \brief Plaintexts used in test vectors */
  static const uint8_t TV_plaintexts[2][16];
  /** \brief Ciphertexts used in test vectors */
  static const uint8_t TV_ciphertexts[2][16];
};

#endif
