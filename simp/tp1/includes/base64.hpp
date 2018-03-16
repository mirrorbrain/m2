/** \file base64.hpp
 * Base 64 encoding an decoding utilities
 *
 * Functions for encoding and decoding from bytes to base 64.
 * Code based on part of the libb64 project which has been placed in the public domain.
 */

#ifndef _BASE64_HPP_
#define _BASE64_HPP_

#include <cstdint>
#include <cstddef>
#include "error.hpp"

typedef enum    {      step_A, step_B, step_C, step_D    }
  base64_step_t;

typedef struct
{
  base64_step_t step;
  uint8_t result;
} encode_state_t;

typedef struct
{
  base64_step_t step;
  uint8_t plainchar;
} decode_state_t;

/** \brief BASE64 class */
class BASE64
{
public:
  /** \brief Function used to encode a byte array.
   *
   * This function process the provided data to output its base64-encoded value.
   * <HR>
   * \param data_in is an array of bytes to process.
   * \param length_in is the number of bytes in the array data_in.
   * \param data_out is a string that will contain the encoded result.
   * \param length_out will be updated to contain the actual size of the encoded result.
   *
   * <HR>
   */ 
  void encode(const uint8_t *data_in, size_t length_in, char* data_out, size_t *length_out);
  /** \brief Function used to decode a base64-encoded string.
   *
   * This function process the provided string to output the corresponding value in a byte array.
   * <HR>
   * \param data_in is the string to process.
   * \param length_in is the number of characters to consider in the string.
   * \param data_out is a byte array that will contain the decoded value.
   * \param length_out will be updated to contain the actual byte-size of the decoded value.
   *
   * <HR>
   */ 
  void decode(const char *data_in, size_t length_in, uint8_t* data_out, size_t *length_out);
    /** \brief Auto test function to test Base 64 conversion w.r.t. randomly generated data. 
   *
   * This function encodes and decodes some random data then checks the original value is recovered.
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
  encode_state_t encodeState;
  decode_state_t decodeState;

  void initEncoding();
  void initDecoding();
  static char encodeValue(uint8_t value_in);
  static int8_t decodeValue(char value_in);
};

#endif
