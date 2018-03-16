/** \file rc4.hpp
 * RC4 symmetric encryption
 *
 * Functions for encrypting using RC4 stream cipher.
 */

#ifndef _RC4_HPP_
#define _RC4_HPP_

#include <cstdint>
#include <cstdio>
#include "error.hpp"

/** \brief RC4 class */
class RC4
{
public:
  /** \brief Function used to encrypt a plaintext into a ciphertext using RC4. 
   *
   * This function encrypts the provided plaintext using the given key in RC4.
   * <HR>
   \param key is a 16-byte long array containing the key.
   \param plaintext is a byte-array containing the plaintext to encrypt.
   \param ciphertext is a byte-array that will contain the encrypted message. It must have at least the same number of cells as plaintext.
   \param p_length is an integer containing the number of bytes to encrypt.
   *
   * <HR>
   */
  static void encrypt(const uint8_t key[16], const uint8_t *plaintext, uint8_t *ciphertext, size_t p_length);
  /** \brief Function used to decrypt a ciphertext into a plaintext using RC4. 
   *
   * This function decrypts the provided ciphertext using the given key in RC4.
   * <HR>
   \param key is a 16-byte long array containing the key.
   \param ciphertext is a byte-array containing the ciphertext to decrypt.
   \param plaintext is a byte-array that will contain the decrypted message. It must have at least the same number of cells as ciphertext
   \param c_length is an integer containing the number of bytes to decrypt.
   *
   * <HR>
   */
  static void decrypt(const uint8_t key[16], const uint8_t *ciphertext, uint8_t *plaintext, size_t c_length);
  /** \brief Auto test function to test RC4 implementation w.r. to few test vectors (from NIST). 
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
  
  private:
  /** \brief RC4 state */
  static uint8_t state[256];
  /** \brief RC4 index i */
  static uint8_t index_i;
  /** \brief RC4 index j */
  static uint8_t index_j;
  /** \brief RC4 key schedule function */
  static void keySchedule(const uint8_t key[16]);
  /** \brief RC4 keystream generation */
  static uint8_t nextKSByte();
   /* Test vectors */
  /** \brief Keys used in test vectors */
  static const uint8_t TV_keys[1][16];
  /** \brief Keystream test vectors */
  static const uint8_t TV_keystreams[6][16];
  /** \brief Offset of Keystream test vectors */
  static const uint32_t TV_offsets[6];
};

#endif
