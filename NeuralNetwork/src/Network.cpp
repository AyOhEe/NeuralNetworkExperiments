#include "../Include/Network.h"

//creates a network based on the genome at GenomePath
Network::Network(std::string GenomePath, int inputs, int outputs, int ActivationFunctionIndex, bool Verbose)
	: Network(GenomePath, inputs, outputs, GetActivationFunctionPointer(ActivationFunctionIndex), Verbose) 
{
	//this can actually have an empty body, we're done here
}

//creates a network based on the genome at GenomePath
Network::Network(std::string GenomePath, int inputs, int outputs, float(*ActivationFunction)(float), bool Verbose) : Nodes(), InputNodes(), OutputNodes()
{
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
		throw std::invalid_argument(ErrorMessage.str().c_str());
	}

	//create the input and output nodes
	for (int i = 0; i < inputs; i++)
	{
		//create a node based on the blank gene
		InputNodes.push_back(new Node(0));
		if (Verbose)
		{
			std::cout << "Input Node _ Created: " << i << std::endl;
		}
	}
	for (int i = 0; i < outputs; i++)
	{
		//create a node based on the blank gene
		OutputNodes.push_back(new Node(0));
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
		NodeGene nodeGene = NodeGene(Gene);
		Nodes.push_back(new Node(nodeGene));

		if (Verbose)
		{
			std::cout << "Node created as " << nodeGene.ToString() << std::endl;
		}

		//destroy the gene now that it's been read
		delete[] Gene;
	}
	//close the chromosome now that we're done
	NodeChromosome.close();

	//start reading in the connection chromosome
	while (!ConnectionChromosome.eof()) 
	{
		//get the next gene
		Gene = ConnectionChromosome.ReadGene();

		//if we've hit the end of the file, ignore this gene and stop reading
		if (ConnectionChromosome.eof())
			break;

		//does this node target an output or a hidden node?
		ConnectionGene connectionGene = ConnectionGene(Gene);
		if (connectionGene.TargetType) 
		{
			//output node, add this connection to the respective hidden node
			OutputNodes[connectionGene.TargetID % OutputNodes.size()]->Connections.push_back(Connection(connectionGene, this));
		}
		else 
		{
			//hidden node, add this connection to the respective hidden node
			Nodes[connectionGene.TargetID % Nodes.size()]->Connections.push_back(Connection(connectionGene, this));
		}

		if (Verbose) 
		{
			std::cout << "Connection created as " << connectionGene.ToString() << std::endl;
		}

		//destroy the gene now that it's been read
		delete[] Gene;
	}
	//close the chromosome now that we're done
	ConnectionChromosome.close();

	if (Verbose) 
	{
		int i = 0;
		for (std::vector<Node*>::iterator nodeiter = Nodes.begin(); nodeiter != Nodes.end(); nodeiter++, i++) 
		{
			std::cout << "Node " << i << " has " << (*nodeiter)->Connections.size() << " connections" << std::endl;
		}
		i = 0;
		for (std::vector<Node*>::iterator nodeiter = OutputNodes.begin(); nodeiter != OutputNodes.end(); nodeiter++, i++)
		{
			std::cout << "Output Node " << i << " has " << (*nodeiter)->Connections.size() << " connections" << std::endl;
		}
	}
}

