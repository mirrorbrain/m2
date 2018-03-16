/** \file key.hpp
 * Key definition
 *
 * Key class definition.
 */

#ifndef KEY_HPP_
#define KEY_HPP_

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "error.hpp"

/** \brief Maximum length of a key. */
constexpr unsigned MAX_KEY_LEN = 64;

/** \brief Key types */
enum key_type_t{
  /** \brief symmetric key */
  KEY_SYM = 1,
  /** \brief public key */
  KEY_PUB = 2,
  /** \brief private key */
  KEY_PRI = 3
};

/** \brief Key use  */
enum key_use_t{
  /** \brief encryption/decryption key (for data) */
  KEY_ENCR = 1,
  /** \brief signature key (for data) */
  KEY_SIGN = 2,
  /** \brief integrity key (for data) */
  KEY_INTE = 3,
  /** \brief wrapping key (for keys) */
  KEY_WRAP = 4
};

typedef uint32_t date_t;

class API;

/** \brief Key class */
class Key
{
public:
  friend class API;
  /** \brief Default constructor: used to create an empty Key before updating it from a JSON string for instance. 
   *
   * <HR>
   */
  Key() : _id(0) , _owner(0) , _type(key_type_t(0)) , _use(key_use_t(0)) , _validity(0) , _associated(0) , _length(0) {};
  /** \brief Key destructor 
   *
   * <HR>
   */
  ~Key() {}
  /** \brief This function set the given attribute to the provided value in class Key.
   *
   * This function is for example used to construct Key classes from a json_object_t.
   * <HR>
   * \param field is a string containing the name of the attribute
   * \param value is the value that should be set to the mentioned attribute.
   * \return 
   * <ul>
   * <li> #NO_ERROR if no error occured 
   * </ul>
   *
   * <HR>
   */
  myerror_t set(const std::string &field, const std::string &value);

  /** \brief Function returning the id of the key 
   *
   * <HR>
   */
  uint32_t getId() const;
  
  /** \brief Function returning the owner of the key 
   *
   * <HR>
   */
  uint32_t getOwner() const;
  
  /** \brief Function returning the type of the key 
   *
   * <HR>
   */
  key_type_t getType() const;
  
  /** \brief Function returning the usage of the key 
   *
   * <HR>
   */
  key_use_t getUse() const;
  
  /** \brief Function returning the validity period of the key 
   *
   * <HR>
   */
  date_t getValidity() const;
  
  /** \brief Function returning the (encrypted) value of the key 
   *
   * <HR>
   */
  std::vector<uint8_t> getValue() const;
  
  /** \brief Serialize the Key object to a byte array of \a _serializedUserSize bytes */
  myerror_t serialize(uint8_t *tab, size_t len);
  
  /** \brief Size of the serialization of a Key object without its value */
  static const size_t _serializedSize = 4 + 4 + 1 + 1 + 4 + 4 + sizeof(size_t);
  
  /** \brief If the corresponding private/public key is in the database: contains its id. */
  uint32_t _associated;
  /** \brief Number of bytes of the key. */
  size_t _length;
private :  
  /** \brief Id of the key, must be unique. */
  uint32_t _id;
  /** \brief Id of the key owner. */
  uint32_t _owner;
  /** \brief Type of the key. */
  key_type_t _type;
  /** \brief Use of the key. */
  key_use_t _use;
  /** \brief Date when the key will not be valid anymore. */
  date_t _validity;
  /** \brief Value of the key. */
  std::vector<uint8_t> _value;
};

  /** \brief This function serializes the Key class into a JSON format string.
   *
   * This function is for example used to dump a Key class when saving the API in a file.
   * <HR>
   * \param os is an output stream in which will be send the serialized User.
   * \param obj is the Key object to serialize.
   * \return the output stream given as argument.
   *
   * <HR>
   */
std::ostream& operator<<(std::ostream& os, const Key& obj);

  /** \brief This function parses a string in JSON format to update a Key class.
   *
   * This function is for example used to construct Key classes when loading the API from a file.
   * <HR>
   * \param is is an input stream from which the serialization of the User will be taken.
   * \param obj is the Key object that will contain the constructed User.
   * \return the input stream given as argument.
   *
   * <HR>
   */
std::istream& operator>>(std::istream& is, Key& obj);

#endif
