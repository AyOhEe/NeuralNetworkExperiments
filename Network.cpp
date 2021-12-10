#include "Include/Network.h"

//creates a network based on the .genome file at GenomePath
Network::Network(std::string GenomePath, int inputs, int outputs, float (*ActivationFunction)(float)) : Nodes(), InputNodes(), OutputNodes()
{
	//all of the connection genes in this network
	std::vector<ConnectionGene> ConnectionGenes;
	//all of the node genes in this network
	std::vector<NodeGene> NodeGenes;

	//store the activation function
	ActivationFunction = ActivationFunction;

	//create the file stream
	std::ifstream GenomeReader(GenomePath, std::ios::binary | std::ios::in);
	//did we successfully open the genome file	?
	if (!GenomeReader)
	{
		//No, log an error in console
		std::stringstream err;
        err << "Error Opening Genome \"" << GenomePath << "\"";
		throw std::runtime_error(err.str().c_str());
	}

	//start reading the genome
	char byte;
	int byteIndex = 0;
	while (!GenomeReader.eof())
	{
		//get the start of the next gene
        GenomeReader.get(byte);

		//decide what we should do based on the gene type(True: Node, False: Connection)
		if (((byte & 0b10000000) >> 7) == 1)
		{
			//Node gene
			unsigned char GeneBytes[2];

			//get the rest of the gene
			GeneBytes[0] = byte;
			GenomeReader.get(byte);
			GeneBytes[1] = byte;
			byteIndex += 1;

            //exit if we find that we've overread
            if(GenomeReader.eof())
            {
                //we've overread. we can ignore this gene as it's incomplete
                break;
            }

			std::cout << "Found Node gene at byte " << byteIndex << "-" << byteIndex + 1 << std::endl;
			std::cout << (int)GeneBytes[0] << " " << (int)GeneBytes[1] << std::endl;

			//separate the gene data into it's parts using bitmasks and bitshifts and store it all in a gene object
			NodeGene Gene;
			Gene.Bias = (((GeneBytes[0] & 0b01111111) << 8) + GeneBytes[1]) / NODE_GENE_BIAS_DIVISOR;
			std::cout << "Gene Constructed as: " << Gene.ToString() << std::endl << std::endl;

			//store the gene
			NodeGenes.push_back(Gene);
		}
		else
		{
			//Connection Gene
			unsigned char GeneBytes[4];

			//get the rest of the gene
			GeneBytes[0] = byte;
			GenomeReader.get(byte);
			GeneBytes[1] = byte;
			GenomeReader.get(byte);
			GeneBytes[2] = byte;
			GenomeReader.get(byte);
			GeneBytes[3] = byte; //i shouldn't have to do this, but it ensures the byte is stored correctly
			byteIndex += 3;

            //exit if we find that we've overread
            if(GenomeReader.eof())
            {
                //we've overread. we can ignore this gene as it's incomplete
                break;
            }

            std::cout << "Found Connection gene at byte " << byteIndex << "-" << byteIndex + 3 << std::endl;
			std::cout << (int)GeneBytes[0] << " " << (int)GeneBytes[1] << " " << (int)GeneBytes[2] << " " << (int)GeneBytes[3] << std::endl;

			//separate the gene data into it's parts using bitmasks and bitshifts and store it all in a gene object
			ConnectionGene Gene;
			Gene.SourceType = (GeneBytes[0] & 0b01000000) == 0b01000000;
			Gene.TargetType = (GeneBytes[0] & 0b00100000) == 0b00100000;
			Gene.SourceID = ((GeneBytes[0] & 0b00011111) << 5) + ((GeneBytes[1] & 0b11111000) >> 3);
			Gene.TargetID = ((GeneBytes[1] & 0b00000111) << 7) + ((GeneBytes[2] & 0b11111110) >> 1);
			Gene.Weight = ((GeneBytes[2] & 0b00000001) == 0b00000001 ? -1 : 1) * (GeneBytes[3] / 128.0f);
			std::cout << "Gene Constructed as: " << Gene.ToString() << std::endl << std::endl;

			//store the gene
			ConnectionGenes.push_back(Gene);
		}
		byteIndex++;
	}
	//close the reader now that we've finished reading the file
	GenomeReader.close();

	//now that we have all of the node and connection genes, we need to create the nodes
	for (std::vector<NodeGene>::iterator GeneIter = NodeGenes.begin(); GeneIter != NodeGenes.end(); GeneIter++) 
	{
		//create a node from the current gene
		Nodes.push_back(Node(*GeneIter));
	}
	//we can safely discard the node genes now, as we've used them
	NodeGenes.clear();

	//create the input and output nodes
	NodeGene BlankGene = NodeGene();
	for(int i = 0; i < inputs; i++)
	{
		//create a node based on the blank gene
		InputNodes.push_back(Node(BlankGene));
		std::cout << "Input Node _ Created: " << i << std::endl;
	}
	for(int i = 0; i < outputs; i++)
	{
		//create a node based on the blank gene
		OutputNodes.push_back(Node(BlankGene));
		std::cout << "Output Node _ Created: " << i << std::endl;
	}
	std::cout << "Input Node Size: " << InputNodes.size() << std::endl;
	std::cout << "Output Node Size: " << OutputNodes.size() << std::endl;

	//now that we have all of the nodes created, we need to create the connections between them
	for(std::vector<ConnectionGene>::iterator GeneIter = ConnectionGenes.begin(); GeneIter != ConnectionGenes.end(); GeneIter++)
	{
		//what kind of target does the gene have
		if (GeneIter->TargetType)
		{
			//output, we can use the network's output node array
			//we modulo the gene's target id to ensure it always gets a node, no matter what the value is
			OutputNodes[GeneIter->TargetID % OutputNodes.size()].Connections.push_back(Connection(*GeneIter, *this));
		}
		else 
		{
			//node, we can use the network's node array
			//we modulo the gene's target id to ensure it always gets a node, no matter what the value is
			Nodes[GeneIter->TargetID % Nodes.size()].Connections.push_back(Connection(*GeneIter, *this));
		}
	}
	//we can safely discard the connection genes now, as we've used them
	ConnectionGenes.clear();
}

