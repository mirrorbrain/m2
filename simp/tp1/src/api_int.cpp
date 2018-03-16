#include "api.hpp"

#include <string>
#include <fstream>

#include "json.hpp"
#include "aes.hpp"
#include "hmac.hpp"
#include "base64.hpp"

static const uint8_t localIntegrityKey[32] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F};

static const uint8_t localConfidentialityKey[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};

myerror_t API::saveAPI()
{
  /** <ol> */
  myerror_t ret = NO_ERROR;
  std::filebuf fb;
  std::ostream os(&fb);
  HMAC tagComputer;
  uint8_t computedTag[tagComputer.tagLength];
  BASE64 encoder;
  char tagString[64];
  size_t temp;
  uint8_t bufferUser[User::_serializedSize];
  uint8_t bufferKey[Key::_serializedSize+MAX_KEY_LEN];
  
  /** <li> Opening the user file */
  if ( fb.open(_fileUsers,std::ios::out) )
    ;
  else
    ret = ERROR_IO;

  if ( ret == NO_ERROR )
    {
      /** <li> Init tag computation </li> */
      tagComputer.init(localIntegrityKey,32);
      
      /** <li> Processing the header </li> <ul> */
      os << "{" << std::endl;
      /** <li> Printing </li> */
      os << "\t\"instance\": " << _instanceId << "," << std::endl;
      /** <li> Adding to tag computation </li> */
      tagComputer.update((uint8_t*)&_instanceId,4);
      /** </ul> */
    }
  
  /** <li> Processing the users </li> <ul> */
  os << "\t\"users\": [" << std::endl;
  for ( auto it = _users.begin() ; (it != _users.end()) && (ret == NO_ERROR) ; )
    {
      /** <li> Printing </li> */
      os << "\t\t" << (**it);
      /** <li> Adding to tag computation </li> */
      (*it)->serialize(bufferUser,User::_serializedSize);
      tagComputer.update(bufferUser,User::_serializedSize);
      /** <li> Next key </li> */
      it ++;
      if ( it != _users.end() )
	os << ",";
      os << std::endl;
    }
  os << "\t]," << std::endl;

  /** <li> Dumping the footer */
  if ( ret == NO_ERROR )
    {
      tagComputer.final(computedTag);
      encoder.encode(computedTag,tagComputer.tagLength,tagString,&temp);
      os << "\t\"tag\": \"" << tagString << "\"" << std::endl;
      os << "}" << std::endl;
    }

  /** <li> Close the file */
  fb.close();
  
  /** <li> Opening the key file */
  if ( fb.open(_fileKeys,std::ios::out) )
    ;
  else
    ret = ERROR_IO;
  
  /** <li> Init tag computation </li> */
  tagComputer.init(localIntegrityKey,32);
      
      /** <li> Processing the header </li> <ul> */
  if ( ret == NO_ERROR )
    {
      os << "{" << std::endl;
      /** <li> Printing </li> */
      os << "\t\"instance\": " << _instanceId << "," << std::endl;
      /** <li> Adding to tag computation </li> */
      tagComputer.update((uint8_t*)&_instanceId,4);
      /** </ul> */
    }
  
  /** <li> Dumping keys */
  os << "\t\"keys\": [" << std::endl;
  for ( auto it = _keys.begin() ; (it != _keys.end()) && (ret == NO_ERROR) ; )
    {
      os << "\t\t" << (**it);
      /** <li> Adding to tag computation </li> */
      (*it)->serialize(bufferKey,Key::_serializedSize+MAX_KEY_LEN);
      tagComputer.update(bufferKey,Key::_serializedSize+(*it)->_length);
      /** <li> Next key </li> */
      it ++;
      if ( it != _keys.end() )
	os << ",";
      os << std::endl;
    }
  os << "\t]" << std::endl;
  
  /** <li> Dumping the footer */
  if ( ret == NO_ERROR )
    {
      tagComputer.final(computedTag);
      encoder.encode(computedTag,tagComputer.tagLength,tagString,&temp);
      os << "\t\"tag\": \"" << tagString << "\"" << std::endl;
      os << "}" << std::endl;
    }

  /** <li> Close the file */
  fb.close();
  /** </ol> */
  return NO_ERROR;
}

