#ifndef CONNECTION_H
#define CONNECTION_H

#include <exception>
#include <iterator>

#include "SpikingNetwork.h"

//forward definitions
class SpikingNetwork;
class Lobe;

struct ConnectionParams
{
	float Weight; //the weight of the connection
	unsigned int SourceNeuronIndex; //the index of the source neuron
	bool SourceNeuronType; //the type of the source neuron
};

class Connection
{
	//networks need special access to connections
	friend SpikingNetwork;
	//as do lobes
	friend Lobe;

	float Weight; //the weight of the connection
	unsigned int SourceNeuron; //the id of the source neuron in the neuron map
	bool SourceNeuronType; //the type of the source neuron(false == input, true == internal)

public:

	//sets the parameters of the connection
	void SetParams(ConnectionParams Params, SpikingNetwork* Net, int* ErrCode, bool verbose = false);
	//gets the parameters of the connection
	ConnectionParams GetParams(SpikingNetwork* Net, int* ErrCode, bool verbose = false);

	//creates a connection in Network from bytes
	Connection(unsigned char* bytes, SpikingNetwork* Net);
	//creates a connection with source id ID and type Type and weight Weight
	Connection(unsigned int ID, bool Type, float NewWeight);

	//creates a connection
	static void CreateConnection(unsigned char* bytes, SpikingNetwork* Net);

	//attempts to add the value of this connection to OutVal. returns false if the source doesn't exist
	bool TryAddValue(SpikingNetwork* Network, float* OutVal);

	//writes the connection to a file
	void WriteConnectionToFile(unsigned int TargetIndex, unsigned int TargetType, SpikingNetwork* Net, std::ofstream &File);

};

#endif