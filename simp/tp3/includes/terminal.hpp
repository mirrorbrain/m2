/** \file terminal.hpp
 * Terminal object
 *
 * Terminal class emulating a smart terminal under power monitoring.
 */

#ifndef _TERMINAL_HPP_
#define _TERMINAL_HPP_

#include <cstdint>
#include <ctime>
#include "error.hpp"
#include "card.hpp"

/** \brief Terminal class */
class Terminal
{
public:
  /** \brief The constructor of the terminal.
   *
   * <HR>
   */
  Terminal();
  /** \brief The destructor of the terminal.
   *
   * <HR>
   */
  ~Terminal();
  /** \brief Plug a card in the terminal.
   *
   * <HR>
   * \param card is a pointer to the card to connect.
   * \return 
   * <ul>
   * <li> #NO_ERROR if no error occured
   * <li> #ERROR_PARAM if the pointer is null
   * </ul>
   *
   * <HR>
   */
  myerror_t connect(Card *card);
  /** \brief Test a PIN code and measure response time.
   *
   * <HR>
   * \param time is a pointer to the variable where the response time will be returned.
   * \param pin is the PIN code to test.
   * \return 
   * <ul>
   * <li> #NO_ERROR if no error occured
   * <li> #ERROR_PARAM if at least one of the pointers is null
   * </ul>
   *
   * <HR>
   */
  myerror_t measurePinResponse(uint64_t *time, uint8_t pin[4]);
  
      /** \brief Send an encrypt request to the card.
   *
   * <HR>
   * \param plaintext is the plaintext to encrypt
   * \param ciphertext is an array that will contain the corresponding ciphertext
   * \return 
   * <ul>
   * <li> #NO_ERROR if no error occured
   * <li> #ERROR_PARAM if at least one of the pointers is null
   * </ul>
   *
   * <HR>
   */
  myerror_t computeAES(uint8_t plaintext[16], uint8_t ciphertext[16]);

  
  /** \brief Send an encrypt request to the card and measure its time response.
   *
   * <HR>
   * \param plaintext is the plaintext to encrypt
   * \param ciphertext is an array that will contain the corresponding ciphertext
   * \param time is a pointer to the variable where the response time will be returned.
   * \return 
   * <ul>
   * <li> #NO_ERROR if no error occured
   * <li> #ERROR_PARAM if at least one of the pointers is null
   * </ul>
   *
   * <HR>
   */
  myerror_t timeAES(uint8_t plaintext[16], uint8_t ciphertext[16], uint64_t *time);
  
  /** \brief Send an encrypt request to the card and measure its power consumption.
   *
   * <HR>
   * \param plaintext is the plaintext to encrypt
   * \param ciphertext is an array that will contain the corresponding ciphertext
   * \param trace is an array that will contain the power consumption of the card during the first AES round.
   * \return 
   * <ul>
   * <li> #NO_ERROR if no error occured
   * <li> #ERROR_PARAM if at least one of the pointers is null
   * </ul>
   *
   * <HR>
   */
  myerror_t measureAES(uint8_t plaintext[16], uint8_t ciphertext[16], uint8_t trace[16]);
    /** \brief Send an encrypt request to the card and generate a fault in the last but one round.
   *
   * <HR>
   * \param plaintext is the plaintext to encrypt
   * \param ciphertext is an array that will contain the corresponding faulted ciphertext
   * \return 
   * <ul>
   * <li> #NO_ERROR if no error occured
   * <li> #ERROR_PARAM if at least one of the pointers is null
   * </ul>
   *
   * <HR>
   */
  myerror_t tamperAES(uint8_t plaintext[16], uint8_t ciphertext[16]);
private:
  /** \brief Card currently plugged in the terminal */ 
  Card *connectedCard;
};
  
#endif
 
