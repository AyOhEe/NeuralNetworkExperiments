#ifndef CONNECTION_H
#define CONNECTION_H

#include "BinaryReader.h"
#include "Network.h"

//forward definitions
class Network;
class Node;
class Connection;

class Connection
{
	//networks need access to connections
	friend Network;
	
	//the node for this connection's source.
	//if < 0 then input index, therwise internal hash
	long long int SourceNode;

	//the weight of this noce
	float Weight;

public:

	//constructs a connection from a gene
	Connection(BR_RETURN_INT_TYPE* Gene, Network *Net);
	//constructs a gene from a node identifier and weight
	Connection(long long int NodeIdentifier, float weight);

	//creates a connection in network Net according to Gene
	static void CreateConnection(BR_RETURN_INT_TYPE* Gene, Network *Net);

	//appends this connection to a node chromosome
	void AppendConnectionToChromosome(std::ofstream FileSteam);

	//returns Gene as a string
	static std::string GeneAsString(BR_RETURN_INT_TYPE* Gene);
};

#endif