#include "../Include/Network.h"

//creates a network based on the genome at GenomePath
Network::Network(std::string GenomePath, int inputs, int outputs, int ActivationFunctionIndex, bool Verbose) :
	Network(GenomePath, inputs, outputs, GetActivationFunctionPointer(ActivationFunctionIndex), Verbose) {}

//creates a network based on the genome at GenomePath
Network::Network(std::string GenomePath, int inputs, int outputs, float(*ActivationFunction)(float), bool Verbose)
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
		for (std::map<unsigned long long, Node>::iterator nodeiter = Nodes.begin(); nodeiter != Nodes.end(); nodeiter++, i++)
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
bool Network::AddNodeBetweenConnection(int TargetNodeIndex, int ConnectionIndex, float bias) 
{
	//find the connection to insert the node in between
}

//adds a connection between nodes to the network
bool Network::AddConnectionBetweenNodes(int SourceNodeIndex, int TargetNodeIndex, float weight) 
{
	//find the target node
	Node* TargetNode;
	if (TargetNodeIndex < NodeCount()) 
	{
		//internal node
		auto NodePlace = Nodes.begin();
		std::advance(NodePlace, SourceNodeIndex);
		TargetNode = &(NodePlace->second);
	}
	else 
	{
		//output node
		auto NodePlace = OutputNodes.begin();
		std::advance(NodePlace, SourceNodeIndex - NodeCount());
		TargetNode = &(*NodePlace);
	}

	//find the source node
	Node* SourceNode;
	if (SourceNodeIndex < InputCount())
	{
		//input node
		auto NodePlace = InputNodes.begin();
		std::advance(NodePlace, SourceNodeIndex);
		SourceNode = &(*NodePlace);
	}
	else
	{
		//internal node
		auto NodePlace = Nodes.begin();
		std::advance(NodePlace, SourceNodeIndex - InputCount());
		SourceNode = &(NodePlace->second);
	}

	//insert the connection
	//TODO(aria): FIX THIS SHIT
}

//removes a node from the network
bool Network::RemoveNode(int NodeIndex) 
{
	try 
	{
		//get the start of the nodes map, move by NodeIndex places, then remove that node
		auto NodePlace = Nodes.begin();
		std::advance(NodePlace, NodeIndex);
		Nodes.erase(NodePlace);
	}
	catch (std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
		return false;
	}
}

//removes a connection from the network
bool Network::RemoveConnection(int NodeIndex, int ConnectionIndex) 
{
	try
	{
		//get the start of the nodes map and move by NodeIndex places
		auto NodePlace = Nodes.begin();
		std::advance(NodePlace, NodeIndex);

		//find and remove the connection from the vector
		NodePlace->second.Connections.erase(NodePlace->second.Connections.begin() + ConnectionIndex);
	}
	catch (std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
		return false;
	}
}

//gets the bias of a node
float Network::GetNodeBias(int NodeIndex)
{
	//ensure the index is within the number of nodes
	if (NodeIndex > NodeCount())
		return 0; //it isn't. return 0

	try
	{
		//get the start of the nodes map and move by NodeIndex places
		auto NodePlace = Nodes.begin();
		std::advance(NodePlace, NodeIndex);

		//get and return the bias of the node
		return NodePlace->second.Bias;
	}
	catch (std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
		return 0;
	}
}

//sets the bias of a node
void Network::SetNodeBias(int NodeIndex, float bias) 
{
	//ensure the index is within the number of nodes
	if (NodeIndex > NodeCount())
		return; //it isn't. return 0

	try
	{
		//get the start of the nodes map and move by NodeIndex places
		auto NodePlace = Nodes.begin();
		std::advance(NodePlace, NodeIndex);

		//set the bias of the node
		NodePlace->second.Bias = bias;
	}
	catch (std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
		return;
	}
}


//get the total number of connections going into a node
int Network::GetTotalNodeConnections(int TargetNodeIndex)
{
	//ensure the index is within the number of nodes
	if (TargetNodeIndex > NodeCount())
		return -1; //it isn't. return -1

	try
	{
		//get the start of the nodes map and move by NodeIndex places
		auto NodePlace = Nodes.begin();
		std::advance(NodePlace, TargetNodeIndex);

		//get and return the bias of the node
		return NodePlace->second.Connections.size();
	}
	catch (std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
		return -1;
	}
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