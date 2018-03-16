/** \file user.hpp
 * User definition
 *
 * User class definition.
 */

#ifndef USER_HPP_
#define USER_HPP_

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include "error.hpp"
#include "sha256.hpp"

/** \brief Maximum length of a user login. */
constexpr unsigned MAX_LOGIN_LEN = 30;
/** \brief Maximum length of a user password. */
constexpr unsigned MAX_PASSW_LEN = 50;

/** \brief User roles
 *
 * This enumerated type has positive values smaller than 256.
 * Hence, it can be converted to uint8_t type for dumping.
 */
enum role_t{
  /** \brief simple user */
  USER_BASIC = 0,
  /** \brief admin */
  USER_ADMIN = 1
};

/** \brief User class */
class User
{
  /* For I/O */
  friend std::ostream& operator<<(std::ostream& os, const User& obj);
  
public:
  /** \brief Default constructor: used to create an empty User before updating it from a JSON string for instance. 
   *
   * <HR>
   */
  User() : _id(0) , _role(USER_BASIC) {};
  /** \brief This constructor creates a user from a login, id and role 
   *
   * <HR>
   */
  User(const char login[MAX_LOGIN_LEN], const char password[MAX_PASSW_LEN], uint32_t id, role_t role);
  /** \brief User destructor 
   *
   * <HR>
   */
  ~User();
  /** \brief This function sets the given attribute to the provided value in class User.
   *
   * This function is for example used to construct User classes from a json_object_t.
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

  /** \brief Function returning the id of the user 
   *
   * <HR>
   */
  uint32_t getId() const;

  /** \brief Function returning the login of the user 
   *
   * <HR>
   */
  const char* getLogin() const;

  /** \brief Function returning the role of the user 
   *
   * <HR>
   */
  const role_t getRole() const;
  
  /** \brief Function comparing two users' logins 
   *
   * <HR>
   */
  bool hasSameLogin(const std::string &login) const;

  /** brief Function comparing provided password to the user one */
  bool isCorrectPassword(const std::string &password) const;

  /** \brief Serialize the User object to a byte array of \a _serializedUserSize bytes */
  myerror_t serialize(uint8_t *tab, size_t len);

  /** \brief Size of the hashed password */
  static const size_t _hashPasswSize = SHA256::hashSize;
  /** \brief Size of the serialization of a User object */
  static const size_t _serializedSize = 4 + MAX_LOGIN_LEN + _hashPasswSize + 1;
private:
  /** \brief Id of the user, must be unique. */
  uint32_t _id;
  /** \brief Login of the user. 
   *
   * The size of a login is at most #MAX_LOGIN_LEN.
   */
  char _login[MAX_LOGIN_LEN+1];
  /** \brief Role of the user. */
  role_t _role;
  /** \brief Password (hashed password) */
  uint8_t _password[_hashPasswSize+1];
  /** \brief Hashes the password */
  static void hashPassword(const char password[MAX_PASSW_LEN], uint8_t digest[_hashPasswSize]);
};

  /** \brief This function serializes the User class into a JSON format string.
   *
   * This function is for example used to dump a User class when saving the API in a file.
   * <HR>
   * \param os is an output stream in which will be send the serialized User.
   * \param obj is the User object to serialize.
   * \return the output stream given as argument.
   *
   * <HR>
   */
std::ostream& operator<<(std::ostream& os, const User& obj);

  /** \brief This function parses a string in JSON format to update a User class.
   *
   * This function is for example used to construct User classes when loading the API from a file.
   * <HR>
   * \param is is an input stream from which the serialization of the User will be taken.
   * \param obj is the User object that will contain the constructed User.
   * \return the input stream given as argument.
   *
   * <HR>
   */
std::istream& operator>>(std::istream& is, User& obj);

#endif
