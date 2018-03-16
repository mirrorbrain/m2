#include <cstdio>
#include <cstring>
#include <map>
#include <string>
#include <vector>

typedef std::vector<unsigned char> curve_t;

struct cmp_str
{
  bool operator()(char const *a, char const *b)
  { return std::strcmp(a,b) < 0; }
};

typedef struct {
  curve_t curve;
  std::map<const char*,std::vector<unsigned char>,cmp_str> data;
} trace_t;

class BinaryReader
{
public:
  BinaryReader(std::string filename)
  {
    file_handler = fopen(filename.c_str(),"rb");
    /* Put traces in memory */
    if ( NULL != file_handler )
      {
	readHeader();
	readCurves();
      }
  }

  ~BinaryReader()
  {
    fclose(this->file_handler);
  }

  unsigned int getNbCurves() const { return nb_curves; }
  
  trace_t getCurve(unsigned index) const
  {
    trace_t res;
    if ( index < nb_curves )
      res = traces[index];
    return res;
  }
private:
  FILE *file_handler;
  unsigned int nb_curves, header_size;
  unsigned char nb_fields;
  std::vector<std::pair<std::string,unsigned int> > fields;
  std::vector<trace_t> traces;

  void readCurves()
  {
    for ( unsigned i = 0 ; i < nb_curves ; i++ )
      {
	trace_t temp;
	// read metadata
	for ( auto it = fields.begin() ; it != fields.end() ; it++ )
	  {
	    std::vector<unsigned char> value(it->second);
	    fread(&value[0],sizeof(unsigned char),it->second,file_handler);
	    temp.data.insert(std::make_pair(it->first.c_str(),value));
	  }
	// read the curve
	unsigned int trace_length;
	fread(&trace_length,sizeof(unsigned int),1,file_handler);
	temp.curve = std::vector<unsigned char>(trace_length);
	fread(&temp.curve[0],sizeof(unsigned char),trace_length,file_handler);
	traces.push_back(temp);
      }
  }
  
  void readHeader()
  {
    unsigned char key_size;
    unsigned int field_size;
    fread(&nb_curves,sizeof(unsigned int),1,file_handler);
    fread(&nb_fields,sizeof(unsigned char),1,file_handler);
    for ( unsigned i = 0 ; i < nb_fields ; i++ )
      {
	fread(&key_size,sizeof(unsigned char),1,file_handler);
	std::vector<char> key_buf(key_size);
	fread(&key_buf[0],sizeof(char),key_size,file_handler);
	fread(&field_size,sizeof(unsigned int),1,file_handler);
	std::string key(&key_buf[0]);
	fields.push_back(std::make_pair(key,field_size));
      }
  }
};
