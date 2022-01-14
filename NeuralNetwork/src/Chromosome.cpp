#include "../Include/Chromosome.h"

//opens a chromosome at ChromosomePath with bit read pattern ...
Chromosome::Chromosome(std::string ChromosomePath, std::vector<int> &_BitPattern) : FileReader(ChromosomePath)
{
	//copy the bit pattern
	BitPattern = _BitPattern;
}

//close the chromosome
Chromosome::~Chromosome()
{
	//close the binary reader
	FileReader.close();
}

//read the next gene
BR_RETURN_INT_TYPE* Chromosome::ReadGene()
{
	//create the gene array
	BR_RETURN_INT_TYPE* Gene = new BR_RETURN_INT_TYPE[BitPattern.size()];

	//iterate through the bit pattern
	for (int i = 0; i < BitPattern.size(); i++) 
	{
		Gene[i] = FileReader.Read(BitPattern[i]);
	}

	//return the gene
	return Gene;
}

//binaryreader functions
bool Chromosome::good() { return FileReader.good(); }
bool Chromosome::eof() { return FileReader.eof(); }
bool Chromosome::fail() { return FileReader.fail(); }
bool Chromosome::bad() { return FileReader.bad(); }
void Chromosome::close() { FileReader.close(); }
//operators
Chromosome::operator bool() { return FileReader ? true : false; }
bool Chromosome::operator!() { return !FileReader; }