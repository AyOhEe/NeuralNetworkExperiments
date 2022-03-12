#ifndef CONNECTION_H
#define CONNECTION_H

#include <exception>
#include <iterator>

#include "Network.h"

//forward definitions
class SpikingNetwork;

class Connection
{
	float Weight; //the weight of the connection
	unsigned int SourceNeuron; //the id of the source neuron in the neuron map

public:

	//creates a connection in Network from bytes
	Connection(unsigned char* bytes, SpikingNetwork* Net);

	//creates a connection
	static void CreateConnection(unsigned char* bytes, SpikingNetwork* Net);

	//attempts to add the value of this connection to OutVal. returns false if the source doesn't exist
	bool TryAddValue(SpikingNetwork* Network, float* OutVal);
};

#endif