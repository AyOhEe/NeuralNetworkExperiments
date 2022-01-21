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
	//the entry in the nodes map for this connection's source
	unsigned long long int SourceNode;

	//the weight of this noce
	float Weight;

public:

	//constructs a connection from a gene
	Connection(BR_RETURN_INT_TYPE* Gene, Network *Net);

	//creates a connection in network Net according to Gene
	static void CreateConnection(BR_RETURN_INT_TYPE* Gene, Network *Net);

	//returns Gene as a string
	static std::string GeneAsString(BR_RETURN_INT_TYPE* Gene);
};

#endif