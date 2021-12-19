#include "../Include/BinaryReader.h"

#include <iostream>

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
BinaryReader::operator bool() { return filestream ? true : false; }
bool BinaryReader::operator!() { return !filestream; }

//reads in bits bits
BR_RETURN_INT_TYPE BinaryReader::Read(unsigned int bits)
{

	std::cout << "READ WITH _ BITS REMAINING : " << BitsRemaining << std::endl;

	//do we have remainder bits?
	if (BitsRemaining <= 0) 
	{
		//no, read in a new remainder byte
		filestream.read(RemainderByte, 1);
		std::cout << (int)(*RemainderByte) << std::endl;
		BitsRemaining = 8;
		std::cout << "REMAINDER INSUFFICIENT" << std::endl;
	}

	//evaluate how many bits we are required to read in
	int requiredNewBits = bits - BitsRemaining;
	std::cout << bits << " - " << BitsRemaining << " = " << requiredNewBits << std::endl;

	//do we already have enough bits?
	if (requiredNewBits <= 0) 
	{
		//yes, return the required number of bits
		BR_RETURN_INT_TYPE result = (unsigned int)*RemainderByte & (int)(pow(2, bits) - 1); // mask the remainder
		*RemainderByte >>= bits; // shift the remainder
		BitsRemaining -= bits; // lower the remainder count
		std::cout << "RETURNING " << result << std::endl << std::endl;
		return result;
	}

	//determine how many bytes we have to read in
	int requiredNewBytes = (int)ceil(requiredNewBits / 8.0);
	std::cout << "REQUIRE _ BYTES, _ BITS: " << requiredNewBytes << ", " << requiredNewBits << std::endl;
	//allocate space for and read in the new bytes
	char* newBytes = new char[requiredNewBytes];
	filestream.read(newBytes, requiredNewBytes); 

	//begin answer construction
	BR_RETURN_INT_TYPE result = 0;

	//start with all of the remainder bits
	result += (int)RemainderByte;
	BitsRemaining = 0;

	//iterate through the data
	for (int i = 0; i < requiredNewBytes; i++) 
	{
		std::cout << (int)(newBytes[i]) << std::endl;

		//make room for the next byte
		result <<= 8;
		//add the next byte
		result += (unsigned int)(newBytes[i]);
	}

	//free up the space we allocated for the data
	delete[] newBytes;

	//return the result
	std::cout << "RETURNING " << result << std::endl << std::endl;
	return result;
}