/** \file error.hpp
 * Error codes definition
 *
 * Error codes definition
 */

#ifndef ERROR_HPP_
#define ERROR_HPP_

/** \brief Error codes
 *
 * */
enum myerror_t{
  /** \brief evertything is ok */
  NO_ERROR = 0,
  /** \brief an I/O error occured */
  ERROR_IO,
  /** \brief provided parameters do not match pre-conditions*/
  ERROR_PARAM,
  /** \brief the data provided did not match the expected format */
  ERROR_FORMAT,
  /** \brief a security error occured */
  ERROR_SEC,
  /** \brief a sequencing error occured */
  ERROR_SEQ,
   /** \brief a memory error occured */
  ERROR_MEM,
  /** \brief an unexpected error occured */
  ERROR_UNEXPECTED,
};

#endif
