#include "../Include/Chromosome.h"

//opens a chromosome at ChromosomePath with bit read pattern ...
template<class GeneSegmentType>
Chromosome<GeneSegmentType>::Chromosome(std::string ChromosomePath, std::vector<int> &_BitPattern) : FileReader(ChromosomePath)
{
	//copy the bit pattern
	BitPattern = _BitPattern;
}

//close the chromosome
template<class GeneSegmentType>
Chromosome<GeneSegmentType>::~Chromosome()
{
	//close the binary reader
	FileReader.close();
}

//read the next gene
template<class GeneSegmentType>
GeneSegmentType* Chromosome<GeneSegmentType>::ReadGene()
{
	//create the gene array
	GeneSegmentType* Gene = new GeneSegmentType[BitPattern.size()];

	//iterate through the bit pattern
	for (int i = 0; i < BitPattern.size(); i++) 
	{
		Gene[i] = FileReader.Read(BitPattern[i]);
	}

	//return the gene
	return Gene;
}

//binaryreader functions
template<class GeneSegmentType>
bool Chromosome<GeneSegmentType>::good() { return FileReader.good(); }
template<class GeneSegmentType>
bool Chromosome<GeneSegmentType>::eof() { return FileReader.eof(); }
template<class GeneSegmentType>
bool Chromosome<GeneSegmentType>::fail() { return FileReader.fail(); }
template<class GeneSegmentType>
bool Chromosome<GeneSegmentType>::bad() { return FileReader.bad(); }
template<class GeneSegmentType>
void Chromosome<GeneSegmentType>::close() { FileReader.close(); }
//operators
template<class GeneSegmentType>
Chromosome<GeneSegmentType>::operator bool() { return FileReader ? true : false; }
template<class GeneSegmentType>
bool Chromosome<GeneSegmentType>::operator!() { return !FileReader; }