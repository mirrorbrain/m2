#include "user.hpp"

#include "base64.hpp"
#include "json.hpp"
#include <cstring>

User::User(const char login[MAX_LOGIN_LEN], const char password[MAX_PASSW_LEN], uint32_t id, role_t role) : _id(id), _role(role)
{
  /* First set login */
  memset(_login,0,MAX_LOGIN_LEN+1);
  strcpy(_login,login);
  /* Then hash password */
  hashPassword(password,_password);
}

User::~User()
{
}

uint32_t User::getId() const
{
  return _id;
}

const char* User::getLogin() const
{
  return _login;
}

const role_t User::getRole() const
{
  return _role;
}

std::ostream& operator<<(std::ostream& os, const User& obj)
{
  char password[64];
  BASE64 converter;
  size_t length;
  converter.encode(obj._password,obj._hashPasswSize,password,&length);
  
  os << "{ ";
  os << "\"id\": " << obj._id << " , ";
  os << "\"login\": \"" << obj._login << "\" , ";
  os << "\"password\": \"" << password << "\" , ";
  os << "\"privilege\": " << obj._role;
  os << " }";
  
    // write obj to stream
    return os;
}

bool User::hasSameLogin(const std::string &login) const
{
  if ( 0 == strncmp(_login,login.c_str(),MAX_LOGIN_LEN) )
    return true;
  return false;
}

bool User::isCorrectPassword(const std::string &password) const
{
  uint8_t temp[_hashPasswSize];
  hashPassword(password.c_str(),temp);
  if ( 0 == memcmp(temp,_password,_hashPasswSize) )
    return true;
  return false;
}

void User::hashPassword(const char password[MAX_PASSW_LEN], uint8_t digest[_hashPasswSize])
{
  SHA256 hashFunc;
  hashFunc.init();
  hashFunc.update((uint8_t *)password,strnlen(password,MAX_PASSW_LEN));
  hashFunc.final(digest);
}

myerror_t User::set(const std::string &field, const std::string &value){
  if ( "id" == field )
    _id = stoul(value);
  if ( "login" == field )
    sprintf(_login,"%s",value.c_str());
  if ( "password" == field )
    {
      BASE64 converter;
      size_t length;
      if ( value.size() <= ((_hashPasswSize*8)/6)+2 )
	{
	  converter.decode(value.c_str(),value.size(),_password,&length);
	}
      else
      {
	std::cerr << value.size() << std::endl;
	return ERROR_FORMAT;
      }
     
    }
  if ( "privilege" == field ) 
    _role = role_t(stoi(value));
  return  NO_ERROR;
}

myerror_t User::serialize(uint8_t *tab, size_t len)
{
  if ( len < _serializedSize )
    return ERROR_PARAM;

  memset(tab,0,_serializedSize);
  
  memcpy(tab,(uint8_t*)&_id,4);
  tab += 4;

  strncpy((char*)tab,_login,MAX_LOGIN_LEN);
  tab += MAX_LOGIN_LEN;

  memcpy(tab,_password,_hashPasswSize);
  tab += _hashPasswSize;

  *tab = (uint8_t) _role;
  
  return  NO_ERROR;
}

std::istream& operator>>(std::istream& is, User& obj)
{
  JSONParser::createObject<User>(is,obj,1000);
  return is;
}
