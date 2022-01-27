#include "../Include/Connection.h"

//constructs a connection from a gene
Connection::Connection(BR_RETURN_INT_TYPE* Gene, Network *Net) 
{
	//calculate the weight of the connection
	Weight = (Gene[5] == 1 ? -1 : 1) * (Gene[6] / CONNECTION_GENE_WEIGHT_DIVISOR);

	//determine the source node type
	if (Gene[1] == 1) 
	{
		//only proceed if the network has nodes
		if (Net->Nodes.size() == 0)
			return;

		//internal node, get the node at the position specified in the gene
		auto NodePlace = Net->Nodes.begin();
		std::advance(NodePlace, Gene[3] % Net->Nodes.size());
		SourceNode = NodePlace->first;
	}
	else 
	{
		//input node
		SourceNode = -(long long)Gene[3] - 1;
	}
}
//constructs a gene from a node identifier and weight
Connection::Connection(long long int NodeIdentifier, float weight) 
{
	//store the node identifier and weight
	SourceNode = NodeIdentifier;
	Weight = weight;
}

//creates a connection in network Net according to Gene
void Connection::CreateConnection(BR_RETURN_INT_TYPE* Gene, Network *Net) 
{
	//determine the target type
	if (Gene[2] == 1) 
	{
		//only proceed if the network has nodes
		if (Net->OutputNodes.size() == 0)
			return;

		//output node, get the node at the position specified in the gene
		auto NodePlace = Net->OutputNodes.begin();
		std::advance(NodePlace, Gene[4] % Net->OutputNodes.size());

		//create the new connection
		NodePlace->Connections.push_back(Connection(Gene, Net));
	}
	else 
	{
		//only proceed if the network has nodes
		if (Net->Nodes.size() == 0)
			return;

		//internal node, get the node at the position specified in the gene
		auto NodePlace = Net->Nodes.begin();
		std::advance(NodePlace, Gene[4] % Net->Nodes.size());

		//create the new connection
		NodePlace->second.Connections.push_back(Connection(Gene, Net));
	}
}

//attempts to get the value of this connection
bool Connection::TryAddValue(float* OutValue, Network* Net, unsigned int* ErrCode) 
{
	try 
	{
		*ErrCode = SUCCESS; //default to success if not told otherwise

		//is the source an input or internal node?
		unsigned int NodeErr = SUCCESS;
		if (SourceNode < 0)
		{
			//input node
			//add the weighted value of the connection
			*OutValue += Weight * Net->InputNodes[-SourceNode - 1].CalculateValue(Net, &NodeErr);
		}
		else
		{
			//internal node
			//add the weighted value of the connection
			*OutValue += Weight * Net->Nodes[SourceNode].CalculateValue(Net, &NodeErr);
		}

		//we succeeded, return true
		return true;
	}
	catch(std::exception &ex)
	{
		//did we throw an error already?
		if (*ErrCode != SUCCESS)
			throw ex; //yes, just throw the error again

		//oops, something went wrong. return false
		std::cout << ex.what() << std::endl;
		*ErrCode = CONNECTION_MISSING_SOURCE; //it's literally the only thing that can go wrong here
		return false;
	}

	//we shouldn't reach here, but i'm supposed to put this here anyway
	return false;
}

//appends this connection to a node chromosome
void Connection::AppendConnectionToChromosome(std::ofstream &FileStream, long long int TargetNode, Network* Net) 
{
	//calculate the old weight as an integer
	int IntWeight = abs(Weight * CONNECTION_GENE_WEIGHT_DIVISOR);

	//determine node types
	bool SourceType = SourceNode < 0;
	bool TargetType = TargetNode < 0;

	//get the target index
	long long int TargetIndex = TargetType ? -TargetNode - 1 : TargetNode;

	//get the source index
	long long int SourceIndex;
	if (SourceType) 
	{
		//input node, convert the identifier to an input index
		SourceIndex = -SourceNode - 1;
	}
	else 
	{
		//internal node
		//only do the next part if the network has nodes
		if (Net->Nodes.size() != 0)
			SourceIndex = std::distance(Net->Nodes.begin(), Net->Nodes.find(SourceNode));
		else //otherwise, set source index to 0
			SourceIndex = 0;
	}

	//recreate the gene
	char* Gene = new char[4];
	Gene[0] = 0b10000000 + (SourceType ? 0 : 0b01000000) + (TargetType ? 0b00100000 : 0);
	Gene[0] += (SourceIndex & 0b1111100000) >> 5;
	Gene[1] += ((SourceIndex & 0b0000011111) << 3) + ((TargetIndex & 0b1110000000) >> 7);
	Gene[2] += ((TargetIndex & 0b0001111111) << 1) + (Weight < 0 ? 0b1 : 0);
	Gene[3] += IntWeight;

	//write the gene
	FileStream << Gene[0] << Gene[1] << Gene[2] << Gene[3];
}

//returns Gene as a string
std::string Connection::GeneAsString(BR_RETURN_INT_TYPE* Gene) 
{
	std::stringstream StringStream;
	StringStream << 
		"Connection Gene: " << 
		Gene[1] << ", " << 
		Gene[2] << ", " << 
		Gene[3] << ", " << 
		Gene[4] << ", " <<
		Gene[5] << ", " <<
		Gene[6] << ", " <<
		(Gene[5] == 1 ? -1 : 1) * (Gene[6] / CONNECTION_GENE_WEIGHT_DIVISOR);
	return StringStream.str();
}