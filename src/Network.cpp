#include "../Include/Network.h"

//creates a network based on the .genome file at GenomePath
Network::Network(std::string GenomePath, int inputs, int outputs, float (*ActivationFunction)(float), bool Verbose) : Nodes(), InputNodes(), OutputNodes()
{
	//all of the connection genes in this network
	std::vector<ConnectionGene> ConnectionGenes;
	//all of the node genes in this network
	std::vector<NodeGene> NodeGenes;

	//store the activation function
	this->ActivationFunction = ActivationFunction;

	//create the file stream
	//std::ifstream GenomeReader(GenomePath, std::ios::in | std::ios::binary);
	BinaryReader GenomeReader(GenomePath);
	//did we successfully open the genome file	?
	if (!GenomeReader)
	{
		//No, log an error in console
		std::stringstream err;
        err << "Error Opening Genome \"" << GenomePath << "\"";
		throw std::runtime_error(err.str().c_str());
	}

	//start reading the genome
	while (!GenomeReader.eof())
	{
		//get the start of the next gene
		int bit = GenomeReader.Read(1);
		std::cout << bit << std::endl;
        bool GeneType = bit == 1;

		//decide what we should do based on the gene type(True: Node, False: Connection)
		if (GeneType)
		{
			//Node gene

			//separate the gene data into it's parts using bitmasks and bitshifts and store it all in a gene object
			NodeGene Gene;
			Gene.Bias = GenomeReader.Read(15) / NODE_GENE_BIAS_DIVISOR;
			if (Verbose) 
			{
				std::cout << "Node Gene Constructed as: " << Gene.ToString() << std::endl << std::endl;
			}

			//exit if we find that we've overread
			if (GenomeReader.eof())
			{
				//we've overread. we can ignore this gene as it's incomplete
				break;
			}

			//store the gene
			NodeGenes.push_back(Gene);
		}
		else
		{
			//Connection Gene

			//separate the gene data into it's parts using bitmasks and bitshifts and store it all in a gene object
			ConnectionGene Gene;
			Gene.SourceType = GenomeReader.Read(1) == 1;
			Gene.TargetType = GenomeReader.Read(1) == 1;
			Gene.SourceID = GenomeReader.Read(10);
			Gene.TargetID = GenomeReader.Read(10);
			Gene.Weight = GenomeReader.Read(9) / CONNECTION_GENE_WEIGHT_DIVISOR;
			if (Verbose)
			{
				std::cout << "Connection Gene Constructed as: " << Gene.ToString() << std::endl << std::endl;
			}

			//exit if we find that we've overread
			if (GenomeReader.eof())
			{
				//we've overread. we can ignore this gene as it's incomplete
				break;
			}

			//store the gene
			ConnectionGenes.push_back(Gene);
		}
	}
	//close the reader now that we've finished reading the file
	GenomeReader.close();

	//now that we have all of the node and connection genes, we need to create the nodes
	for (std::vector<NodeGene>::iterator GeneIter = NodeGenes.begin(); GeneIter != NodeGenes.end(); GeneIter++) 
	{
		//create a node from the current gene
		Nodes.push_back(new Node(*GeneIter));
	}
	//we can safely discard the node genes now, as we've used them
	NodeGenes.clear();

	//create the input and output nodes
	NodeGene BlankGene = NodeGene();
	for(int i = 0; i < inputs; i++)
	{
		//create a node based on the blank gene
		InputNodes.push_back(new Node(BlankGene));
		if (Verbose)
		{
			std::cout << "Input Node _ Created: " << i << std::endl;
		}
	}
	for(int i = 0; i < outputs; i++)
	{
		//create a node based on the blank gene
		OutputNodes.push_back(new Node(BlankGene));
		if (Verbose)
		{
			std::cout << "Output Node _ Created: " << i << std::endl;
		}
	}
	if (Verbose)
	{
		std::cout << "Input Node Size: " << InputNodes.size() << std::endl;
		std::cout << "Output Node Size: " << OutputNodes.size() << std::endl;
	}

	//now that we have all of the nodes created, we need to create the connections between them
	for(std::vector<ConnectionGene>::iterator GeneIter = ConnectionGenes.begin(); GeneIter != ConnectionGenes.end(); GeneIter++)
	{
		//what kind of target does the gene have
		if (GeneIter->TargetType)
		{
			//output, we can use the network's output node array
			//we modulo the gene's target id to ensure it always gets a node, no matter what the value is
			OutputNodes[GeneIter->TargetID % OutputNodes.size()]->Connections.push_back(Connection(*GeneIter, *this));
		}
		else 
		{
			//node, we can use the network's node array
			//we modulo the gene's target id to ensure it always gets a node, no matter what the value is
			Nodes[GeneIter->TargetID % Nodes.size()]->Connections.push_back(Connection(*GeneIter, *this));
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
	for(std::vector<Node*>::iterator NodeIter = OutputNodes.begin(); NodeIter != OutputNodes.end(); NodeIter++)
	{
		//store the value for this node
		Values.push_back((*NodeIter)->CalculateValue(this));
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
		std::vector<Node*>::iterator NodeIter = InputNodes.begin();
		for(; NodeIter != InputNodes.end(); InputIter++, NodeIter++)
		{
			//set the value for this input node to it's corresponding input value
			(*NodeIter)->Value = *InputIter;
		}

		//and also set all of our nodes and output nodes to need to recalculate their values
		for (NodeIter = Nodes.begin(); NodeIter != Nodes.end(); NodeIter++) 
		{
			//tell this node that it needs to recalculate
			(*NodeIter)->NeedsToRecalculate = true;
		}
		for (NodeIter = OutputNodes.begin(); NodeIter != OutputNodes.end(); NodeIter++)
		{
			//tell this node that it needs to recalculate
			(*NodeIter)->NeedsToRecalculate = true;
		}
	}
	else 
	{
		//Nope, throw an exception
		std::cout << "Invalid number of inputs to network" << std::endl;
		throw std::runtime_error("Invalid number of inputs to network");
	}
}

//destroys the network
Network::~Network() 
{
	//iterate through all of the nodes and delete them
	for (Node* Node : InputNodes) 
	{
		delete Node;
	}
	for (Node* Node : Nodes)
	{
		delete Node;
	}
	for (Node* Node : OutputNodes)
	{
		delete Node;
	}
	
	//clear all of the pointer vectors
	if(!InputNodes.empty())
		InputNodes.clear();
	if (!Nodes.empty())
		Nodes.clear();
	if (!OutputNodes.empty())
		OutputNodes.clear();
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
void Network::SaveNetwork(std::string GenomePath, bool verbose)
{
	//open the file at the genome path
	std::ofstream GenomeFile(GenomePath.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);

	//iterate through all of the internal nodes in the network to save them and their connections
	for(std::vector<Node*>::iterator NodeIter = Nodes.begin(); 
		NodeIter != Nodes.end(); 
		NodeIter++)
	{
		//iterate through all of the connections for this node
		for (std::vector<Connection>::iterator ConnectionIter = (*NodeIter)->Connections.begin();
			ConnectionIter != (*NodeIter)->Connections.end();
			ConnectionIter++) 
		{
			//save this connection in the file
			ConnectionIter->AsGene(this, false, NodeIter - Nodes.begin()).AppendGene(GenomeFile, verbose);
		}
		//save this node in the file
		(*NodeIter)->AsGene().AppendGene(GenomeFile, verbose);
	}

	//iterate through all of the output nodes in the network to save ONLY their connections
	for (std::vector<Node*>::iterator NodeIter = OutputNodes.begin();
		NodeIter != OutputNodes.end();
		NodeIter++)
	{
		//iterate through all of the connections for this node
		for (std::vector<Connection>::iterator ConnectionIter = (*NodeIter)->Connections.begin();
			ConnectionIter != (*NodeIter)->Connections.end();
			ConnectionIter++)
		{
			//save this connection in the file
			ConnectionIter->AsGene(this, true, NodeIter - OutputNodes.begin()).AppendGene(GenomeFile, verbose);
		}
	}

	//close the file stream
	GenomeFile.close();
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
void ConnectionGene::AppendGene(std::ofstream &stream, bool verbose)
{
	//get this gene as a byte array
	unsigned char Gene[4] = {0, 0, 0, 0};

	//get this gene's weight and sign as an integer
	int IntWeight = roundl(abs(Weight * CONNECTION_GENE_WEIGHT_DIVISOR));

	//store the gene's component parts in the byte array
	Gene[0] += 0b01000000 * SourceType; // gene source type
	Gene[0] += 0b00100000 * TargetType; // gene target type
	Gene[0] += (SourceID >> 5) & 0b00011111; // first 5 bits of source id
	Gene[1] += (SourceID & 0b00011111) << 3; // last 5 bits of source id
	Gene[1] += (TargetID >> 7) & 0b00000111; // first 3 bits of target id
	Gene[2] += (TargetID & 0b01111111) << 1; // last 7 bits of target id
	Gene[2] += Weight < 0 ? 0b00000001 : 0; // sign of int weight
	Gene[3] = (unsigned char)IntWeight; // and we can just store the rest of the weight here

	//send the bytes to the stream
	stream << Gene[0] << Gene[1] << Gene[2] << Gene[3];

	if (verbose) 
	{
		std::cout << "Appended gene to file: " << ToString() << std::endl;
		std::cout << "BIN: " << (int)Gene[0] << ", " << (int)Gene[1] << ", " << (int)Gene[2] << ", " << (int)Gene[3] << std::endl << std::endl;
	}
}

//appends this gene to a filestream
void NodeGene::AppendGene(std::ofstream &stream, bool verbose)
{
	//get this gene as a byte array
	unsigned char Gene[2] = {0, 0};
	
	//get this gene's bias as an integer
	int IntBias = roundl(Bias * NODE_GENE_BIAS_DIVISOR);

	//store the gene's component parts in the byte array
	Gene[0] += 0b10000000; // this is a node gene
	Gene[0] += IntBias < 0 ? 0b01000000 : 0; //sign bit
	Gene[0] += (IntBias & 0b0011111100000000) >> 8; // first 6 bits of the number
	Gene[1] += IntBias & 0b0000000011111111; // rest of the number


	//send the bytes to the stream
	stream << Gene[0] << Gene[1];

	if (verbose)
	{
		std::cout << "Appended gene to file: " << ToString() << std::endl;
		std::cout << "BIN: " << (int)Gene[0] << ", " << (int)Gene[1] << std::endl << std::endl; 
	}
}