myerror_t API::loadAPI()
{
  /** <ol> */
  myerror_t ret = NO_ERROR;
  char c;
  char buffer[1000];
  std::filebuf fb;
  std::istream is(&fb);
  HMAC tagComputer;
  uint8_t computedTag[tagComputer.tagLength];
  uint8_t providedTag[tagComputer.tagLength+1]; // +1 comes from a bug in BASE 64 decoder
  BASE64 encoder;
  uint8_t bufferUser[User::_serializedSize];
  uint8_t bufferKey[Key::_serializedSize+MAX_KEY_LEN];

  if ( _isInit )
    reset();
  
  /** <li> Opening the user file */
  if ( fb.open(_fileUsers,std::ios::in) )
    ;
  else
    ret = ERROR_IO;
  
  /** <li> Reading header */
  if ( ret == NO_ERROR )
    {
      /** <li> Init tag computation </li> */
      tagComputer.init(localIntegrityKey,32);
      
      /* Find the begining of API object */
      is.getline(buffer,1000,'{');
      is.getline(buffer,1000,':');
      if ( std::string(buffer).find("instance") == std::string::npos )
	{
	  std::cerr << "[error] Missing instance Id in JSON file.\n";
	  ret = ERROR_FORMAT;
	}
      else
	{
	  is.getline(buffer,1000,',');
	  _instanceId = atol(buffer);
	  tagComputer.update((uint8_t*)&_instanceId,4);
	}
    }
  
  /** <li> Reading users */
  is.getline(buffer,1000,'[');
  if ( std::string(buffer).find("users") == std::string::npos )
    {
      std::cerr << "[error] Expecting users in JSON file.\n";
      ret = ERROR_FORMAT;
    }

  c = ',';
  while ( (ret == NO_ERROR) && (c == ',') )
    {
      User *newUser = new User();
      /* Gets next user */
      
      ret = JSONParser::createObject<User>(is, *newUser, 1000);
      if ( ret == NO_ERROR )
	{
	  /* Add user to the list */
	  addUser(newUser);
	  /** <li> Adding to tag computation </li> */
	  newUser->serialize(bufferUser,User::_serializedSize);
	  tagComputer.update(bufferUser,User::_serializedSize);  
	  /* If no coma found : end of array */
	  is.get(c);
	}
    }

  /** <li> Compute the tag corresponding to the file </li> */
  tagComputer.final(computedTag);

  /* get the tag provided by the file and check it */
  is.getline(buffer,1000,':');
  if ( std::string(buffer).find("tag") == std::string::npos )
    {
      std::cerr << "[error] Expecting an integrity tag for the file.\n";
      ret = ERROR_FORMAT;
    }
  else
    {
      is.getline(buffer,1000,'"');
      is.getline(buffer,1000,'"');
      /* here buffer should contain the content of the quotes */
      size_t size = strnlen(buffer,1000);
     encoder.decode(buffer,size,providedTag,&size);

     if ( 0 != strncmp((char*) computedTag,(char*) providedTag,tagComputer.tagLength) )
       {
	 std::cerr << "[error] Integrity check is a failure.\n";
	 reset();
	 ret = ERROR_SEC;
       }
    }

  fb.close();
    
  /** <li> Opening the key file */
  if ( fb.open(_fileKeys,std::ios::in) )
    ;
  else
    ret = ERROR_IO;
  
  /** <li> Reading header */
  if ( ret == NO_ERROR )
    {
      /** <li> Init tag computation </li> */
      tagComputer.init(localIntegrityKey,32);
      
      /* Find the begining of API object */
      is.getline(buffer,1000,'{');
      is.getline(buffer,1000,':');
      if ( std::string(buffer).find("instance") == std::string::npos )
	{
	  std::cerr << "[error] Missing instance Id in JSON file.\n";
	  ret = ERROR_FORMAT;
	}
      else
	{
	  is.getline(buffer,1000,',');
	  if ( atol(buffer) != _instanceId )
	    {
	      std::cerr << "[error] Instance not matching in both JSON files.\n";
	      ret = ERROR_SEC;
	    }
	  tagComputer.update((uint8_t*)&_instanceId,4);
	}
    }
  
  /** <li> Reading keys */
  if ( NO_ERROR == ret )
    {
      is.getline(buffer,1000,'[');
      if ( std::string(buffer).find("keys") == std::string::npos )
	{
	  std::cerr << "[error] Expecting keys in JSON file.\n";
	  ret = ERROR_FORMAT;
	}
    }
  
  c = ',';
  while ( (ret == NO_ERROR) && (c == ',') )
    {
      Key *newKey = new Key();
      /* Gets next key */
      ret = JSONParser::createObject<Key>(is, *newKey, 1000);
      if ( ret == NO_ERROR )
	{
	  /* Add key to the list */
	  addKey(newKey);
	  /** <li> Adding to tag computation </li> */
	  newKey->serialize(bufferKey,Key::_serializedSize+MAX_KEY_LEN);
	  tagComputer.update(bufferKey,Key::_serializedSize+newKey->_length);
	  /* If no coma found : end of array */
	  is >> c;
	}
      else
	{
	  /* Handling the case of empty key */
	  if ( ret == ERROR_FORMAT )
	    {
	      ret = NO_ERROR;
	      is >> c;
	    }
	}
    }


  /** <li> Compute the tag corresponding to the file </li> */
  tagComputer.final(computedTag);

  /* get the tag provided by the file and check it */
  is.getline(buffer,1000,':');
  if ( std::string(buffer).find("tag") == std::string::npos )
    {
      std::cerr << "[error] Expecting an integrity tag for the file.\n";
      ret = ERROR_FORMAT;
    }
  else
    {
      is.getline(buffer,1000,'"');
      is.getline(buffer,1000,'"');
      /* here buffer should contain the content of the quotes */
      size_t size = strnlen(buffer,1000);
     encoder.decode(buffer,size,providedTag,&size);

     if ( 0 != strncmp((char*) computedTag,(char*) providedTag,tagComputer.tagLength) )
       {
	 std::cerr << "[error] Integrity check is a failure.\n";
	 reset();
	 ret = ERROR_SEC;
       }
    }
  
  /** <li> Closing the file */
  fb.close();
  
  /** </ol> */
  return ret;
}

