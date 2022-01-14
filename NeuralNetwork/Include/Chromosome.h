#ifndef CHROMOSOME_H
#define CHROMOSOME_H

#include "BinaryReader.h"

#include <vector>
#include <string>

//a chromosome consisting of genes in a file
template<class GeneSegmentType>
class Chromosome
{
	//the binary reader for the file
	BinaryReader FileReader;
	
	//the bit pattern for this
	std::vector<int> BitPattern;
public:

	//opens a chromosome at ChromosomePath with bit read pattern BitPattern
	Chromosome(std::string ChromosomePath, std::vector<int> &_BitPattern);
	//close the chromosome
	~Chromosome();

	//read the next gene
	GeneSegmentType* ReadGene();

	//BinaryReader functions
	bool good();
	bool eof();
	bool fail();
	bool bad();
	void close();
	//operators
	operator bool();
	bool operator!();
};

#endif