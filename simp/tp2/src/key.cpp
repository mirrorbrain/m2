#include "key.hpp"

#include "base64.hpp"
#include "json.hpp"
#include <cstring>

uint32_t Key::getId() const
{
  return _id;
}

uint32_t Key::getOwner() const
{
  return _owner;
}

key_type_t Key::getType() const
{
  return _type;
}

key_use_t Key::getUse() const
{
  return _use;
}

date_t Key::getValidity() const
{
  return _validity;
}

std::vector<uint8_t> Key::getValue() const
{
  return _value;
}

std::ostream& operator<<(std::ostream& os, const Key& obj)
{
os << "{ ";
os << "\"id\": " << obj.getId() << " , ";
os << "\"owner\": " << obj.getOwner() << " , ";
os << "\"type\": " << obj.getType() << " , ";
os << "\"use\": " << obj.getUse() << " , ";
os << "\"validity\": " << obj.getValidity() << " , ";
if ( (KEY_SYM != obj.getType()) && (0 != obj._associated) )
    os << "\"associated\": " << obj._associated<< " , ";
  os << "\"length\": " << obj._length<< " , ";
  os << "\"value\": \"";
  BASE64 converter;
  char buffer[200];
  size_t len;
converter.encode(obj.getValue().data(),obj._length,buffer,&len);
  os << buffer;
  os << "\"";
  os << " }";
  
    // write obj to stream
    return os;
}

myerror_t Key::set(const std::string &field, const std::string &value){
  if ( "id" == field )
    _id = stoul(value);
  if ( "owner" == field )
    _owner = stoul(value);
  if ( "type" == field )
    _type = key_type_t(stoi(value));
  if ( "use" == field )
    _use = key_use_t(stoi(value));
  if ( "validity" == field )
    _validity = stoul(value);
  if ( "associated" == field )
    _associated = stoul(value);
  if ( "length" == field )
    _length = stoul(value);
  if ( ("value" == field) && (0 != _length) )
    {
      BASE64 decoder;
      size_t key_len;
      _value = std::vector<uint8_t>(_length,0);
      decoder.decode(value.c_str(),value.size(),_value.data(),&key_len);
    }
}

myerror_t Key::serialize(uint8_t *tab, size_t len)
{
  if ( len < _serializedSize + _length )
    return ERROR_PARAM;

  memset(tab,0,_serializedSize+_length);
  
  memcpy(tab,(uint8_t*)&_id,4);
  tab += 4;
  
  memcpy(tab,(uint8_t*)&_owner,4);
  tab += 4;
  
  memcpy(tab,(uint8_t*)&_type,1);
  tab += 1;
  
  memcpy(tab,(uint8_t*)&_use,1);
  tab += 1;
  
  memcpy(tab,(uint8_t*)&_validity,4);
  tab += 4;
  
  memcpy(tab,(uint8_t*)&_associated,4);
  tab += 4;
  
  memcpy(tab,(uint8_t*)&_length,sizeof(size_t));
  tab += sizeof(size_t);

  memcpy(tab,_value.data(),_length);
  
  return  NO_ERROR;
}

std::istream& operator>>(std::istream& is, Key& obj)
{
  JSONParser::createObject<Key>(is,obj,1000);
  return is;
}
