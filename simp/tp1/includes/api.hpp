/** \file api.hpp
 * API definition
 *
 * API definition 
 */

#ifndef API_HPP_
#define API_HPP_

#include <vector>

#include "error.hpp"
#include "user.hpp"
#include "key.hpp"

/** \brief API class */
class API
{
public:
  /** \brief The constructor initialises API from the file given in parameter. 
   *
   * <HR>
   * \param filename string containing the relative path and filename of the file containing the protected memory content to load.
   *
   * <HR>
   */
  API(const char *fileUsers, const char *fileKeys);

/** \brief The API destructor
   *
   * Automatically dump current API memory to the file used for constructing the object.
   * <HR>
   */
  ~API();

  /** \brief Resets the API
   *
   * Resets the API by deleting users and memory.
   * An admin account is created with default password s3cr3t_p@ss
   * <HR>
   */
  void reset();

  /** \brief Register a new user to the API
   *
   * Add the user to the list of API users.
   * <HR>
   * \param login is a string containing the login of the user to create
   * \param password contains the corresponding password
   * \param id is a pointer to a variable that will get the id of the created user or be set to 0 in case of error (id could be NULL)
   * \return 
   * <ul>
   * <li> #NO_ERROR if no error occured </li>
   * <li> #ERROR_SEC if current user has not admin rights </li>
   * <li> #ERROR_SEQ if nobody is connected </li>
   * <li> #ERROR_PARAM if user is already in the base </li>
   * </ul>
   *
   * <HR>
   */
  myerror_t newUser(const std::string &login, const std::string &password, uint32_t *id);

 /** \brief Delete the user from the API
   *
   * Deleting a user from the user list (admin rights required).
   * <HR>
   * \param id is the id of the user to delete
   * \return 
   * <ul>
   * <li> #NO_ERROR if no error occured </li>
   * <li> #ERROR_SEC if connected user is not admin or if he tries to delete himself </li>
   * <li> #ERROR_PARAM if user is not in the base </li>
   * </ul>
   *
   * <HR>
   */
  myerror_t deleteUser(uint32_t id);
  
  /** \brief Randomly generate a key and add it to the memory
   *
   * <HR>
   * \param type should be one of the key_type_t values
   * \param use should be one of the key_use_t values
   * \param validity could be anything
   * \param length should not be 0 and should not exceed the maximal size of a key #MAX_KEY_LEN
   * \param id will be set to 0 in case of error or will contain the newly created key id (could be NULL)
   * \return
   * <ul>
   * <li> #NO_ERROR if no error occured </li>
   * <li> #ERROR_PARAM if invalid attributes are given </li>
   * </ul>
   *
   * <HR>
   */
  myerror_t newKey(key_type_t type, key_use_t use, date_t validity, size_t length, uint32_t *id);
  
  /* \brief Disconnect the current user from the API
   * <HR>
   * \return 
   * <ul>
   * <li> #NO_ERROR if no error occured 
   * <li> #ERROR_SEQ if nobody was connected
   * </ul>
   * <HR>
   */
  myerror_t disconnect();

  /* \brief Connect a user to the API
   * <HR>
   * \param login is a string containing the login of the user to connect
   * \param password contains the corresponding password
   * \return 
   * <ul>
   * <li> #NO_ERROR if no error occured 
   * <li> #ERROR_PARAM if user not in the base
   * <li> #ERROR_SEQ if a user is already connected
   * </ul>
   * <HR>
   */
  myerror_t connectUser(const std::string &login, const std::string &password);
  
  /* \brief Wrap a key using AES
   * <HR>
   * \param idWrapKey is the id of the key that will be used for wrapping
   * \param idExportedKey id the id of the key to export
   * \param output is a byte array that will contain the wrapped key
   * \param length is a pointer to the length of the byte array. Eventually, it will be set to the size of the wrapped key.
   * \return 
   * <ul>
   * <li> #NO_ERROR if no error occured 
   * <li> #ERROR_PARAM if one key is unknown or if keys are of a wrong type/use or if length is too small
   * <li> #ERROR_SEC if unauthorized users is trying to use a key
   * </ul>
   * <HR>
   */
  myerror_t wrap(uint32_t idWrapKey, uint32_t idExportedKey, uint8_t *output, size_t *length);
  
  /* \brief Unwrap a key using AES
   * <HR>
   * \param idWrapKey is the id of the key that will be used for wrapping
   * \param importedKey is a byte array that will contain the wrapped key
   * \param length is a pointer to the length of the byte array.
   * \param type should be one of the key_type_t values
   * \param use should be one of the key_use_t values
   * \param validity could be anything
   * \param id will be set to 0 in case of error or will contain the newly created key id (could be NULL)
   * \return 
   * <ul>
   * <li> #NO_ERROR if no error occured 
   * <li> #ERROR_PARAM if one key is unknown or if keys are of a wrong type/use
   * <li> #ERROR_SEC if unauthorized users is trying to use a key
   * </ul>
   * <HR>
   */
  myerror_t unwrap(uint32_t idWrapKey, uint8_t *importedKey, size_t length, key_type_t type, key_use_t use, date_t validity, uint32_t *id);
  
