#include "../Include/BinaryReader.h"

//open a binary reader at path
BinaryReader::BinaryReader(std::string filepath)
{
	//open the filestream
	filestream = std::ifstream(filepath, std::ios::binary | std::ios::in);
	//allocate RemainderByte
	RemainderByte = (char*)malloc(sizeof(char));
}
//destroy the binary reader
BinaryReader::~BinaryReader() 
{
	//close the filestream
	filestream.close();
	//free the RemainderByte
	free(RemainderByte);
}

//ifstream functions
bool BinaryReader::good() { return filestream.good(); }
bool BinaryReader::eof() { return filestream.eof(); }
bool BinaryReader::fail() { return filestream.fail(); }
bool BinaryReader::bad() { return filestream.bad(); }
void BinaryReader::close() { filestream.close(); }
//operators
BinaryReader::operator bool() { return filestream.operator bool; }
bool BinaryReader::operator!() { return filestream.operator!; }

//reads in bits bits
BR_RETURN_INT_TYPE BinaryReader::Read(unsigned BR_RETURN_INT_TYPE bits)
{
	//do we have remainder bits?
	if (BitsRemaining <= 0) 
	{
		//no, read in a new remainder byte
		filestream.read(RemainderByte, 1);
		BitsRemaining = 8;
	}

	//evaluate how many bits we are required to read in
	int requiredNewBits = bits - BitsRemaining;

	//do we already have enough bits?
	if (requiredNewBits <= 0) 
	{
		//yes, return the required number of bits
		int result = *RemainderByte & (int)(pow(2, bits) - 1); // mask the remainder
		*RemainderByte >>= bits; // shift the remainder
		BitsRemaining -= bits; // lower the remainder count
		return result;
	}

	//determine how many bytes we have to read in
	int requiredNewBytes = ceil(requiredNewBits / 8.0);
	//allocate space for and read in the new bytes
	char* newBytes = (char*)malloc(sizeof(char) * requiredNewBytes);
	filestream.read(newBytes, requiredNewBytes); 

	//begin answer construction
	BR_RETURN_INT_TYPE result;

	//start with all of the remainder bits
	result += (int)RemainderByte;
	BitsRemaining = 0;

	//iterate through the data
	for (int i = 0; i < requiredNewBytes; i++) 
	{
		//make room for the next byte
		result <<= 8;
		//add the next byte
		result += (int)newBytes[i];
	}

	//free up the space we allocated for the data
	free((void*)newBytes);

	//return the result
	return result;
}