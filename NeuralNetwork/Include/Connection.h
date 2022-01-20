#ifndef CONNECTION_H
#define CONNECTION_H

#include "BinaryReader.h"
#include "Network.h"

class Connection
{
	//the entry in the nodes map for this connection's source
	unsigned long long int SourceNode;

	//the weight of this noce
	float Weight;

public:

	//constructs a connection from a gene
	Connection(BR_RETURN_INT_TYPE* Gene, Network &Net);
};

#endif