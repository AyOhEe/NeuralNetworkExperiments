// dllmain.cpp : Defines the entry point for the DLL application.
#include "../Include/framework.h"

#include <map>
#include <algorithm>

#include "../../NeuralNetwork/Include/Network.h"
#include "../../NeuralNetwork/Include/Node.h"

#ifdef NEURALNETDLL_EXPORTS
#define NEURALNET_API __declspec(dllexport)
#else
#define NEURALNET_API __declspec(dllimport)
#endif

#define VALIDATE_HANDLE_BOOL \
/*ensure the handle is valid*/ \
std::map<unsigned int, Network*>::iterator handleLocation = __NETWORKS.find(handle); \
if (handleLocation == __NETWORKS.end()) \
{ \
	/*invalid handle, indicate failure*/ \
	return FALSE; \
} \

#define VALIDATE_HANDLE_VOID \
/*ensure the handle is valid*/ \
std::map<unsigned int, Network*>::iterator handleLocation = __NETWORKS.find(handle); \
if (handleLocation == __NETWORKS.end()) \
{ \
	/*invalid handle, indicate failure*/ \
	return; \
} \

#define VALIDATE_HANDLE_INT \
/*ensure the handle is valid*/ \
std::map<unsigned int, Network*>::iterator handleLocation = __NETWORKS.find(handle); \
if (handleLocation == __NETWORKS.end()) \
{ \
	/*invalid handle, indicate failure*/ \
	return -1; \
} \

//all networks alive
std::map<unsigned int, Network*> __NETWORKS = std::map<unsigned int, Network*>();