//creates a network based on two genomes
Network::Network(std::string GenomePathA, std::string GenomePathB, BreedSettings Settings, int inputs, int outputs, int ActivationFunctionIndex, bool Verbose)
{
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
	if (!NodeChromosomeA | !ConnectionChromosomeA | !NodeChromosomeB | !ConnectionChromosomeB)
	{
		//we couldn't open one of them, throw an exception
		std::stringstream ErrorMessage;
		ErrorMessage << "Unable to open Chromosomes at " << __FILE__ << ":" << __LINE__ << ". Make sure Genomes are valid";
		throw std::invalid_argument(ErrorMessage.str().c_str());
	}

	//create the vectors for each chromosome's genes
	std::vector<NodeGene> NodeGeneSetA;
	std::vector<NodeGene> NodeGeneSetB;
	std::vector<ConnectionGene> ConnectionGeneSetA;
	std::vector<ConnectionGene> ConnectionGeneSetB;

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

		NodeGeneSetA.push_back(NodeGene(Gene));
		delete Gene;
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

		NodeGeneSetB.push_back(NodeGene(Gene));
		delete Gene;
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

		ConnectionGeneSetA.push_back(ConnectionGene(Gene));
		delete Gene;
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

		ConnectionGeneSetB.push_back(ConnectionGene(Gene));
		delete Gene;
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
		InputNodes.push_back(new Node(0));
		if (Verbose)
		{
			std::cout << "Input Node _ Created: " << i << std::endl;
		}
	}
	for (int i = 0; i < outputs; i++)
	{
		//create a node based on the blank gene
		OutputNodes.push_back(new Node(0));
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
	std::vector<NodeGene> *CurrentNodeGeneSet = &NodeGeneSetA;
	//iterate through all of the crossover points
	for (int GeneCount : NodeChromosomeCrossoverPointGeneCount) 
	{
		//construct the nodes for the amount of genes specified
		for (int Gene_i = GeneCount; Gene_i < GenomeProgress + GeneCount; Gene_i++)
		{
			//construct and append the node to the network
			Nodes.push_back(new Node(CurrentNodeGeneSet->operator[](Gene_i)));
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
	std::vector<ConnectionGene> *CurrentConnectionGeneSet = &ConnectionGeneSetA;
	//iterate through all of the crossover points
	for (int GeneCount : ConnectionChromosomeCrossoverPointGeneCount) 
	{
		//construct the connections for the amount of genes specified
		for (int Gene_i = GeneCount; Gene_i < GenomeProgress + GeneCount; Gene_i++) 
		{
			//get the current gene
			ConnectionGene CurrentGene = CurrentConnectionGeneSet->operator[](Gene_i);
			//determine what kind of node this connection should be added to
			if (CurrentGene.TargetType) 
			{
				//output node
				OutputNodes[CurrentGene.TargetID % OutputNodes.size()]->Connections.push_back(Connection(CurrentGene, this));
			}
			else 
			{
				//hidden node
				Nodes[CurrentGene.TargetID % OutputNodes.size()]->Connections.push_back(Connection(CurrentGene, this));
			}
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
		std::cout << "Invalid number of inputs to network: \n\t Expected " << InputNodes.size() << " but got " << Inputs.size() << std::endl;
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

//saves the network to a file on disk
void Network::SaveNetwork(std::string GenomePath, bool verbose)
{
	//create the directory for the genome
	std::filesystem::create_directory(std::filesystem::path(GenomePath));

	//open the files at the chromosome paths
	std::ofstream NodeChromosomeFile(GenomePath + "/Nodes.chr", std::ios::out | std::ios::trunc | std::ios::binary);
	std::ofstream ConnectionChromosomeFile(GenomePath + "/Connections.chr", std::ios::out | std::ios::trunc | std::ios::binary);

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
			ConnectionIter->AsGene(this, false, NodeIter - Nodes.begin()).AppendGene(ConnectionChromosomeFile, verbose);
		}
		//save this node in the file
		(*NodeIter)->AsGene().AppendGene(NodeChromosomeFile, verbose);
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
			ConnectionIter->AsGene(this, true, NodeIter - OutputNodes.begin()).AppendGene(ConnectionChromosomeFile, verbose);
		}
	}

	//close the filestreams
	NodeChromosomeFile.close();
	ConnectionChromosomeFile.close();
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
	int IntWeight = (int)roundl(abs(Weight * CONNECTION_GENE_WEIGHT_DIVISOR));

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
	int IntBias = (int)roundl(abs(Bias) * NODE_GENE_BIAS_DIVISOR);

	//store the gene's component parts in the byte array
	Gene[0] += 0b10000000; // this is a node gene
	Gene[0] += Bias < 0 ? 0b01000000 : 0; //sign bit
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

//adds a node between a connectionto the network
bool Network::AddNodeBetweenConnection(int TargetNodeIndex, int ConnectionIndex, float bias) 
{
	//ensure the target node index is valid
	if (TargetNodeIndex > NodeCount() + OutputCount()) 
	{
		//invalid, exit out
		return false;
	}

	//get the source node
	Node* Target;
	if (TargetNodeIndex >= NodeCount()) 
	{
		Target = OutputNodes[TargetNodeIndex - NodeCount()];
	}
	else 
	{
		Target = Nodes[TargetNodeIndex];
	}

	//ensure the connection index is valid
	if (ConnectionIndex > Target->Connections.size()) 
	{
		//invalid, exit out
		return false;
	}

	//get the connection
	Connection* OldConnection = &(Target->Connections[ConnectionIndex % Target->Connections.size()]);

	//create the new node and add it to the nodes vector
	Node* NewNode = new Node(bias);
	Nodes.push_back(NewNode);

	//create a new connection with the source of the old connection
	NewNode->Connections.push_back(Connection(1, OldConnection->Source));
	//set the source of the old connection to the new node
	OldConnection->Source = NewNode;

	//we completed the operation okay, return true
	return true;
}
//adds a connection between nodes to the network
bool Network::AddConnectionBetweenNodes(int SourceNodeIndex, int TargetNodeIndex, float weight) 
{
	//ensure the source node index is valid
	if (SourceNodeIndex > InputCount() + NodeCount())
	{
		//invalid, exit out
		return false;
	}
	//ensure the target node index is valid
	if (TargetNodeIndex > NodeCount() + OutputCount())
	{
		//invalid, exit out
		return false;
	}

	//get the source node
	Node* Source;
	if (SourceNodeIndex >= InputCount())
	{
		Source = Nodes[SourceNodeIndex - InputCount()];
	}
	else
	{
		Source = InputNodes[SourceNodeIndex];
	}
	//get the target node
	Node* Target;
	if (TargetNodeIndex >= NodeCount())
	{
		Target = OutputNodes[TargetNodeIndex - NodeCount()];
	}
	else
	{
		Target = Nodes[TargetNodeIndex];
	}

	//create the connection and add it to the target node
	Target->Connections.push_back(Connection(weight, Source));

	//we completed the operation okay, return true
	return true;
}

//gets the bias of a node
float Network::GetNodeBias(int NodeIndex) 
{
	//ensure the node index is valid
	if (NodeIndex > NodeCount())
	{
		//invalid, exit out
		return 0;
	}

	//get the node and return it's bias
	return Nodes[NodeIndex]->Bias;
}
//sets the bias of a node
void Network::SetNodeBias(int NodeIndex, float bias) 
{
	//ensure the node index is valid
	if (NodeIndex > NodeCount())
	{
		//invalid, exit out
		return;
	}

	//get the node and set it's bias
	Nodes[NodeIndex]->Bias = bias;
}
//get the total number of connections into a node
int Network::GetTotalNodeConnections(int TargetNodeIndex) 
{
	//ensure the target node index is valid
	if (TargetNodeIndex > NodeCount() + OutputCount())
	{
		//invalid, exit out
		std::cout << "Invalid TargetNodeIndex on line " << __LINE__ << std::endl;
		return -1;
	}

	//get the target node
	Node* Target;
	if (TargetNodeIndex >= NodeCount())
	{
		Target = OutputNodes[TargetNodeIndex - NodeCount()];
	}
	else
	{
		Target = Nodes[TargetNodeIndex];
	}

	//return it's number of connections
	return Target->Connections.size();
}
//gets the weight of a connection
float Network::GetConnectionWeight(int TargetNodeIndex, int ConnectionIndex) 
{
	//ensure the target node index is valid
	if (TargetNodeIndex > NodeCount() + OutputCount())
	{
		//invalid, exit out
		return 0;
	}

	//get the target node
	Node* Target;
	if (TargetNodeIndex >= NodeCount())
	{
		Target = OutputNodes[TargetNodeIndex - NodeCount()];
	}
	else
	{
		Target = Nodes[TargetNodeIndex];
	}

	//ensure the connection index is valid
	if (ConnectionIndex > Target->Connections.size())
	{
		//invalid, exit out
		return 0;
	}

	//get and the connection's weight
	return Target->Connections[ConnectionIndex % Target->Connections.size()].Weight;
}
//sets the weight of a connection
void Network::SetConnectionWeight(int TargetNodeIndex, int ConnectionIndex, float weight) 
{
	//ensure the target node index is valid
	if (TargetNodeIndex > NodeCount() + OutputCount())
	{
		//invalid, exit out
		return;
	}

	//get the target node
	Node* Target;
	if (TargetNodeIndex >= NodeCount())
	{
		Target = OutputNodes[TargetNodeIndex - NodeCount()];
	}
	else
	{
		Target = Nodes[TargetNodeIndex];
	}

	//ensure the connection index is valid
	if (ConnectionIndex > Target->Connections.size())
	{
		//invalid, exit out
		return;
	}

	//get and the connection's weight
	Target->Connections[ConnectionIndex % Target->Connections.size()].Weight = weight;
}

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
float (*GetActivationFunctionPointer(int Index))(float)
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

//removes a node from the network
bool Network::RemoveNode(int NodeIndex) 
{
	//validate the node index
	if (NodeIndex > Nodes.size()) 
	{
		//invalid index, return false
		return false;
	}

	//get and remove the node from the network
	delete Nodes[NodeIndex];
	Nodes.erase(Nodes.begin() + NodeIndex);

	//return true once the node has been removed
	return true;
}

//removes a connection from the network
bool Network::RemoveConnection(int NodeIndex, int ConnectionIndex) 
{
	//validate the node index
	if (NodeIndex > Nodes.size())
	{
		//invalid index, return false
		return false;
	}
	//get the node
	Node* Target = Nodes[NodeIndex];

	//validate the connection index
	if (ConnectionIndex > Target->Connections.size()) 
	{
		//invalid index, return false
		return false;
	}
	//get and remove the connection
	Target->Connections.erase(Target->Connections.begin() + ConnectionIndex);

	//return true once the connection has been removed
	return true;
}

//creates a connection gene from a chromosome gene
ConnectionGene::ConnectionGene(BR_RETURN_INT_TYPE* Gene)
{
	SourceType = Gene[1] == 1;
	TargetType = Gene[2] == 1;
	SourceID = Gene[3];
	TargetID = Gene[4];
	Weight = (Gene[5] == 1 ? -1 : 1) * (Gene[6] / CONNECTION_GENE_WEIGHT_DIVISOR);
}
//default constructor
ConnectionGene::ConnectionGene() {}

//creates a node gene from a chromosome gene
NodeGene::NodeGene(BR_RETURN_INT_TYPE* Gene)
{
	Bias = (Gene[1] == 1 ? -1 : 1) * (Gene[2] / NODE_GENE_BIAS_DIVISOR);
}
//default constructor
NodeGene::NodeGene() {}