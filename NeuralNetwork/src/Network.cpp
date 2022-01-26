#include "../Include/Network.h"

//returns sigmoid of x
float Sigmoid(float x)
{
	return (float)(1 / (1 + pow(2.718, -x)));
}
//returns ReLu of x
float ReLu(float x)
{
	return std::fmax(0, x);
}
//returns leaky ReLu of x
float LeakyReLu(float x)
{
	return std::fmax(0.1 * x, x);
}

//returns an activation function pointer based on it's index
float(*GetActivationFunctionPointer(int Index))(float)
{
	switch (Index)
	{
	case 1:
		return tanh;
		break;
	case 2:
		return ReLu;
		break;
	case 3:
		return LeakyReLu;
		break;
	default:
		return Sigmoid;
		break;
	}
}

//creates a network based on the genome at GenomePath
Network::Network(std::string GenomePath, int inputs, int outputs, int ActivationFunctionIndex, unsigned int* ErrCode, bool Verbose) :
	Network(GenomePath, inputs, outputs, GetActivationFunctionPointer(ActivationFunctionIndex), ErrCode, Verbose) {}

//creates a network based on the genome at GenomePath
Network::Network(std::string GenomePath, int inputs, int outputs, float(*ActivationFunction)(float), unsigned int* ErrCode, bool Verbose)
{
	try
	{
		//default the error code to success
		*ErrCode = SUCCESS;

		//make sure we were given a valid number of inputs or outputs
		if (inputs <= 0 | outputs <= 0)
		{
			//invalid!
			*ErrCode = NETWORK_INVALID_ARGUMENT;
			std::stringstream ErrorMessage;
			ErrorMessage << "Inputs/Outputs <= 0! This should not happen! " << __FILE__ << ":" << __LINE__;
			throw std::invalid_argument(ErrorMessage.str().c_str());
		}

		//store the activation function
		this->ActivationFunction = ActivationFunction;

		//create a temporary vector to store the bit patterns
		std::vector<int> NodeBitPattern = { 1, 1, 14 };
		std::vector<int> ConnectionBitPattern = { 1, 1, 1, 10, 10, 1, 8 };

		//create the chromosome readers
		Chromosome NodeChromosome(GenomePath + "/Nodes.chr", NodeBitPattern);
		Chromosome ConnectionChromosome(GenomePath + "/Connections.chr", ConnectionBitPattern);

		//make sure we could open the files
		if (!NodeChromosome | !ConnectionChromosome)
		{
			//we couldn't open one of them, throw an exception
			std::stringstream ErrorMessage;
			ErrorMessage << "Unable to open Chromosomes at " << __FILE__ << ":" << __LINE__ << ". Make sure Genome is valid";
			*ErrCode = NETWORK_INVALID_GENOME;
			throw std::invalid_argument(ErrorMessage.str().c_str());
		}

		//create the input and output nodes
		for (int i = 0; i < inputs; i++)
		{
			//create a node based on the blank gene
			InputNodes.push_back(Node(0.0f));
			if (Verbose)
			{
				std::cout << "Input Node _ Created: " << i << std::endl;
			}
		}
		for (int i = 0; i < outputs; i++)
		{
			//create a node based on the blank gene
			OutputNodes.push_back(Node(0.0f));
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

		//start reading in the node chromosome
		BR_RETURN_INT_TYPE* Gene;
		while (!NodeChromosome.eof())
		{
			//get the next gene
			Gene = NodeChromosome.ReadGene();

			//if we've hit the end of the file, ignore this gene and stop reading
			if (NodeChromosome.eof())
				break;

			//create and add it's node
			Nodes.insert(std::pair(UniqueNodeIndex++, Node(Gene)));

			if (Verbose)
			{
				std::cout << "Node created as " << Node::GeneAsString(Gene) << std::endl;
			}

			//destroy the gene now that it's been read
			delete[] Gene;
		}
		//close the node chromosome now that we're done
		NodeChromosome.close();

		//start reading the connection chromosome
		while (!ConnectionChromosome.eof())
		{
			//get the next gene
			Gene = ConnectionChromosome.ReadGene();

			//if we've hit the end of the file, ignore this gene and stop reading
			if (ConnectionChromosome.eof())
				break;

			//create and add it's connection
			Connection::CreateConnection(Gene, this);

			if (Verbose)
			{
				std::cout << "Connection created as " << Connection::GeneAsString(Gene) << std::endl;
			}

			//destroy the gene now that it's been read
			delete[] Gene;
		}
		//close the connection chromosome now that we're done
		ConnectionChromosome.close();

		if (Verbose)
		{
			int i = 0;
			for (std::map<long long, Node>::iterator nodeiter = Nodes.begin(); nodeiter != Nodes.end(); nodeiter++, i++)
			{
				std::cout << "Node " << i << " has " << nodeiter->second.Connections.size() << " connections" << std::endl;
			}
			i = 0;
			for (std::vector<Node>::iterator nodeiter = OutputNodes.begin(); nodeiter != OutputNodes.end(); nodeiter++, i++)
			{
				std::cout << "Output Node " << i << " has " << nodeiter->Connections.size() << " connections" << std::endl;
			}
		}
	}
	catch (std::exception &ex)
	{
		*ErrCode = UNKNOWN_FAILURE;
		std::stringstream ErrorMessage;
		ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
		std::cout << ErrorMessage.str() << std::endl << ex.what() << std::endl;
		return;
	}
}

//creates a network based on two genomes
Network::Network(std::string GenomePathA, std::string GenomePathB, BreedSettings Settings, int inputs, int outputs, int ActivationFunctionIndex, unsigned int* ErrCode, bool Verbose)
{
	try 
	{
		//default the error code to success
		*ErrCode = SUCCESS;

		//make sure we were given a valid number of inputs or outputs
		if (inputs <= 0 | outputs <= 0)
		{
			//invalid!
			*ErrCode = NETWORK_INVALID_ARGUMENT;
			std::stringstream ErrorMessage;
			ErrorMessage << "Inputs/Outputs <= 0! This should not happen! " << __FILE__ << ":" << __LINE__;
			throw std::invalid_argument(ErrorMessage.str().c_str());
		}

		//retrieve and store the activation function
		ActivationFunction = GetActivationFunctionPointer(ActivationFunctionIndex);

		//create a temporary vector to store the bit patterns
		std::vector<int> NodeBitPattern = { 1, 1, 14 };
		std::vector<int> ConnectionBitPattern = { 1, 1, 1, 10, 10, 1, 8 };

		//create the chromosome readers
		Chromosome NodeChromosomeA(GenomePathA + "/Nodes.chr", NodeBitPattern);
		Chromosome ConnectionChromosomeA(GenomePathA + "/Connections.chr", ConnectionBitPattern);
		Chromosome NodeChromosomeB(GenomePathB + "/Nodes.chr", NodeBitPattern);
		Chromosome ConnectionChromosomeB(GenomePathB + "/Connections.chr", ConnectionBitPattern);

		//make sure we could open the files
		if (!NodeChromosomeA | !ConnectionChromosomeA)
		{
			//we couldn't open one of them, throw an exception
			std::stringstream ErrorMessage;
			ErrorMessage << "Unable to open Chromosome A for breeding at " << __FILE__ << ":" << __LINE__ << ". Make sure Genome is valid";
			*ErrCode = NETWORK_INVALID_BREED_GENOME_A;
			throw std::invalid_argument(ErrorMessage.str().c_str());
		}
		if (!NodeChromosomeA | !ConnectionChromosomeA)
		{
			//we couldn't open one of them, throw an exception
			std::stringstream ErrorMessage;
			ErrorMessage << "Unable to open Chromosome A for breeding at " << __FILE__ << ":" << __LINE__ << ". Make sure Genome is valid";
			*ErrCode = NETWORK_INVALID_BREED_GENOME_B;
			throw std::invalid_argument(ErrorMessage.str().c_str());
		}

		//create the vectors for each chromosome's genes
		std::vector<BR_RETURN_INT_TYPE*> NodeGeneSetA;
		std::vector<BR_RETURN_INT_TYPE*> NodeGeneSetB;
		std::vector<BR_RETURN_INT_TYPE*> ConnectionGeneSetA;
		std::vector<BR_RETURN_INT_TYPE*> ConnectionGeneSetB;

		//read in the chromosomes
		BR_RETURN_INT_TYPE* Gene;
		while (!NodeChromosomeA.eof())
		{
			//read in, mutate and store the next gene
			Gene = NodeChromosomeA.ReadGene();

			//determine if we should mutate the gene
			if (std::rand() % 1000 < Settings.MutationChance)
			{
				//pick a section of the gene to mutate
				int SelectedGene = std::rand() % 3;
				Gene[SelectedGene] ^= 1 << (std::rand() % NodeBitPattern[SelectedGene]);
			}

			NodeGeneSetA.push_back(Gene);
		}
		while (!NodeChromosomeB.eof())
		{
			//read in, mutate and store the next gene
			Gene = NodeChromosomeB.ReadGene();

			//determine if we should mutate the gene
			if (std::rand() % 1000 < Settings.MutationChance)
			{
				//pick a section of the gene to mutate
				int SelectedGene = std::rand() % 3;
				Gene[SelectedGene] ^= 1 << (std::rand() % NodeBitPattern[SelectedGene]);
			}

			NodeGeneSetB.push_back(Gene);
		}

		while (!ConnectionChromosomeA.eof())
		{
			//read in, mutate and store the next gene
			Gene = ConnectionChromosomeA.ReadGene();

			//determine if we should mutate the gene
			if (std::rand() % 1000 < Settings.MutationChance)
			{
				//pick a section of the gene to mutate
				int SelectedGene = std::rand() % 7;
				Gene[SelectedGene] ^= 1 << (std::rand() % ConnectionBitPattern[SelectedGene]);
			}

			ConnectionGeneSetA.push_back(Gene);
		}
		while (!ConnectionChromosomeB.eof())
		{
			//read in, mutate and store the next gene
			Gene = ConnectionChromosomeB.ReadGene();

			//determine if we should mutate the gene
			if (std::rand() % 1000 < Settings.MutationChance)
			{
				//pick a section of the gene to mutate
				int SelectedGene = std::rand() % 7;
				Gene[SelectedGene] ^= 1 << (std::rand() % ConnectionBitPattern[SelectedGene]);
			}

			ConnectionGeneSetB.push_back(Gene);
		}

		//get the smallest chromosome size from each genome
		int MinNodeChromosomeLength = std::min(NodeGeneSetA.size() - 1, NodeGeneSetB.size() - 1);
		int MinConnectionChromosomeLength = std::min(ConnectionGeneSetA.size() - 1, ConnectionGeneSetB.size() - 1);

		if (Verbose)
		{
			std::cout << "MinNodeChromosomeLength: " << MinNodeChromosomeLength << std::endl;
			std::cout << "MinConnectionChromosomeLength: " << MinConnectionChromosomeLength << std::endl;
		}

		//determine how many genes to read before crossing over each time
		std::vector<int> NodeChromosomeCrossoverPointGeneCount;
		std::vector<int> ConnectionChromosomeCrossoverPointGeneCount;
		int LastDeltaNodeCrossover = 0;
		int LastDeltaConnectionCrossover = 0;
		for (float CrossoverPercent : Settings.CrossoverPoints)
		{
			//calculate the amount of genes to read before the next crossover
			int NodeChromosomeDeltaCrossoverPoint = (int)std::ceil(CrossoverPercent * MinNodeChromosomeLength) - LastDeltaNodeCrossover;
			int ConnectionChromosomeDeltaCrossoverPoint = (int)std::ceil(CrossoverPercent * MinConnectionChromosomeLength) - LastDeltaConnectionCrossover;

			//store them
			NodeChromosomeCrossoverPointGeneCount.push_back(NodeChromosomeDeltaCrossoverPoint);
			ConnectionChromosomeCrossoverPointGeneCount.push_back(ConnectionChromosomeDeltaCrossoverPoint);

			//and store them as the last used variables
			LastDeltaNodeCrossover = NodeChromosomeDeltaCrossoverPoint;
			LastDeltaConnectionCrossover = ConnectionChromosomeDeltaCrossoverPoint;
		}

		if (Verbose)
		{
			std::cout << "Node chromosome delta crossover points: ";
			for (int i : NodeChromosomeCrossoverPointGeneCount)
				std::cout << i << ", ";
			std::cout << std::endl;
			std::cout << "Connection chromosome delta crossover points: ";
			for (int i : ConnectionChromosomeCrossoverPointGeneCount)
				std::cout << i << ", ";
			std::cout << std::endl;
		}

		//create the input and output nodes
		for (int i = 0; i < inputs; i++)
		{
			//create a node based on the blank gene
			InputNodes.push_back(Node(0.0f));
			if (Verbose)
			{
				std::cout << "Input Node _ Created: " << i << std::endl;
			}
		}
		for (int i = 0; i < outputs; i++)
		{
			//create a node based on the blank gene
			OutputNodes.push_back(Node(0.0f));
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

		//construct the nodes in the network
		int GenomeProgress = 0;
		bool CurrentGenome = false;
		std::vector<BR_RETURN_INT_TYPE*> *CurrentNodeGeneSet = &NodeGeneSetA;
		//iterate through all of the crossover points
		for (int GeneCount : NodeChromosomeCrossoverPointGeneCount)
		{
			//construct the nodes for the amount of genes specified
			for (int Gene_i = GeneCount; Gene_i < GenomeProgress + GeneCount; Gene_i++)
			{
				//construct and append the node to the network
				Nodes.insert(std::pair(UniqueNodeIndex++, Node(CurrentNodeGeneSet->operator[](Gene_i))));
			}

			//invert the current gene set
			if (CurrentGenome)
				CurrentNodeGeneSet = &NodeGeneSetA;
			else
				CurrentNodeGeneSet = &NodeGeneSetB;
			CurrentGenome = !CurrentGenome;

			//start parsing the other genome from the same place
			GenomeProgress += GeneCount;
		}

		//construct the connections in the network
		GenomeProgress = 0;
		CurrentGenome = false;
		std::vector<BR_RETURN_INT_TYPE*> *CurrentConnectionGeneSet = &ConnectionGeneSetA;
		//iterate through all of the crossover points
		for (int GeneCount : ConnectionChromosomeCrossoverPointGeneCount)
		{
			//construct the connections for the amount of genes specified
			for (int Gene_i = GeneCount; Gene_i < GenomeProgress + GeneCount; Gene_i++)
			{
				//get the current gene
				BR_RETURN_INT_TYPE* CurrentGene = CurrentConnectionGeneSet->operator[](Gene_i);
				//create the connection
				Connection::CreateConnection(CurrentGene, this);
			}

			//invert the current gene set
			if (CurrentGenome)
				CurrentConnectionGeneSet = &ConnectionGeneSetA;
			else
				CurrentConnectionGeneSet = &ConnectionGeneSetB;
			CurrentGenome = !CurrentGenome;

			//start parsing the other genome from the same place
			GenomeProgress += GeneCount;
		}

		//delete all of the genes
		for (auto Gene : NodeGeneSetA)
			delete Gene;
		for (auto Gene : NodeGeneSetB)
			delete Gene;
		for (auto Gene : ConnectionGeneSetA)
			delete Gene;
		for (auto Gene : ConnectionGeneSetB)
			delete Gene;
	}
	catch (std::exception &ex)
	{
		*ErrCode = UNKNOWN_FAILURE;
		std::stringstream ErrorMessage;
		ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
		std::cout << ErrorMessage.str() << std::endl << ex.what() << std::endl;
		return;
	}
}

//returns the values of all of the output nodes
std::vector<float> Network::GetResults(unsigned int* ErrCode)
{
	try 
	{
		//default the error code to success
		*ErrCode = SUCCESS;

		//the vector for the results to be stored in
		std::vector<float> Results;

		//iterate through the output nodes
		unsigned int Err = SUCCESS;
		for (auto NodeIter : OutputNodes)
		{
			Results.push_back(NodeIter.CalculateValue(this, &Err));
			if (Err != SUCCESS)
				*ErrCode = Err;
		}

		//return the results
		return Results;
	}
	catch (std::exception &ex)
	{
		*ErrCode = UNKNOWN_FAILURE;
		std::stringstream ErrorMessage;
		ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
		std::cout << ErrorMessage.str() << std::endl << ex.what() << std::endl;
		return;
	}
}

//sets the values of all of the input nodes
void Network::SetInputs(std::vector<float>& Inputs, unsigned int* ErrCode)
{
	try {
		//default the error code to success
		*ErrCode = SUCCESS;

		//make sure that our float vector and node vector have the same size
		if (Inputs.size() != InputNodes.size())
		{
			//invalid inputs
			std::stringstream ErrorMessage;
			ErrorMessage << "Invalid Network inputs at" << __FILE__ << ":" << __LINE__ << ". Make inputs are valid";
			*ErrCode = NETWORK_INVALID_INPUTS;
			throw std::invalid_argument(ErrorMessage.str().c_str());
		}

		//iterate through all of the nodes
		for (auto NodeIter = Nodes.begin(); NodeIter != Nodes.end(); std::advance(NodeIter, 1))
		{
			//require it to recalculate
			NodeIter->second.NeedsToRecalc = true;
		}
		//iterate through all of the output nodes
		for (int i = 0; i < OutputNodes.size(); i++)
		{
			OutputNodes[i].NeedsToRecalc = true;
		}

		//set the values of the input nodes
		for (int i = 0; i < InputNodes.size(); i++)
		{
			InputNodes[i].value = Inputs[i];
		}
	}
	catch (std::exception &ex)
	{
		*ErrCode = UNKNOWN_FAILURE;
		std::stringstream ErrorMessage;
		ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
		std::cout << ErrorMessage.str() << std::endl << ex.what() << std::endl;
		return;
	}
}

//saves the network to a file on disk
void Network::SaveNetwork(std::string GenomePath, unsigned int* ErrCode, bool verbose)
{
	try 
	{
		//default the error code to success
		*ErrCode = SUCCESS;

		//create the directory for the genome
		std::filesystem::create_directory(std::filesystem::path(GenomePath));

		//open the files at the chromosome paths
		std::ofstream NodeChromosomeFile(GenomePath + "/Nodes.chr", std::ios::out | std::ios::trunc | std::ios::binary);
		std::ofstream ConnectionChromosomeFile(GenomePath + "/Connections.chr", std::ios::out | std::ios::trunc | std::ios::binary);

		//make sure we can open both files
		if (!NodeChromosomeFile.good() | !ConnectionChromosomeFile.good())
		{
			//we couldn't open one of them, throw an exception
			std::stringstream ErrorMessage;
			ErrorMessage << "Unable to save to Genome at " << __FILE__ << ":" << __LINE__ << ". Make sure Genome location is valid";
			*ErrCode = NETWORK_INVALID_SAVE_PATH;
			throw std::invalid_argument(ErrorMessage.str().c_str());
		}

		//iterate through all of the internal nodes in the network to save them and their connections
		long long int InternalNodeIdentifier = 0;
		for (auto NodeIter : Nodes)
		{
			//iterate through all of the connections for this node
			for (std::vector<Connection>::iterator ConnectionIter = NodeIter.second.Connections.begin();
				ConnectionIter != NodeIter.second.Connections.end();
				ConnectionIter++)
			{
				//save this connection in the file
				ConnectionIter->AppendConnectionToChromosome(ConnectionChromosomeFile, InternalNodeIdentifier++, this);
			}
			//save this node in the file
			NodeIter.second.AppendNodeToChromosome(NodeChromosomeFile);
		}

		//iterate through all of the output nodes in the network to save ONLY their connections
		long long int OutputNodeIdentifier = -1;
		for (auto NodeIter : OutputNodes)
		{
			//iterate through all of the connections for this node
			for (std::vector<Connection>::iterator ConnectionIter = NodeIter.Connections.begin();
				ConnectionIter != NodeIter.Connections.end();
				ConnectionIter++)
			{
				//save this connection in the file
				ConnectionIter->AppendConnectionToChromosome(ConnectionChromosomeFile, OutputNodeIdentifier--, this);
			}
		}

		//close the filestreams
		NodeChromosomeFile.close();
		ConnectionChromosomeFile.close();
	}
	catch (std::exception &ex)
	{
		*ErrCode = UNKNOWN_FAILURE;
		std::stringstream ErrorMessage;
		ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
		std::cout << ErrorMessage.str() << std::endl << ex.what() << std::endl;
		return;
	}
}

//the number of inputs, outputs and nodes in the network
int Network::InputCount() 
{
	return InputNodes.size();
}
int Network::OutputCount() 
{
	return OutputNodes.size();
}
int Network::NodeCount() 
{
	return Nodes.size();
}


//adds a node between a connection to the network
bool Network::AddNodeBetweenConnection(int TargetNodeIndex, int ConnectionIndex, float bias, unsigned int* ErrCode)
{
	try
	{
		//default the error code to success
		*ErrCode = SUCCESS;

		//find the target node
		long long int TargetNodeIdentifier = 0;
		if (TargetNodeIndex < NodeCount() & TargetNodeIndex >= 0)
		{
			//internal node
			auto NodePlace = Nodes.begin();
			std::advance(NodePlace, TargetNodeIndex);
			TargetNodeIdentifier = NodePlace->first;
		}
		else if (TargetNodeIndex < NodeCount() + OutputCount() & TargetNodeIndex >= 0)
		{
			//output node
			TargetNodeIdentifier = -OutputCount() - (TargetNodeIndex - NodeCount());
		}
		else
		{
			//invalid index
			std::stringstream ErrorMessage;
			ErrorMessage << "Invalid node index at" << __FILE__ << ":" << __LINE__ << ". Make node index >= 0 and < NodeCount + OutputCount";
			*ErrCode = NETWORK_INVALID_NODE_INDEX;
			throw std::invalid_argument(ErrorMessage.str().c_str());
		}

		//determine if the target is an output node or an internal node
		if (TargetNodeIdentifier < 0)
		{
			//we're dealing with an output node
			TargetNodeIdentifier = -TargetNodeIdentifier - 1;

			//ensure that the connection index is valid
			if (ConnectionIndex >= OutputNodes[TargetNodeIdentifier].Connections.size() | ConnectionIndex < 0)
			{
				//invalid index
				std::stringstream ErrorMessage;
				ErrorMessage << "Invalid Connection index at" << __FILE__ << ":" << __LINE__ << ". Make connection index >= 0 and < TotalNodeConnections";
				*ErrCode = NETWORK_INVALID_CONN_INDEX;
				throw std::invalid_argument(ErrorMessage.str().c_str());
			}

			//create the new node
			Node NewNode = Node(bias);

			//add a connection to the node identical to the old connection
			NewNode.Connections.push_back(Connection(OutputNodes[TargetNodeIdentifier].Connections[ConnectionIndex]));

			//set the weight of the old connection to 1
			OutputNodes[TargetNodeIdentifier].Connections[ConnectionIndex].Weight = 1;

			//add the new node to the node map
			Nodes.insert(std::pair(UniqueNodeIndex++, NewNode));

			//change the old connection's source
			OutputNodes[TargetNodeIdentifier].Connections[ConnectionIndex].SourceNode = UniqueNodeIndex - 1;
		}
		else
		{
			//we're dealing with an internal node

			//ensure that the connection index is valid
			if (ConnectionIndex >= Nodes[TargetNodeIdentifier].Connections.size() | ConnectionIndex < 0)
			{
				//invalid index
				std::stringstream ErrorMessage;
				ErrorMessage << "Invalid Connection index at" << __FILE__ << ":" << __LINE__ << ". Make connection index >= 0 and < TotalNodeConnections";
				*ErrCode = NETWORK_INVALID_CONN_INDEX;
				throw std::invalid_argument(ErrorMessage.str().c_str());
			}

			//create the new node
			Node NewNode = Node(bias);

			//add a connection to the node identical to the old connection
			NewNode.Connections.push_back(Connection(Nodes[TargetNodeIdentifier].Connections[ConnectionIndex]));

			//set the weight of the old connection to 1
			Nodes[TargetNodeIdentifier].Connections[ConnectionIndex].Weight = 1;

			//add the new node to the node map
			Nodes.insert(std::pair(UniqueNodeIndex++, NewNode));

			//change the old connection's source
			Nodes[TargetNodeIdentifier].Connections[ConnectionIndex].SourceNode = UniqueNodeIndex - 1;
		}
	}
	catch (std::exception &ex)
	{
		*ErrCode = UNKNOWN_FAILURE;
		std::stringstream ErrorMessage;
		ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
		std::cout << ErrorMessage.str() << std::endl << ex.what() << std::endl;
		return false;
	}

	return true;
}

//adds a connection between nodes to the network
bool Network::AddConnectionBetweenNodes(int SourceNodeIndex, int TargetNodeIndex, float weight, unsigned int* ErrCode)
{
	try
	{
		//default the error code to success
		*ErrCode = SUCCESS;

		//find the target node
		long long int TargetNodeIdentifier = 0;
		if (TargetNodeIndex < NodeCount() & TargetNodeIndex >= 0)
		{
			//internal node
			auto NodePlace = Nodes.begin();
			std::advance(NodePlace, TargetNodeIndex);
			TargetNodeIdentifier = NodePlace->first;
		}
		else if (TargetNodeIndex < NodeCount() + OutputCount() & TargetNodeIndex >= 0)
		{
			//output node
			TargetNodeIdentifier = -OutputCount() - (TargetNodeIndex - NodeCount());
		}
		else
		{
			//invalid index
			std::stringstream ErrorMessage;
			ErrorMessage << "Invalid node index at" << __FILE__ << ":" << __LINE__ << ". Make node index >= 0 and < NodeCount + OutputCount";
			*ErrCode = NETWORK_INVALID_NODE_INDEX;
			throw std::invalid_argument(ErrorMessage.str().c_str());
		}

		//find the source node
		long long int SourceNodeIdentifier = 0;
		if (SourceNodeIndex < InputCount() & SourceNodeIndex >= 0)
		{
			//input node
			SourceNodeIdentifier = -SourceNodeIndex - 1;
		}
		else if (SourceNodeIndex < InputCount() + NodeCount() & SourceNodeIndex >= 0)
		{
			//internal node
			auto NodePlace = Nodes.begin();
			std::advance(NodePlace, SourceNodeIndex - InputCount());
			SourceNodeIdentifier = NodePlace->first;
		}
		else
		{
			//invalid index
			std::stringstream ErrorMessage;
			ErrorMessage << "Invalid node index at" << __FILE__ << ":" << __LINE__ << ". Make node index >= 0 and < NodeCount + OutputCount";
			*ErrCode = NETWORK_INVALID_NODE_INDEX;
			throw std::invalid_argument(ErrorMessage.str().c_str());
		}

		//insert the connection
		if (TargetNodeIdentifier < 0)
		{
			//output node as target
			OutputNodes[-TargetNodeIdentifier - 1].Connections.push_back(Connection(SourceNodeIdentifier, weight));
		}
		else
		{
			//internal node as target
			Nodes[TargetNodeIdentifier].Connections.push_back(Connection(SourceNodeIdentifier, weight));
		}
	}
	catch (std::exception &ex)
	{
		*ErrCode = UNKNOWN_FAILURE;
		std::stringstream ErrorMessage;
		ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
		std::cout << ErrorMessage.str() << std::endl << ex.what() << std::endl;
		return false;
	}
	
	return true;
}

//removes a node from the network
bool Network::RemoveNode(int NodeIndex, unsigned int* ErrCode)
{
	try
	{
		//default the error code to success
		*ErrCode = SUCCESS;

		//ensure that the nodeindex is valid
		if (NodeIndex >= NodeCount() | NodeIndex < 0)
		{
			//invalid index
			std::stringstream ErrorMessage;
			ErrorMessage << "Invalid node index at" << __FILE__ << ":" << __LINE__ << ". Make node index >= 0 and < NodeCount + OutputCount";
			*ErrCode = NETWORK_INVALID_NODE_INDEX;
			throw std::invalid_argument(ErrorMessage.str().c_str());
		}

		//get the start of the nodes map, move by NodeIndex places, then remove that node
		auto NodePlace = Nodes.begin();
		std::advance(NodePlace, NodeIndex);
		Nodes.erase(NodePlace);
	}
	catch (std::exception &ex)
	{
		*ErrCode = UNKNOWN_FAILURE;
		std::stringstream ErrorMessage;
		ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
		std::cout << ErrorMessage.str() << std::endl << ex.what() << std::endl;
		return false;
	}

	return true;
}

//removes a connection from the network
bool Network::RemoveConnection(int NodeIndex, int ConnectionIndex, unsigned int* ErrCode)
{
	try
	{
		//default the error code to success
		*ErrCode = SUCCESS;

		//ensure that the nodeindex is valid
		if (NodeIndex >= NodeCount() + OutputCount() | NodeIndex < 0)
		{
			//invalid index
			std::stringstream ErrorMessage;
			ErrorMessage << "Invalid node index at" << __FILE__ << ":" << __LINE__ << ". Make node index >= 0 and < NodeCount + OutputCount";
			*ErrCode = NETWORK_INVALID_NODE_INDEX;
			throw std::invalid_argument(ErrorMessage.str().c_str());
		}

		//what kind of node are we dealing with
		if (NodeIndex < NodeCount() & NodeIndex >= 0)
		{
			//internal node
			//get the start of the nodes map and move by NodeIndex places
			auto NodePlace = Nodes.begin();
			std::advance(NodePlace, NodeIndex);

			//ensure that the connectionindex is valid
			if (ConnectionIndex >= NodePlace->second.Connections.size() | ConnectionIndex < 0)
			{
				//invalid index
				std::stringstream ErrorMessage;
				ErrorMessage << "Invalid Connection index at" << __FILE__ << ":" << __LINE__ << ". Make connection index >= 0 and < TotalNodeConnections";
				*ErrCode = NETWORK_INVALID_CONN_INDEX;
				throw std::invalid_argument(ErrorMessage.str().c_str());
			}

			//find and remove the connection from the vector
			NodePlace->second.Connections.erase(NodePlace->second.Connections.begin() + ConnectionIndex);
		}
		else if (NodeIndex < NodeCount() + OutputCount() & NodeIndex >= 0)
		{
			//output node
			//get the output node
			auto NodePlace = OutputNodes.begin() + (NodeIndex - NodeCount());

			//ensure that the connectionindex is valid
			if (ConnectionIndex >= NodePlace->Connections.size() | ConnectionIndex < 0)
			{
				//invalid index
				std::stringstream ErrorMessage;
				ErrorMessage << "Invalid Connection index at" << __FILE__ << ":" << __LINE__ << ". Make connection index >= 0 and < TotalNodeConnections";
				*ErrCode = NETWORK_INVALID_CONN_INDEX;
				throw std::invalid_argument(ErrorMessage.str().c_str());
			}

			//find and remove the connection from the vector
			NodePlace->Connections.erase(NodePlace->Connections.begin() + ConnectionIndex);
		}
		else 
		{
			//invalid index
			std::stringstream ErrorMessage;
			ErrorMessage << "Invalid node index at" << __FILE__ << ":" << __LINE__ << ". Make node index >= 0 and < NodeCount + OutputCount";
			*ErrCode = NETWORK_INVALID_NODE_INDEX;
			throw std::invalid_argument(ErrorMessage.str().c_str());
		}
	}
	catch (std::exception &ex)
	{
		*ErrCode = UNKNOWN_FAILURE;
		std::stringstream ErrorMessage;
		ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
		std::cout << ErrorMessage.str() << std::endl << ex.what() << std::endl;
		return false;
	}

	return true;
}

//gets the bias of a node
float Network::GetNodeBias(int NodeIndex, unsigned int* ErrCode)
{
	try
	{
		//default the error code to success
		*ErrCode = SUCCESS;

		//ensure the index is within the number of nodes
		if (NodeIndex >= NodeCount() | NodeIndex < 0) 
		{
			//invalid index
			std::stringstream ErrorMessage;
			ErrorMessage << "Invalid node index at" << __FILE__ << ":" << __LINE__ << ". Make node index >= 0 and < NodeCount + OutputCount";
			*ErrCode = NETWORK_INVALID_NODE_INDEX;
			throw std::invalid_argument(ErrorMessage.str().c_str());
		}

		//get the start of the nodes map and move by NodeIndex places
		auto NodePlace = Nodes.begin();
		std::advance(NodePlace, NodeIndex);

		//get and return the bias of the node
		return NodePlace->second.Bias;
	}
	catch (std::exception &ex)
	{
		*ErrCode = UNKNOWN_FAILURE;
		std::stringstream ErrorMessage;
		ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
		std::cout << ErrorMessage.str() << std::endl << ex.what() << std::endl;
		return 0;
	}

	return true;
}

//sets the bias of a node
void Network::SetNodeBias(int NodeIndex, float bias, unsigned int* ErrCode)
{
	try
	{
		//default the error code to success
		*ErrCode = SUCCESS;

		//ensure the index is within the number of nodes
		if (NodeIndex >= NodeCount() | NodeIndex < 0)
		{
			//invalid index
			std::stringstream ErrorMessage;
			ErrorMessage << "Invalid node index at" << __FILE__ << ":" << __LINE__ << ". Make node index >= 0 and < NodeCount + OutputCount";
			*ErrCode = NETWORK_INVALID_NODE_INDEX;
			throw std::invalid_argument(ErrorMessage.str().c_str());
		}

		//get the start of the nodes map and move by NodeIndex places
		auto NodePlace = Nodes.begin();
		std::advance(NodePlace, NodeIndex);

		//set the bias of the node
		NodePlace->second.Bias = bias;
	}
	catch (std::exception &ex)
	{
		*ErrCode = UNKNOWN_FAILURE;
		std::stringstream ErrorMessage;
		ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
		std::cout << ErrorMessage.str() << std::endl << ex.what() << std::endl;
		return;
	}
}

//get the total number of connections going into a node
int Network::GetTotalNodeConnections(int TargetNodeIndex, unsigned int* ErrCode)
{
	try
	{
		//default the error code to success
		*ErrCode = SUCCESS;

		//ensure the index is within the number of nodes
		if (TargetNodeIndex >= NodeCount() + OutputCount() | TargetNodeIndex < 0)
		{
			//invalid index
			std::stringstream ErrorMessage;
			ErrorMessage << "Invalid node index at" << __FILE__ << ":" << __LINE__ << ". Make node index >= 0 and < NodeCount + OutputCount";
			*ErrCode = NETWORK_INVALID_NODE_INDEX;
			throw std::invalid_argument(ErrorMessage.str().c_str());
		}

		//what kind of node are we dealing with
		if (TargetNodeIndex < NodeCount() & TargetNodeIndex >= 0)
		{
			//internal node
			//get the start of the nodes map and move by NodeIndex places
			auto NodePlace = Nodes.begin();
			std::advance(NodePlace, TargetNodeIndex);

			//get and return the bias of the node
			return NodePlace->second.Connections.size();
		}
		else if (TargetNodeIndex < NodeCount() + OutputCount() & TargetNodeIndex >= 0)
		{
			//output node
			//get the output node
			auto NodePlace = OutputNodes.begin() + (TargetNodeIndex - NodeCount());

			//return the connections count
			return NodePlace->Connections.size();
		}
		else 
		{
			//invalid index
			std::stringstream ErrorMessage;
			ErrorMessage << "Invalid node index at" << __FILE__ << ":" << __LINE__ << ". Make node index >= 0 and < NodeCount + OutputCount";
			*ErrCode = NETWORK_INVALID_NODE_INDEX;
			throw std::invalid_argument(ErrorMessage.str().c_str());
		}
	}
	catch (std::exception &ex)
	{
		*ErrCode = UNKNOWN_FAILURE;
		std::stringstream ErrorMessage;
		ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
		std::cout << ErrorMessage.str() << std::endl << ex.what() << std::endl;
		return -1;
	}
}

//gets the weight of a connection
float Network::GetConnectionWeight(int TargetNodeIndex, int ConnectionIndex, unsigned int* ErrCode)
{
	try
	{
		//default the error code to success
		*ErrCode = SUCCESS;

		//ensure the index is within the number of nodes
		if (TargetNodeIndex >= NodeCount() + OutputCount() | TargetNodeIndex < 0)
		{
			//invalid index
			std::stringstream ErrorMessage;
			ErrorMessage << "Invalid node index at" << __FILE__ << ":" << __LINE__ << ". Make node index >= 0 and < NodeCount + OutputCount";
			*ErrCode = NETWORK_INVALID_NODE_INDEX;
			throw std::invalid_argument(ErrorMessage.str().c_str());
		}

		//what type of target node are we looking at?
		if(TargetNodeIndex < NodeCount() & TargetNodeIndex >= 0)
		{
			//internal node
			//get the start of the nodes map and move by NodeIndex places
			auto NodePlace = Nodes.begin();
			std::advance(NodePlace, TargetNodeIndex);

			//ensure that the connectionindex is valid
			if (ConnectionIndex >= NodePlace->second.Connections.size() | ConnectionIndex < 0)
			{
				//invalid index
				std::stringstream ErrorMessage;
				ErrorMessage << "Invalid Connection index at" << __FILE__ << ":" << __LINE__ << ". Make connection index >= 0 and < TotalNodeConnections";
				*ErrCode = NETWORK_INVALID_CONN_INDEX;
				throw std::invalid_argument(ErrorMessage.str().c_str());
			}

			//get the start of the connections vector and move by connectionindex places
			auto ConnectionPlace = NodePlace->second.Connections.begin();
			std::advance(ConnectionPlace, ConnectionIndex);

			//get and return the weight of the connection
			return ConnectionPlace->Weight;
		}
		else if (TargetNodeIndex < NodeCount() + OutputCount() & TargetNodeIndex >= 0)
		{
			//output node
			//get the start of the nodes vector and move by NodeIndex places
			auto NodePlace = OutputNodes.begin();
			std::advance(NodePlace, TargetNodeIndex);

			//ensure that the connectionindex is valid
			if (ConnectionIndex >= NodePlace->Connections.size() | ConnectionIndex < 0)
			{
				//invalid index
				std::stringstream ErrorMessage;
				ErrorMessage << "Invalid Connection index at" << __FILE__ << ":" << __LINE__ << ". Make connection index >= 0 and < TotalNodeConnections";
				*ErrCode = NETWORK_INVALID_CONN_INDEX;
				throw std::invalid_argument(ErrorMessage.str().c_str());
			}

			//get the start of the connections vector and move by connectionindex places
			auto ConnectionPlace = NodePlace->Connections.begin();
			std::advance(ConnectionPlace, ConnectionIndex);

			//get and return the weight of the connection
			return ConnectionPlace->Weight;
		}
		else 
		{
			//invalid index
			std::stringstream ErrorMessage;
			ErrorMessage << "Invalid node index at" << __FILE__ << ":" << __LINE__ << ". Make node index >= 0 and < NodeCount + OutputCount";
			*ErrCode = NETWORK_INVALID_NODE_INDEX;
			throw std::invalid_argument(ErrorMessage.str().c_str());
		}
	}
	catch (std::exception &ex)
	{
		*ErrCode = UNKNOWN_FAILURE;
		std::stringstream ErrorMessage;
		ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
		std::cout << ErrorMessage.str() << std::endl << ex.what() << std::endl;
		return -1;
	}
}

//sets the weight of a connection
bool Network::SetConnectionWeight(int TargetNodeIndex, int ConnectionIndex, float weight, unsigned int* ErrCode)
{
	try
	{
		//default the error code to success
		*ErrCode = SUCCESS;

		//ensure the index is within the number of nodes we can look at
		if (TargetNodeIndex >= NodeCount() + OutputCount())
			return false; //it isn't. return -1

		//what type of target node are we looking at?
		if (TargetNodeIndex < NodeCount() & TargetNodeIndex >= 0)
		{
			//internal node
			//get the start of the nodes map and move by NodeIndex places
			auto NodePlace = Nodes.begin();
			std::advance(NodePlace, TargetNodeIndex);

			//ensure that the connectionindex is valid
			if (ConnectionIndex >= NodePlace->second.Connections.size() | ConnectionIndex < 0)
			{
				//invalid index
				std::stringstream ErrorMessage;
				ErrorMessage << "Invalid Connection index at" << __FILE__ << ":" << __LINE__ << ". Make connection index >= 0 and < TotalNodeConnections";
				*ErrCode = NETWORK_INVALID_CONN_INDEX;
				throw std::invalid_argument(ErrorMessage.str().c_str());
			}

			//get the start of the connections vector and move by connectionindex places
			auto ConnectionPlace = NodePlace->second.Connections.begin();
			std::advance(ConnectionPlace, ConnectionIndex);

			//set the weight of the connection
			ConnectionPlace->Weight = weight;
		}
		else if (TargetNodeIndex < NodeCount() + OutputCount() & TargetNodeIndex >= 0)
		{
			//output node
			//get the start of the nodes vector and move by NodeIndex places
			auto NodePlace = OutputNodes.begin();
			std::advance(NodePlace, TargetNodeIndex);

			//ensure that the connectionindex is valid
			if (ConnectionIndex >= NodePlace->Connections.size() | ConnectionIndex < 0)
			{
				//invalid index
				std::stringstream ErrorMessage;
				ErrorMessage << "Invalid Connection index at" << __FILE__ << ":" << __LINE__ << ". Make connection index >= 0 and < TotalNodeConnections";
				*ErrCode = NETWORK_INVALID_CONN_INDEX;
				throw std::invalid_argument(ErrorMessage.str().c_str());
			}

			//get the start of the connections vector and move by connectionindex places
			auto ConnectionPlace = NodePlace->Connections.begin();
			std::advance(ConnectionPlace, ConnectionIndex);

			//get and return the weight of the connection
			ConnectionPlace->Weight = weight;
		}
		else
		{
			//invalid index
			std::stringstream ErrorMessage;
			ErrorMessage << "Invalid node index at" << __FILE__ << ":" << __LINE__ << ". Make node index >= 0 and < NodeCount + OutputCount";
			*ErrCode = NETWORK_INVALID_NODE_INDEX;
			throw std::invalid_argument(ErrorMessage.str().c_str());
		}
	}
	catch (std::exception &ex)
	{
		*ErrCode = UNKNOWN_FAILURE;
		std::stringstream ErrorMessage;
		ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
		std::cout << ErrorMessage.str() << std::endl << ex.what() << std::endl;
		return false;
	}

	//successful operation
	return true;
}