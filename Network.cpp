#include "Include/Network.h"

#include <fstream>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <iomanip>

//creates a network based on the .genome file at GenomePath
Network::Network(std::string GenomePath)
{
	//all of the connection genes in this network
	std::vector<ConnectionGene> ConnectionGenes;
	//all of the node genes in this network
	std::vector<NodeGene> NodeGenes;

	//create the file stream
	std::ifstream GenomeReader(GenomePath, std::ios::binary | std::ios::in);
	//did we successfully open the genome file	?
	if (!GenomeReader)
	{
		//No, log an error in console
		std::stringstream err;
        err << "Error Opening Genome \"" << GenomePath << "\"";
		throw err.str().c_str();
	}

	//start reading the genome
	char byte;
    GenomeReader.get(byte);
    std::stringstream stream;
    const char* HexMap = "0123456789ABCDEF";
	while (!GenomeReader.eof())
	{
        unsigned short int ushortByte = (unsigned short int)byte;
        GenomeReader.get(byte);
        std::cout << "Char 1 index: " << ((ushortByte >> 4) % 16) << std::endl;
        std::cout << "Char 2 index: " << (ushortByte % 16) << std::endl;
        stream << HexMap[(ushortByte >> 4) % 16] << HexMap[ushortByte % 16];
	}
    std::cout << stream.str() << std::endl;
}

//creates an empty network
//Network::Network() {}

//creates a network from a pre-existing vector of Node and Connection Genes
/*Network(std::vector<ConnectionGene> &ConnectionGenes, std::vector<NodeGene> &NodeGenes)
{
    
}*/