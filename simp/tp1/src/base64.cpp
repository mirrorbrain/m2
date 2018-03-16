#include "base64.hpp"

#include <cstdlib>

myerror_t BASE64::test()
{
  myerror_t err = ERROR_UNEXPECTED;
  unsigned i, j;
  
  uint8_t input[60], result[60];
  char output[80];
  size_t size_64, size_str, size_res;
  const size_t nb_tests = 50;

  BASE64 temp;
  
  for ( i = 0 ; i < nb_tests ; i++ )
    {
      size_str = rand() % 60;
      for ( j = 0 ; j < size_str ; j++ )
	input[j] = rand();
      
      temp.encode(input,size_str,output,&size_64);
      output[size_64] = 0;
      
      temp.decode(output,size_64,result,&size_res);
      result[size_res] = 0;

      if ( size_str != size_res )
      	goto end;
      for ( j = 0 ; j < size_str ; j++ )
	if ( input[j] != result[j] )
	  goto end;
    }
  err = NO_ERROR;
  
 end:
  return err;
}

void BASE64::initEncoding()
{
  encodeState.step = step_A;
  encodeState.result = 0;
}

void BASE64::initDecoding()
{
  decodeState.step = step_A;
  decodeState.plainchar = 0;
}

void BASE64::encode(const uint8_t *data_in, size_t length_in, char* data_out, size_t *length_out)
{
  const uint8_t* plainchar = data_in;
  const uint8_t* const plaintextend = data_in + length_in;
  char* codechar = data_out;

  uint8_t fragment, result;

  initEncoding();
  
  result = encodeState.result;
	
  switch (encodeState.step)
    {
      while (1)
	{
	case step_A:
	  if (plainchar == plaintextend)
	    {
	      encodeState.result = result;
	      encodeState.step = step_A;
	      goto finalize_encoding;
	    }
	  fragment = *plainchar++;
	  result = (fragment & 0x0fc) >> 2;
	  *codechar++ = encodeValue(result);
	  result = (fragment & 0x003) << 4;
	case step_B:
	  if (plainchar == plaintextend)
	    {
	      encodeState.result = result;
	      encodeState.step = step_B;
	      goto finalize_encoding;
	    }
	  fragment = *plainchar++;
	  result |= (fragment & 0x0f0) >> 4;
	  *codechar++ = encodeValue(result);
	  result = (fragment & 0x00f) << 2;
	case step_C:
	  if (plainchar == plaintextend)
	    {
	      encodeState.result = result;
	      encodeState.step = step_C;
	      goto finalize_encoding;
	    }
	  fragment = *plainchar++;
	  result |= (fragment & 0x0c0) >> 6;
	  *codechar++ = encodeValue(result);
	  result  = (fragment & 0x03f) >> 0;
	  *codechar++ = encodeValue(result);
	}
    }

 finalize_encoding:
  switch (encodeState.step)
    {
    case step_B:
      *codechar++ = encodeValue(encodeState.result);
      *codechar++ = '=';
      *codechar++ = '=';
      break;
    case step_C:
      *codechar++ = encodeValue(encodeState.result);
      *codechar++ = '=';
      break;
    case step_A:
      break;
    }

  (*length_out) = codechar - data_out;
  *codechar = 0;
  return ;
}
#include <iostream>
void BASE64::decode(const char *data_in, size_t length_in, uint8_t* data_out, size_t *length_out)
{  
  const char* codechar = data_in;
  uint8_t* plainchar = data_out;
  int8_t fragment;

   initDecoding();
   
  *plainchar = decodeState.plainchar;
	
  switch (decodeState.step)
    {
      while (1)
	{
	case step_A:
	  do {
	    if (codechar == data_in+length_in)
	      {
		decodeState.step = step_A;
		decodeState.plainchar = *plainchar;
		(*length_out) =  plainchar - data_out;
		return ;
	      }
	    fragment = decodeValue(*codechar++);
	  } while (fragment < 0);
	  *plainchar    = (((uint8_t)fragment) & 0x03f) << 2;
	case step_B:
	  do {
	    if (codechar == data_in+length_in)
	      {
		decodeState.step = step_B;
		decodeState.plainchar = *plainchar;
		(*length_out) =  plainchar - data_out;
		return ;
	      }
	    fragment = decodeValue(*codechar++);
	  } while (fragment < 0);
	  *plainchar++ |= (((uint8_t)fragment) & 0x030) >> 4;
	  *plainchar    = (((uint8_t)fragment) & 0x00f) << 4;
	case step_C:
	  do {
	    if (codechar == data_in+length_in)
	      {
		decodeState.step = step_C;
		decodeState.plainchar = *plainchar;
		(*length_out) =  plainchar - data_out;
		return ;
	      }
	    fragment = decodeValue(*codechar++);
	  } while (fragment < 0);
	  *plainchar++ |= (((uint8_t)fragment) & 0x03c) >> 2;
	  *plainchar    = (((uint8_t)fragment) & 0x003) << 6;
	case step_D:
	  do {
	    if (codechar == data_in+length_in)
	      {
		decodeState.step = step_D;
		decodeState.plainchar = *plainchar;
		(*length_out) =  plainchar - data_out;
		return ;
	      }
	    fragment = decodeValue(*codechar++);
	  } while (fragment < 0);
	  *plainchar++   |= (((uint8_t)fragment) & 0x03f);
	}
    }
  // control should not reach here
  (*length_out) =  plainchar - data_out;
  return ;
}

char BASE64::encodeValue(uint8_t value_in)
{
  static const char* encoding = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  if (value_in > 63)
    return '=';
  return encoding[value_in];
}

int8_t BASE64::decodeValue(char value_in)
{
  static const int8_t decoding[] = {62,-1,-1,-1,63,52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-2,-1,-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51};
  static const char decoding_size = sizeof(decoding);
  value_in -= 43;
  if (value_in < 0 || value_in > decoding_size)
    return -1;
  return decoding[(int)value_in];
}

