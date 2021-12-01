#include "Include/Network.h"

#include <fstream>
#include <vector>
#include <iostream>

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
		std::string err("Error Opening Genome \"");
		err.append(GenomePath);
		err.append("\"");
		throw err.c_str();
	}

	//start reading the genome
	char byte;
	while (!GenomeReader.eof())
	{
		GenomeReader.get(byte);
		std::cout << byte;
	}
}

//creates an empty network
Network::Network() 
{

}