  /* \brief Encrypt message using AES (ECB)
   * <HR>
   * \param idKey is the id of the key that will be used for encryption
   * \param buffer is a byte array that contains the plaintext and will eventually contain the corresponding ciphertext
   * \param length is the length of the provided byte array.
   * \return 
   * <ul>
   * <li> #NO_ERROR if no error occured 
   * <li> #ERROR_PARAM if the key is unknown or it is of a wrong type/use or if length is too large 
   * <li> #ERROR_SEC if unauthorized users is trying to use a key
   * </ul>
   * <HR>
   */
  myerror_t encrypt(uint32_t idKey, uint8_t *buffer, size_t length);
  
  /* \brief Decrypt message using AES (ECB)
   * <HR>
   * \param idKey is the id of the key that will be used for encryption
   * \param buffer is a byte array that contains the ciphertext and will eventually contain the corresponding plaintext
   * \param length is the length of the provided byte array.
   * \return 
   * <ul>
   * <li> #NO_ERROR if no error occured 
   * <li> #ERROR_PARAM if the key is unknown or it is of a wrong type/use
   * <li> #ERROR_SEC if unauthorized users is trying to use a key
   * </ul>
   * <HR>
   */
  myerror_t decrypt(uint32_t idKey, uint8_t *buffer, size_t length);
  
  /** \brief Init a file corresponding to a void API object
   *
   * Format a file to be a void API object.
   * <HR>
   */
  static void initFile(const char *fileUsers, const char *fileKeys, unsigned instanceId);
  
  private:
  /** \brief Id of the API instance (used to identify this instance and to ensure user and key files are corresponding to the same instance */
  uint32_t _instanceId;
	
  /** \brief Boolean variable set to true only once memory have been initialised by loading a file. */
  bool _isInit;
  
  /** \brief Identity of the currently connected user (0 if no user connected). */
  User * _connectedUser;
  
  /** \brief Variable containing the relative path and filename of the file from which the user memory got initialised and where it should be saved on exit. */
  std::string _fileUsers;
  
  /** \brief Variable containing the relative path and filename of the file from which the key memory got initialised and where it should be saved on exit. */
  std::string _fileKeys;
  
  /** \brief List of registered users */
  std::vector<User*> _users;
  /** \brief Memory containing all keys that may be manipulated by the API */
  std::vector<Key*> _keys; 

  /** \brief Add a key to the memory
   *
   * <HR>
   * \param key is a pointer to the Key class that should be added to the memory.
   * \return #NO_ERROR
   *
   * <HR>
   */
  myerror_t addKey(Key *key);

  /** \brief Add a user to the base
   *
   * Add the user to the list of API users.
   * <HR>
   * \param user is a pointer to the User class that should be registered.
   * \return 
   * <ul>
   * <li> #NO_ERROR if no error occured 
   * <li> #ERROR_PARAM if user is already in the base
   * </ul>
   *
   * <HR>
   */
  myerror_t addUser(User *user);
  
  /** \brief This function stores the data into the file API#filename.
   *
   * <HR>
   * \return 
   * <ul>
   * <li> #NO_ERROR if no error occured 
   * <li> #ERROR_IO if an error occured when writing in the file
   * </ul>
   *
   * <HR>
   */
  myerror_t saveAPI();

  /** \brief This function loads the protected data contained in the file API#filename. 
   *
   * <HR>
   * \return 
   * <ul>
   * <li> #NO_ERROR if no error occured 
   * <li> #ERROR_IO if an error occured when reading the file
   * <li> #ERROR_FORMAT if an error occured when parsing the data
   * </ul>
   *
   * <HR>
   */
  myerror_t loadAPI();
  
  /** \brief This function finds a key corresponding to a given id.
   *
   * <HR>
   * \param id is the id of the searched key
   * \param key is a pointer to a key* object that will be set to point to the found key
   * \return 
   * <ul>
   * <li> #NO_ERROR if no error occured 
   * <li> #ERROR_PARAM if no key is found
   * </ul>
   *
   * <HR>
   */
  myerror_t findKey(uint32_t id, Key **key);
  
  /** \brief This function decrypts the key using the local encryption key. IMPORTANT: erase buffer after use !
   *
   * <HR>
   * \param key is a pointer to the key to decrypt
   * \param output will contain the key value
   * \param length is a pointer to the length of the byte array that will be set to the final size of the wrapped key. 
   * \return 
   * <ul>
   * <li> #NO_ERROR if no error occured 
   * <li> #ERROR_PARAM if ouptut is too small
   * </ul>
   *
   * <HR>
   */
  myerror_t decryptKey(const Key *key, uint8_t *output, size_t *length);

  /** \brief This function encrypts the data using the local encryption key.
   *
   * <HR>
   * \param data is a pointer to the data to protect
   * \param length is the length of the data (in bytes).
   * \return 
   * <ul>
   * <li> #NO_ERROR if no error occured 
   * </ul>
   *
   * <HR>
   */
  myerror_t protectData(uint8_t *data, size_t length);
  
};

#endif
