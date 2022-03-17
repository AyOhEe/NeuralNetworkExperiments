#ifndef CONNECTION_H
#define CONNECTION_H

#include <exception>
#include <iterator>

#include "SpikingNetwork.h"

//forward definitions
class SpikingNetwork;

class Connection
{
	float Weight; //the weight of the connection
	unsigned int SourceNeuron; //the id of the source neuron in the neuron map
	bool SourceNeuronType; //the type of the source neuron(false == input, true == internal)

public:

	//creates a connection in Network from bytes
	Connection(unsigned char* bytes, SpikingNetwork* Net);

	//creates a connection
	static void CreateConnection(unsigned char* bytes, SpikingNetwork* Net);

	//attempts to add the value of this connection to OutVal. returns false if the source doesn't exist
	bool TryAddValue(SpikingNetwork* Network, float* OutVal);

	//writes the connection to a file
	void WriteConnectionToFile(unsigned int TargetIndex, unsigned int TargetType, SpikingNetwork* Net, std::ofstream &File);
};

#endif