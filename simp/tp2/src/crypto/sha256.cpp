#include "sha256.hpp"

myerror_t SHA256::test()
{
  myerror_t err = ERROR_UNEXPECTED;
  size_t byte_idx;
  uint8_t result[32];

  SHA256 temp;

  for ( unsigned test_idx = 0 ; test_idx < 4 ; test_idx++ )
    {
      temp.init();
      temp.update((const uint8_t*) TV_messages[test_idx],TV_lengths[test_idx]);
      temp.final(result);
      for ( byte_idx = 0 ; (byte_idx < hashSize) && (result[byte_idx] == TV_hashes[test_idx][byte_idx]) ; byte_idx ++ )
	;
      if ( byte_idx != hashSize )
	goto end;
    }
  
  if ( byte_idx == hashSize )
    err = NO_ERROR;
  
 end:
  return err;
}

void SHA256::init()
{
	dataBitLength = 0;
	state[0] = 0x6a09e667;
	state[1] = 0xbb67ae85;
	state[2] = 0x3c6ef372;
	state[3] = 0xa54ff53a;
	state[4] = 0x510e527f;
	state[5] = 0x9b05688c;
	state[6] = 0x1f83d9ab;
	state[7] = 0x5be0cd19;
	bufferLength = 0;
	for ( size_t i = 0 ; i < 64 ; i ++ )
	  dataBuffer[i] = 0;
}

void SHA256::update(const uint8_t *data, size_t length)
{
  for ( uint32_t i = 0 ; i < length ; ++i )
    {
      dataBuffer[bufferLength] = data[i];
      bufferLength++;
      if (bufferLength == 64)
	{
	  compress(dataBuffer);
	  dataBitLength += 512;
	  bufferLength = 0;
	}
    }
}

void SHA256::final(uint8_t hash[32])
{
  size_t i;
  i = bufferLength;
  // Pad whatever data is left in the buffer.
  if (bufferLength < 56)
    {
      dataBuffer[i++] = 0x80;
      while (i < 56)
	dataBuffer[i++] = 0x00;
    }
  else
    {
      dataBuffer[i++] = 0x80;
      while (i < 64)
	dataBuffer[i++] = 0x00;
      compress(dataBuffer);
      memset(dataBuffer, 0, 56);
    }

  // Append to the padding the total message's length in bits and transform.
  dataBitLength += bufferLength * 8;
  dataBuffer[63] = dataBitLength;
  dataBuffer[62] = dataBitLength >> 8;
  dataBuffer[61] = dataBitLength >> 16;
  dataBuffer[60] = dataBitLength >> 24;
  dataBuffer[59] = dataBitLength >> 32;
  dataBuffer[58] = dataBitLength >> 40;
  dataBuffer[57] = dataBitLength >> 48;
  dataBuffer[56] = dataBitLength >> 56;
  compress(dataBuffer);

  // Since this implementation uses little endian byte ordering and SHA uses big endian,
  // reverse all the bytes when copying the final state to the output hash.
  for ( i = 0 ; i < 4 ; ++i)
    {
      hash[i]      = (state[0] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 4]  = (state[1] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 8]  = (state[2] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 12] = (state[3] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 16] = (state[4] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 20] = (state[5] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 24] = (state[6] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 28] = (state[7] >> (24 - i * 8)) & 0x000000ff;
    }
}

#define SHA256_ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))
#define SHA256_CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define SHA256_MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

static inline uint32_t EP0(uint32_t x)
{
  return SHA256_ROTRIGHT(x,2) ^ SHA256_ROTRIGHT(x,13) ^ SHA256_ROTRIGHT(x,22);
}

static inline uint32_t EP1(uint32_t x)
{
  return SHA256_ROTRIGHT(x,6) ^ SHA256_ROTRIGHT(x,11) ^ SHA256_ROTRIGHT(x,25);
}
  
static inline uint32_t SIG0(uint32_t x)
{
  return SHA256_ROTRIGHT(x,7) ^ SHA256_ROTRIGHT(x,18) ^ (x >> 3);
}

static inline uint32_t SIG1(uint32_t x)
{
  return SHA256_ROTRIGHT(x,17) ^ SHA256_ROTRIGHT(x,19) ^ (x >> 10);
}

void SHA256::compress(const uint8_t data[64])
{
  size_t i, j;
  uint32_t a, b, c, d, e, f, g, h, t1, t2, m[64];

  for ( i = 0, j = 0 ; i < 16 ; ++i, j += 4)
    {
      m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
    }
  for ( ; i < 64; ++i)
    {
      m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];
    }

  a = state[0];
  b = state[1];
  c = state[2];
  d = state[3];
  e = state[4];
  f = state[5];
  g = state[6];
  h = state[7];

  for ( i = 0 ; i < 64 ; ++i )
    {
      t1 = h + EP1(e) + SHA256_CH(e,f,g) + k[i] + m[i];
      t2 = EP0(a) + SHA256_MAJ(a,b,c);
      h = g;
      g = f;
      f = e;
      e = d + t1;
      d = c;
      c = b;
      b = a;
      a = t1 + t2;
    }

  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  state[4] += e;
  state[5] += f;
  state[6] += g;
  state[7] += h;
}

/* DATA */

const uint32_t SHA256::k[64] = {
  0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
  0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
  0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
  0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
  0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
  0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
  0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
  0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

const size_t SHA256::TV_lengths[4] = {
  0, 3, 56, 112
};

const char * SHA256::TV_messages[4] = {
  "",
  "abc",
  "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
  "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"
};

const uint8_t SHA256::TV_hashes[4][32] = {
  { 0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, 0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24, 0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c, 0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55 } ,
  { 0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea, 0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23, 0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c, 0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad } ,
  { 0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8, 0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39, 0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67, 0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1 },
  { 0xcf, 0x5b, 0X16, 0xa7, 0x78, 0xaf, 0X83, 0x80, 0x03, 0x6c, 0Xe5, 0x9e, 0x7b, 0x04, 0X92, 0x37, 0x0b, 0x24, 0X9b, 0x11, 0xe8, 0xf0, 0X7a, 0x51, 0xaf, 0xac, 0X45, 0x03, 0x7a, 0xfe, 0Xe9, 0xd1 }
};
