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

//returns the values of all of the output nodes
std::vector<float> Network::GetResults() 
{
	//the vector for the results to be stored in
	std::vector<float> Results;

	//iterate through the output nodes
	for(auto NodeIter : OutputNodes)
	{
		Results.push_back(NodeIter.value);
	}

	//return the results
	return Results;
}

//sets the values of all of the input nodes
void Network::SetInputs(std::vector<float>& Inputs) 
{
	//iterate through all of the nodes
	for (auto NodeIter = Nodes.begin(); NodeIter != Nodes.begin(); std::advance(NodeIter, 1))
	{
		//require it to recalculate
		NodeIter->second.NeedsToRecalc = true;
	}
	//iterate through all of the output nodes
	for(auto OutputNodeIter : OutputNodes)
	{
		OutputNodeIter.NeedsToRecalc = true;
	}

	//set the values of the input nodes
	for(int i = 0; i < InputNodes.size(); i++)
	{
		InputNodes[i].value = Inputs[i];
	}
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
	//find the target node
	long long int TargetNodeIdentifier = 0;
	if (TargetNodeIndex >= NodeCount()) 
	{
		//output node
		TargetNodeIdentifier = -OutputCount() - (TargetNodeIndex - NodeCount());
	}
	else 
	{
		//internal node
		auto NodePlace = Nodes.begin();
		std::advance(NodePlace, TargetNodeIndex);
		TargetNodeIdentifier = NodePlace->first;
	}

	//determine if the target is an output node or an internal node
	if(TargetNodeIdentifier < 0)
	{
		//we're dealing with an output node
		TargetNodeIdentifier = -TargetNodeIdentifier - 1;

		//ensure that the connection index is valid
		if (ConnectionIndex >= OutputNodes[TargetNodeIdentifier].Connections.size())
			return false;

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
		if (ConnectionIndex >= Nodes[TargetNodeIdentifier].Connections.size())
			return false;

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

	return true;
}

//adds a connection between nodes to the network
bool Network::AddConnectionBetweenNodes(int SourceNodeIndex, int TargetNodeIndex, float weight) 
{
	//find the target node
	long long int TargetNodeIdentifier = 0;
	if (TargetNodeIndex >= NodeCount())
	{
		//output node
		TargetNodeIdentifier = -OutputCount() - (TargetNodeIndex - NodeCount());
	}
	else
	{
		//internal node
		auto NodePlace = Nodes.begin();
		std::advance(NodePlace, TargetNodeIndex);
		TargetNodeIdentifier = NodePlace->first;
	}

	//find the source node
	long long int SourceNodeIdentifier = 0;
	if (SourceNodeIndex >= InputCount())
	{
		//internal node
		auto NodePlace = Nodes.begin();
		std::advance(NodePlace, SourceNodeIndex - InputCount());
		SourceNodeIdentifier = NodePlace->first;
	}
	else
	{
		//input node
		SourceNodeIdentifier = -SourceNodeIndex - 1;
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
	
	return true;
}

//removes a node from the network
bool Network::RemoveNode(int NodeIndex) 
{
	//ensure that the nodeindex is valid
	if (NodeIndex >= NodeCount())
		return false;

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

	return true;
}

//removes a connection from the network
bool Network::RemoveConnection(int NodeIndex, int ConnectionIndex) 
{
	//ensure that the nodeindex is valid
	if (NodeIndex >= NodeCount() + OutputCount())
		return false;

	try
	{
		//what kind of node are we dealing with
		if (NodeIndex < NodeCount())
		{
			//internal node
			//get the start of the nodes map and move by NodeIndex places
			auto NodePlace = Nodes.begin();
			std::advance(NodePlace, NodeIndex);

			//ensure that the connectionindex is valid
			if (ConnectionIndex >= NodePlace->second.Connections.size())
				return false;

			//find and remove the connection from the vector
			NodePlace->second.Connections.erase(NodePlace->second.Connections.begin() + ConnectionIndex);
		}
		else
		{
			//output node
			//get the output node
			auto NodePlace = OutputNodes.begin() + (NodeIndex - NodeCount());

			//ensure that the connectionindex is valid
			if (ConnectionIndex >= NodePlace->Connections.size())
				return false;

			//find and remove the connection from the vector
			NodePlace->Connections.erase(NodePlace->Connections.begin() + ConnectionIndex);
		}
	}
	catch (std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
		return false;
	}

	return true;
}

//gets the bias of a node
float Network::GetNodeBias(int NodeIndex)
{
	//ensure the index is within the number of nodes
	if (NodeIndex >= NodeCount())
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

	return true;
}

//sets the bias of a node
void Network::SetNodeBias(int NodeIndex, float bias) 
{
	//ensure the index is within the number of nodes
	if (NodeIndex >= NodeCount())
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
	if (TargetNodeIndex >= NodeCount() + OutputCount())
		return -1; //it isn't. return -1

	try
	{
		//what kind of node are we dealing with
		if (TargetNodeIndex < NodeCount())
		{
			//internal node
			//get the start of the nodes map and move by NodeIndex places
			auto NodePlace = Nodes.begin();
			std::advance(NodePlace, TargetNodeIndex);

			//get and return the bias of the node
			return NodePlace->second.Connections.size();
		}
		else 
		{
			//internal node
			//get the output node
			auto NodePlace = OutputNodes.begin() + (TargetNodeIndex - NodeCount());

			//return the connections count
			return NodePlace->Connections.size();
		}
	}
	catch (std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
		return -1;
	}
}

//gets the weight of a connection
float Network::GetConnectionWeight(int TargetNodeIndex, int ConnectionIndex) 
{
	//ensure the index is within the number of nodes we can look at
	if (TargetNodeIndex >= NodeCount() + OutputCount())
		return -1; //it isn't. return -1

	try
	{
		//what type of target node are we looking at?
		if(TargetNodeIndex >= NodeCount())
		{
			//output node
			//get the start of the nodes vector and move by NodeIndex places
			auto NodePlace = OutputNodes.begin();
			std::advance(NodePlace, TargetNodeIndex);

			//ensure that the connectionindex is valid
			if (ConnectionIndex >= NodePlace->Connections.size())
				return false;

			//get the start of the connections vector and move by connectionindex places
			auto ConnectionPlace = NodePlace->Connections.begin();
			std::advance(ConnectionPlace, ConnectionIndex);

			//get and return the weight of the connection
			return ConnectionPlace->Weight;
		}
		else 
		{
			//internal node
			//get the start of the nodes map and move by NodeIndex places
			auto NodePlace = Nodes.begin();
			std::advance(NodePlace, TargetNodeIndex);

			//ensure that the connectionindex is valid
			if (ConnectionIndex >= NodePlace->second.Connections.size())
				return false;

			//get the start of the connections vector and move by connectionindex places
			auto ConnectionPlace = NodePlace->second.Connections.begin();
			std::advance(ConnectionPlace, ConnectionIndex);

			//get and return the weight of the connection
			return ConnectionPlace->Weight;
		}
	}
	catch (std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
		return -1;
	}
}

//sets the weight of a connection
bool Network::SetConnectionWeight(int TargetNodeIndex, int ConnectionIndex, float weight) 
{
	//ensure the index is within the number of nodes we can look at
	if (TargetNodeIndex >= NodeCount() + OutputCount())
		return false; //it isn't. return -1

	try
	{
		//what type of target node are we looking at?
		if (TargetNodeIndex >= NodeCount())
		{
			//output node
			//get the start of the nodes vector and move by NodeIndex places
			auto NodePlace = OutputNodes.begin();
			std::advance(NodePlace, TargetNodeIndex);

			//ensure that the connectionindex is valid
			if (ConnectionIndex >= NodePlace->Connections.size())
				return false;

			//get the start of the connections vector and move by connectionindex places
			auto ConnectionPlace = NodePlace->Connections.begin();
			std::advance(ConnectionPlace, ConnectionIndex);

			//get and return the weight of the connection
			ConnectionPlace->Weight = weight;
		}
		else
		{
			//internal node
			//get the start of the nodes map and move by NodeIndex places
			auto NodePlace = Nodes.begin();
			std::advance(NodePlace, TargetNodeIndex);

			//ensure that the connectionindex is valid
			if (ConnectionIndex >= NodePlace->second.Connections.size())
				return false;

			//get the start of the connections vector and move by connectionindex places
			auto ConnectionPlace = NodePlace->second.Connections.begin();
			std::advance(ConnectionPlace, ConnectionIndex);

			//set the weight of the connection
			ConnectionPlace->Weight = weight;
		}
	}
	catch (std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
		return false;
	}

	//successful operation
	return true;
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