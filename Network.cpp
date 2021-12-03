#include "Include/Network.h"

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
	int byteIndex = 0;
	while (!GenomeReader.eof())
	{
		//get the start of the next gene
        GenomeReader.get(byte);

		//decide what we should do based on the gene type(True: Node, False: Connection)
		if ((((int)byte & 0b10000000) >> 7) == 1)
		{
			//Node gene
			int gene[2];

			//get the rest of the gene
			gene[0] = (int)byte;
			GenomeReader.get(byte);
			gene[1] = (int)byte;
			byteIndex += 1;

            //exit if we find that we've overread
            if(GenomeReader.eof())
            {
                //we've overread. we can ignore this gene as it's incomplete
                break;
            }

			std::cout << "Found Node gene at byte " << byteIndex << "-" << byteIndex + 1 << std::endl;
			std::cout << gene[0] << " " << gene[1] << std::endl;
		}
		else
		{
			//Connection Gene
			int gene[4];

			//get the rest of the gene
			gene[0] = (int)byte;
			GenomeReader.get(byte);
			gene[1] = (int)byte;
			GenomeReader.get(byte);
			gene[2] = (int)byte;
			GenomeReader.get(byte);
			gene[3] = (int)byte;
			byteIndex += 3;

            //exit if we find that we've overread
            if(GenomeReader.eof())
            {
                //we've overread. we can ignore this gene as it's incomplete
                break;
            }

            std::cout << "Found Connection gene at byte " << byteIndex << "-" << byteIndex + 3 << std::endl;
			std::cout << gene[0] << " " << gene[1] << " " << gene[2] << " " << gene[3] << std::endl;
		}
		byteIndex++;
	}
}

//creates an empty network
//Network::Network() {}

//creates a network from a pre-existing vector of Node and Connection Genes
/*Network(std::vector<ConnectionGene> &ConnectionGenes, std::vector<NodeGene> &NodeGenes)
{
    
}*/