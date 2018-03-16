#include "json.hpp"

void JSONParser::stripQuotes(std::string &str)
{
  size_t first_pos, last_pos;	  
  first_pos = str.find_first_of('"');
  if ( first_pos != std::string::npos )
    {
      last_pos = str.find_last_of('"');
      str = str.substr(first_pos+1,last_pos-first_pos-1);
    }
}

JSONParser::json_object_t JSONParser::getObject(std::istream& is) const
{
  char read;
  std::string obj;
  json_object_t res;
  size_t len, pos, next_pos;
  
  // look for the first brace
  do
    {
      is.get(read);
      if ( (read != '{') && (read != ' ') && (read != '\n') && (read != '\t') )
	return res;
    }
  while ( read != '{' );

  len = 0;
  do
    {
      is.get(read);
      if ( read != '}' )
	{
	  obj += read;
	  if ( len >= _maxObjectSize )
	    {
	      res.clear();
	      return res;
	    }
	  len ++;
	}
    }
  while ( read != '}' );

  // Now tokenize object
  pos = 0;
  do
    {
      next_pos = obj.find(',',pos);

      auto fv = tokenize(obj,pos,next_pos);
	
      res.emplace_back(fv.first,fv.second);
      
      pos = next_pos+1;
    }
  while ( next_pos != std::string::npos );
  
  return res;
}

std::pair<std::string,std::string> JSONParser::tokenize(const std::string &str, size_t start, size_t end)
{
  std::pair<std::string,std::string> res;
  size_t  colon_pos;
  if ( end == 0 )
    end = str.size();
  
  colon_pos = str.find(':',start);
  
  res.first = str.substr(start,colon_pos-start);
  res.second = str.substr(colon_pos+1,end-colon_pos-1);

  stripQuotes(res.first);
  stripQuotes(res.second);
  
  return res;
}

