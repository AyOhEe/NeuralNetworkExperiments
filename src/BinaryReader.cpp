#include "../Include/BinaryReader.h"

//ifstream functions
bool BinaryReader::good() { return filestream.good(); }
bool BinaryReader::eof() { return filestream.eof(); }
bool BinaryReader::fail() { return filestream.fail(); }
bool BinaryReader::bad() { return filestream.bad(); }
//operators
BinaryReader::operator bool() { return filestream.operator bool; }
bool BinaryReader::operator!() { return filestream.operator!; }