myerror_t API::addKey(Key *key)
{
  _keys.push_back(key);
  return NO_ERROR;
}

myerror_t API::addUser(User *user)
{
  myerror_t ret = NO_ERROR;
  unsigned i;
  for ( i = 0 ; (i < _users.size()) && (ret == NO_ERROR) ; i++ )
    {
      if ( user->hasSameLogin(_users[i]->getLogin()) )
	{
	  ret = ERROR_PARAM;
	}
    }
  if ( ret == NO_ERROR )
    {
      _users.push_back(std::move(user));
    }
  return ret;
}

myerror_t API::findKey(uint32_t id, Key **key)
{
  /** <ol> */
  myerror_t ret = ERROR_PARAM;

  *key = NULL;
  for ( auto it = _keys.begin() ; (it != _keys.end()) && (ret != NO_ERROR) ; it ++ )
    {
      if ( (*it)->getId() == id )
	{
	  *key = *it;
	  ret = NO_ERROR;
	}
    }
  
  /** </ol> */
  return ret;
}

myerror_t API::decryptKey(const Key *key, uint8_t *output, size_t *length)
{
  myerror_t ret = ERROR_UNEXPECTED;
  uint8_t buffer[16];
  int remaining = key->_length;
  size_t index = 0;

  if ( *length < key->_length )
    ret = ERROR_PARAM;
  else
    {
      while ( remaining > 0 )
	{
	  for ( unsigned i = 0 ; i < 16 ; i ++ )
	    {
	      if ( index < key->_length )
		buffer[i] = key->getValue()[index];
	      else
		buffer[i] = 0;
	      index ++;
	    }
	  AES::decrypt(localConfidentialityKey,buffer,buffer);
	  for ( unsigned i = 0 ; i < 16 ; i ++ )
	    {
	      output[index-16+i] = buffer[i];
	      remaining --;
	      if ( remaining == 0 )
		break;
	    }	  
	}
    }
 
  ret = NO_ERROR;
  
  return ret;
}

myerror_t API::protectData(uint8_t *data, size_t length)
{
  myerror_t ret = ERROR_UNEXPECTED;
  uint8_t buffer[16];
  int remaining = length;
  size_t index = 0;

  while ( remaining > 0 )
    {
      for ( unsigned i = 0 ; i < 16 ; i ++ )
	{
	  if ( index < length )
	    buffer[i] = data[index];
	  else
	    buffer[i] = 0;
	  index ++;
	}
      AES::encrypt(localConfidentialityKey,buffer,buffer);
      for ( unsigned i = 0 ; i < 16 ; i ++ )
	{
	  data[index-16+i] = buffer[i];
	  remaining --;
	  if ( remaining == 0 )
	    break;
	}	  
    }

  ret = NO_ERROR;
  
  return ret;
}
