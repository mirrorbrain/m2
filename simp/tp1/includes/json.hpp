/** \file json.hpp
 * JSON parser
 *
 * JSONParser class definition.
 */

#ifndef JSON_HPP_
#define JSON_HPP_

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "error.hpp"

class JSONEntry
{
public:
  JSONEntry(std::string field, std::string value) : _field(field) , _value(value) {}
  std::string getField() const { return _field; }
  std::string getValue() const { return _value; }
private:
  const std::string _field;
  const std::string _value;
};

class JSONParser
{
public:
  typedef std::vector<JSONEntry> json_object_t;
  
  JSONParser(size_t maxObjectSize) : _maxObjectSize(maxObjectSize) {}
  json_object_t getObject(std::istream& is) const;
  static std::pair<std::string,std::string> tokenize(const std::string &str, size_t start = 0, size_t end = 0);
  template<class T>
  //static std::istream&
  static myerror_t createObject(std::istream& is, T& obj, size_t max);
private:
  const size_t _maxObjectSize;
  static void stripQuotes(std::string &str);
};

// TODO : handle errors from obj.set()
template<class T>
//std::istream&
myerror_t JSONParser::createObject(std::istream& is, T& obj, size_t max)
{
  myerror_t ret = NO_ERROR;
  JSONParser parser(max);

  JSONParser::json_object_t temp = parser.getObject(is);

  if ( temp.empty() )
    ret = ERROR_FORMAT;
  else
    {
      for ( auto it = temp.begin() ; it != temp.end() ; it++ )
	{
	  obj.set(it->getField(),it->getValue());
	}
    }
  
  return ret;
}

#endif
