/** \file aes.hpp
 * AES symmetric encryption
 *
 * Functions for encrypting using AES-128 blockcipher.
 */

#ifndef _HMAC_HPP_
#define _HMAC_HPP_

#include <cstdint>
#include <cstddef>
#include "error.hpp"

#include "sha256.hpp"

/** \brief HMAC class */
/** At the moment uses SHA-256 hash function but should be init with any hash function in a future implementation. */
class HMAC
{
public:
  static constexpr size_t tagLength = 32;
  /** \brief Function used to init the mac module.
   *
   * This function inits the hash module and starts hashing the key xored to ipad.
   * <HR>
   * \param key is an array of bytes containing the integrity key.
   * \param length is the number key bytes.
   *
   * <HR>
   */
  void init(const uint8_t *key, size_t length);
  /** \brief Function used to add data-block to the processed input.
   *
   * This function process the provided data but outputs nothing.
   * Multiple call to update may be of interest when data is being received in multiple blocks or if the message is too large or the memory.
   * <HR>
   * \param data is an array of bytes to process.
   * \param length is the number of element in the array data.
   *
   * <HR>
   */
  void update(const uint8_t *data, size_t length);
  /** \brief Function used to finalize the mac procedure.
   *
   * This function stores in the parameter the tag computed from the data blocks provided by the calls to the update function.
   * <HR>
   * \param hash is an array that will contain the final tag value. Enough space should have been allocated to contain the tag value which size depends on the hash function used.
   *
   * <HR>
   */
  void final(uint8_t *tag);
  /** \brief Auto test function to test HMAC implementation w.r. to few test vectors (SHA-256). 
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
  /* Context */
  /** \brief Hash function used. */
  SHA256 *hashFunction;
  /** \brief Length of the key used. */
  size_t keyLength;
  /** \brief Key used. */
  const uint8_t *keyValue;
  
  /* Constants */
  /** \brief Constant used for inner padding */
  static const uint8_t ipad;
  /** \brief Constant used for outer padding */
  static const uint8_t opad;
  
  /* Test vectors */
  /** \brief Lengths of test vectorkeys */
  static const size_t TV_klengths[3];
  /** \brief Keys used in test vectors */
  static const uint8_t TV_keys[3][256];
  /** \brief Lengths of test vector messages */
  static const size_t TV_mlengths[3];
  /** \brief Messages used in test vectors */
  static const uint8_t TV_messages[3][128];
  /** \brief Hashes obtained for test vectors */
  static const uint8_t TV_tags[3][tagLength];
};

#endif
