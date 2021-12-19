#ifndef BINARYREADER_H
#define BINARYREADER_H
/* BinaryReader.H
 * Contains the `BinaryReader` Class
 */

#include <fstream>
#include <math.h>

//ensure that we always have a valid int type
#ifndef BR_RETURN_INT_TYPE
#define BR_RETURN_INT_TYPE int
#endif

//acts like a regular filestream, but returns bits at a time instead of bytes
class BinaryReader
{
	//the filestream for this object
	std::ifstream filestream;

	char* RemainderByte; //the byte left over from the last read
	int BitsRemaining; //the bits left in RemainderByte

public:

	//reads in bits bits
	BR_RETURN_INT_TYPE Read(unsigned BR_RETURN_INT_TYPE bits);

	//ifstream functions
	bool good();
	bool eof();
	bool fail();
	bool bad();
	//operators
	operator bool();
	bool operator!();
};

#endif