extern "C"
{
	//creates a network and returns a handle
	unsigned int NEURALNET_API CREATE_NETWORK(const char* GenomePath, int Inputs, int Outputs, int ActivationFunctionIndex, bool Verbose)
	{
		//create the new network
		Network* Net = new Network(std::string(GenomePath), Inputs, Outputs, ActivationFunctionIndex, Verbose);

		//insert it into the vector
		__NETWORKS[(unsigned int)Net] = Net;

		//return the "handle" which is really just the memory address of the network(guaranteed to be unique)
		return (unsigned int)Net;
	}

	//removes a network if it exists
	BOOL NEURALNET_API DESTROY_NETWORK(unsigned int handle)
	{
		VALIDATE_HANDLE_BOOL

		//valid handle, delete the network and remove it from the vector
		delete handleLocation->second;
		__NETWORKS.erase(handleLocation);

		//indicate success
		return TRUE;
	}

	//sets the inputs of a network
	BOOL NEURALNET_API SET_NETWORK_INPUTS(unsigned int handle, float* inputs, int n_inputs)
	{
		VALIDATE_HANDLE_BOOL

		//validate n_inputs
		if (n_inputs != handleLocation->second->InputCount())
		{
			return FALSE;
		}

		//convert the inputs into a vector
		std::vector<float> Inputs;
		std::copy(inputs, inputs + n_inputs, std::back_inserter(Inputs));

		//set the inputs
		handleLocation->second->SetInputs(Inputs);
		return TRUE;
	}

	//calculates the values of a network
	BOOL NEURALNET_API GET_NETWORK_OUTPUTS(unsigned int handle, float* outputs, int n_outputs)
	{
		VALIDATE_HANDLE_BOOL

		//validate n_outputs
		if (n_outputs != handleLocation->second->OutputCount())
		{
			return FALSE;
		}

		//copy over the network outputs to the output array
		std::vector<float> Outputs = handleLocation->second->GetResults();
		std::copy(Outputs.begin(), Outputs.end(), outputs);

		return TRUE;
	}

	//TODO: make like half of these functions give error codes and give them some damn try-catch statements
	//returns the number of inputs, outputs or nodes in a network
	int NEURALNET_API GET_NETWORK_INPUT_COUNT(unsigned int handle)
	{
		VALIDATE_HANDLE_BOOL

		return handleLocation->second->InputCount();
	}
	int NEURALNET_API GET_NETWORK_NODE_COUNT(unsigned int handle)
	{
		VALIDATE_HANDLE_BOOL

		return handleLocation->second->NodeCount();
	}
	int NEURALNET_API GET_NETWORK_OUTPUT_COUNT(unsigned int handle)
	{
		VALIDATE_HANDLE_BOOL

		return handleLocation->second->OutputCount();
	}

	//adds a node between a connection to a network
	BOOL NEURALNET_API ADD_NODE_BETWEEN_CONNECTION(unsigned int handle, int TargetNodeIndex, int ConnectionIndex, float bias) 
	{
		VALIDATE_HANDLE_BOOL

		handleLocation->second->AddNodeBetweenConnection(TargetNodeIndex, ConnectionIndex, bias);
		return TRUE;
	}
	//adds a connection between nodes to a network
	BOOL NEURALNET_API ADD_CONNECTION_BETWEEN_NODES(unsigned int handle, int SourceNodeIndex, int TargetNodeIndex, float weight) 
	{
		VALIDATE_HANDLE_BOOL
			
		handleLocation->second->AddConnectionBetweenNodes(SourceNodeIndex, TargetNodeIndex, weight);
		return TRUE;
	}

	//removes a node from the network
	BOOL NEURALNET_API REMOVE_NODE(unsigned int handle, int NodeIndex) 
	{
		VALIDATE_HANDLE_BOOL

		handleLocation->second->RemoveNode(NodeIndex);
		return TRUE;
	}
	//removes a connection from the network
	BOOL NEURALNET_API REMOVE_CONNECTION(unsigned int handle, int NodeIndex, int ConnectionIndex) 
	{
		VALIDATE_HANDLE_BOOL

		handleLocation->second->RemoveConnection(NodeIndex, ConnectionIndex);
		return TRUE;
	}

	//gets the bias of a node
	FLOAT NEURALNET_API GET_NODE_BIAS(unsigned int handle, int NodeIndex) 
	{
		VALIDATE_HANDLE_BOOL

		return handleLocation->second->GetNodeBias(NodeIndex);
	}
	//sets the bias of a node
	BOOL NEURALNET_API SET_NODE_BIAS(unsigned int handle, int NodeIndex, float bias)
	{
		VALIDATE_HANDLE_BOOL

		handleLocation->second->SetNodeBias(NodeIndex, bias);
		return TRUE;
	}

	//gets the total number of connections going into a connection
	INT NEURALNET_API GET_NODE_CONNECTION_COUNT(unsigned int handle, int TargetNodeIndex) 
	{
		VALIDATE_HANDLE_INT

		return handleLocation->second->GetTotalNodeConnections(TargetNodeIndex);
	}

	//gets the weight of a connection
	FLOAT NEURALNET_API GET_CONNECTION_WEIGHT(unsigned int handle, int TargetNodeIndex, int ConnectionIndex) 
	{
		VALIDATE_HANDLE_BOOL

		return handleLocation->second->GetConnectionWeight(TargetNodeIndex, ConnectionIndex);
	}
	//sets the weight of a connection
	BOOL NEURALNET_API SET_CONNECTION_WEIGHT(unsigned int handle, int TargetNodeIndex, int ConnectionIndex, float weight)
	{
		VALIDATE_HANDLE_BOOL

		handleLocation->second->SetConnectionWeight(TargetNodeIndex, ConnectionIndex, weight);
		return TRUE;
	}

	//saves a network
	BOOL NEURALNET_API SAVE_NETWORK(unsigned int handle, const char* path) 
	{
		VALIDATE_HANDLE_BOOL

		handleLocation->second->SaveNetwork(std::string(path));
		return true;
	}
}

//TODO: throw some definitions for these functions in a header and split this file into multiple files(maybe?)
//TODO: write bindings for most of these in python