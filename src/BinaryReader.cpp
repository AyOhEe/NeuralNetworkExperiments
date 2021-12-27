#include "../Include/BinaryReader.h"

#include <iostream>

//open a binary reader at path
BinaryReader::BinaryReader(std::string filepath)
{
	//open the filestream
	filestream = std::ifstream(filepath, std::ios::binary | std::ios::in);
	
	//initialise the remainder byte and remaining bits
	RemainderByte = 0;
	BitsRemaining = 0;
}
//destroy the binary reader
BinaryReader::~BinaryReader() 
{
	//close the filestream
	filestream.close();
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
	std::cout << "READ WITH _ BITS REMAINING AND REMAINDER _ : " << BitsRemaining << ", " << (unsigned int)RemainderByte << std::endl;

	//do we have remainder bits?
	if (BitsRemaining <= 0) 
	{
		//no, read in a new remainder byte
		filestream.read((char*)&RemainderByte, 1);
		std::cout << (unsigned int)RemainderByte << std::endl;
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
		BR_RETURN_INT_TYPE result = (RemainderByte >> (8 - bits)); // mask the remainder
		RemainderByte <<= bits; // shift the remainder
		BitsRemaining -= bits; // lower the remainder count
		std::cout << "EARLY RETURNING " << result << std::endl << std::endl;
		return result;
	}

	//determine how many bytes we have to read in
	int requiredNewBytes = (int)ceil(requiredNewBits / 8.0);
	std::cout << "REQUIRE _ BYTES, _ BITS: " << requiredNewBytes << ", " << requiredNewBits << std::endl;
	//allocate space for and read in the new bytes
	unsigned char* newBytes = new unsigned char[requiredNewBytes];
	filestream.read((char*)newBytes, requiredNewBytes); 

	//begin answer construction
	BR_RETURN_INT_TYPE result = 0;

	//start with all of the remainder bits
	result += RemainderByte >> (8 - BitsRemaining);
	std::cout << "STARTING RESULT AS " << result << std::endl;
	BitsRemaining = 0;

	//iterate through the data(excluding the last byte)
	for (int i = 0; i < requiredNewBytes - 1; i++) 
	{
		std::cout << (int)(newBytes[i]) << std::endl;

		//make room for the next byte
		result <<= 8;
		//add the next byte
		result += (int)(newBytes[i]);
	}
	//calculate how many more bits we need
	int FinalRequiredBits = requiredNewBits - ((requiredNewBytes - 1) * 8);
	std::cout << "REQUIRE " << FinalRequiredBits << " BITS AT END" << std::endl;
	//add on those bits
	result <<= FinalRequiredBits;
	result += newBytes[requiredNewBytes - 1] >> (8 - FinalRequiredBits);

	//store the unused bits
	RemainderByte = newBytes[requiredNewBytes - 1] << FinalRequiredBits;
	BitsRemaining = 8 - FinalRequiredBits;

	//free up the space we allocated for the data
	delete[] newBytes;

	//return the result
	std::cout << "LATE RETURNING " << result << std::endl << std::endl;
	return result;
}