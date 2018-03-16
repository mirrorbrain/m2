/** \file sha256.hpp
 * SHA-256 hash function
 *
 * Functions for hashing using SHA-256 hash function.
 */

#ifndef _SHA256_HPP_
#define _SHA256_HPP_

#include <cstdint>
#include <cstring>
#include "error.hpp"


/** \brief SHA256 class */
class SHA256
{
public:
  static const size_t hashSize = 32;
  static const size_t blockSize = 64;
  /** \brief Function used to init the hash module.
   *
   * This function inits the hash module by reseting the state to initial values.
   * <HR>
   *
   * <HR>
   */
  void init();
  /** \brief Function used to add data-block to the processed input.
   *
   * This function process the provided data but outputs nothing.
   * Multiple call to update may be of interest when data is being received in multiple blocks or if the message is too large or the memory.
   * <HR>
   \param data is an array of bytes to process.
   \param length is the number of element in the array data.
   *
   * <HR>
   */
  void update(const uint8_t *data, size_t length);
  /** \brief Function used to finalize the hash procedure.
   *
   * This function stores in the parameter the output of the hashing process applied to the data blocks provided by the calls to the update function.
   * <HR>
   \param hash is a 32-byte array that will contain the final hash value.
   *
   * <HR>
   */
  void final(uint8_t hash[32]);
  /** \brief Auto test function to test SHA-256 implementation w.r. to few test vectors. 
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
  /** \brief SHA-256 compression function */
  void compress(const uint8_t data[64]); 

  /* Context */
  /** \brief State of the hash process */
  uint32_t state[8];
  /** \brief Number of bits processed by the hash function */
  uint64_t dataBitLength;
  /** \brief Data buffer used before enough bytes (64) are available */
  uint8_t dataBuffer[64];
  /** \brief Current number of bytes in the data buffer */
  size_t bufferLength;
  
  /* Constants */
  /** \brief Constants for the compression function */
  static const uint32_t k[64];
  
  /* Test vectors */
  /** \brief Lengths of test vectors */
  static const size_t TV_lengths[4];
  /** \brief Messages used in test vectors */
  static const char *TV_messages[4];
  /** \brief Hashes obtained for test vectors */
  static const uint8_t TV_hashes[4][32];
};

#endif