//returns the values of all of the output nodes
std::vector<float> Network::GetResults() 
{
	//vector to store the values in the nodes
	std::vector<float> Values;

	//iterate through the nodes and store their values
	for(std::vector<Node>::iterator NodeIter = OutputNodes.begin(); NodeIter != OutputNodes.end(); NodeIter++)
	{
		//store the value for this node
		Values.push_back(NodeIter->Value);
	}

	//return the values we got
	return Values;
}

//sets the values of all of the input nodes
void Network::SetInputs(std::vector<float> &Inputs) 
{
	//do we have the same number of inputs as input nodes?
	if (Inputs.size() == InputNodes.size()) 
	{
		//yes, carry on and set the inputs
		std::vector<float>::iterator InputIter = Inputs.begin();
		std::vector<Node>::iterator NodeIter = InputNodes.begin();
		for(; NodeIter != InputNodes.end(); InputIter++, NodeIter++)
		{
			//set the value for this input node to it's corresponding input value
			NodeIter->Value = *InputIter;
		}
	}
	else 
	{
		//Nope, throw an exception
		std::cout << "Invalid number of inputs to network" << std::endl;
		throw std::runtime_error("Invalid number of inputs to network");
	}
}

//creates a blank network
Network::Network() 
{

}

//creates a network from a pre-existing vector of Node and Connection Genes
/*Network(std::vector<ConnectionGene> &ConnectionGenes, std::vector<NodeGene> &NodeGenes, float (*ActivationFunction)(float))
{
	//store the activation function
	ActivationFunction = ActivationFunction;
    
}*/

//saves the network to a file on disk
void SaveNetwork(std::string GenomePath) 
{
	
}

//returns a representation of this gene as a string
std::string ConnectionGene::ToString()
{
	std::stringstream str;
	str << (SourceType ? "Hidden" : "Input");
	str << ", " << (TargetType ? "Output" : "Hidden");
	str << ", " << SourceID;
	str << ", " << TargetID;
	str << ", " << Weight;
	return str.str();
}
//returns a representation of this gene as a string
std::string NodeGene::ToString()
{
	std::stringstream str;
	str << Bias;
	return str.str();
}


//appends this gene to a filestream
void ConnectionGene::AppendGene(std::ofstream stream) 
{
	
}

//appends this gene to a filestream
void NodeGene::AppendGene(std::ofstream stream) 
{
	//get this gene as a byte array
	unsigned char Gene[2];

	//store the gene's component parts in the byte array
	Gene[0] += 0b10000000;
	Gene[0] += 0b0111111111111111 & (int)(Bias * NODE_GENE_BIAS_DIVISOR);
}