#ifndef BINARYREADER_H
#define BINARYREADER_H
/* BinaryReader.H
 * Contains the `BinaryReader` Class
 */

#include <fstream>

//acts like a regular filestream, but returns bits at a time instead of bytes
class BinaryReader
{
	//the filestream for this object
	std::ifstream filestream;

	char RemainderByte; //the byte left over from the last read
	int BitsRemaining; //the bits left in RemainderByte

public:

	//reads in bits bits
	char* Read(int bits);

	//ifstream functions
	bool good() { return filestream.good(); }
	bool eof() { return filestream.eof(); }
	bool fail() { return filestream.fail(); }
	bool bad() { return filestream.bad(); }
	//operators
	operator bool() { return filestream.operator bool; }
	bool operator!() { return filestream.operator!; }
};

#endif