// dllmain.cpp : Defines the entry point for the DLL application.
#include "../Include/framework.h"

#include <map>
#include <algorithm>

#include "../../NeuralNetwork/Include/Network.h"
#include "../../NeuralNetwork/Include/Node.h"

#if defined(__NT__) | defined(_WIN32)
    #ifdef NEURALNETDLL_EXPORTS
		#define NEURALNET_API __declspec(dllexport)
    #else
		#define NEURALNET_API __declspec(dllimport)
    #endif
#elif __unix__
    #define NEURALNET_API 
#endif

#define VALIDATE_HANDLE_BOOL \
/*ensure the handle is valid*/ \
std::map<unsigned long long int, Network*>::iterator handleLocation = __NETWORKS.find(handle); \
if (handleLocation == __NETWORKS.end()) \
{ \
	/*invalid handle, indicate failure*/ \
	return false; \
} \

#define VALIDATE_HANDLE_VOID \
/*ensure the handle is valid*/ \
std::map<unsigned long long int, Network*>::iterator handleLocation = __NETWORKS.find(handle); \
if (handleLocation == __NETWORKS.end()) \
{ \
	/*invalid handle, indicate failure*/ \
	return; \
} \

#define VALIDATE_HANDLE_INT \
/*ensure the handle is valid*/ \
std::map<unsigned long long int, Network*>::iterator handleLocation = __NETWORKS.find(handle); \
if (handleLocation == __NETWORKS.end()) \
{ \
	/*invalid handle, indicate failure*/ \
	return -1; \
} \

//all networks alive
std::map<unsigned long long int, Network*> __NETWORKS = std::map<unsigned long long int, Network*>();

extern "C"
{
	//creates a network and returns a handle
	unsigned long long int NEURALNET_API CREATE_NETWORK(const char* GenomePath, int Inputs, int Outputs, int ActivationFunctionIndex, bool Verbose)
	{
		//create the new network
		Network* Net = new Network(std::string(GenomePath), Inputs, Outputs, ActivationFunctionIndex, Verbose);

		//insert it into the vector
		__NETWORKS[(unsigned long long int)Net] = Net;

		//return the "handle" which is really just the memory address of the network(guaranteed to be unique)
		return (unsigned long long int)Net;
	}

	//removes a network if it exists
    bool NEURALNET_API DESTROY_NETWORK(unsigned long long int handle)
	{
		VALIDATE_HANDLE_BOOL

		//valid handle, delete the network and remove it from the vector
		delete handleLocation->second;
		__NETWORKS.erase(handleLocation);

		//indicate success
		return true;
	}

	//sets the inputs of a network
	bool NEURALNET_API SET_NETWORK_INPUTS(unsigned long long int handle, float* inputs, int n_inputs)
	{
		VALIDATE_HANDLE_BOOL

		//validate n_inputs
		if (n_inputs != handleLocation->second->InputCount())
		{
			return false;
		}

		//convert the inputs into a vector
		std::vector<float> Inputs;
		std::copy(inputs, inputs + n_inputs, std::back_inserter(Inputs));

		//set the inputs
		handleLocation->second->SetInputs(Inputs);
		return true;
	}

	//calculates the values of a network
	bool NEURALNET_API GET_NETWORK_OUTPUTS(unsigned long long int handle, float* outputs, int n_outputs)
	{
		VALIDATE_HANDLE_BOOL

		//validate n_outputs
		if (n_outputs != handleLocation->second->OutputCount())
		{
			return false;
		}

		//copy over the network outputs to the output array
		std::vector<float> Outputs = handleLocation->second->GetResults();
		std::copy(Outputs.begin(), Outputs.end(), outputs);

		return true;
	}

	//TODO: make like half of these functions give error codes and give them some damn try-catch statements
	//returns the number of inputs, outputs or nodes in a network
	int NEURALNET_API GET_NETWORK_INPUT_COUNT(unsigned long long int handle)
	{
		VALIDATE_HANDLE_BOOL

		return handleLocation->second->InputCount();
	}
	int NEURALNET_API GET_NETWORK_NODE_COUNT(unsigned long long int handle)
	{
		VALIDATE_HANDLE_BOOL

		return handleLocation->second->NodeCount();
	}
	int NEURALNET_API GET_NETWORK_OUTPUT_COUNT(unsigned long long int handle)
	{
		VALIDATE_HANDLE_BOOL

		return handleLocation->second->OutputCount();
	}

	//adds a node between a connection to a network
	bool NEURALNET_API ADD_NODE_BETWEEN_CONNECTION(unsigned long long int handle, int TargetNodeIndex, int ConnectionIndex, float bias) 
	{
		VALIDATE_HANDLE_BOOL

		handleLocation->second->AddNodeBetweenConnection(TargetNodeIndex, ConnectionIndex, bias);
		return true;
	}
	//adds a connection between nodes to a network
	bool NEURALNET_API ADD_CONNECTION_BETWEEN_NODES(unsigned int handle, int SourceNodeIndex, int TargetNodeIndex, float weight) 
	{
		VALIDATE_HANDLE_BOOL
			
		handleLocation->second->AddConnectionBetweenNodes(SourceNodeIndex, TargetNodeIndex, weight);
		return true;
	}

	//removes a node from the network
	bool NEURALNET_API REMOVE_NODE(unsigned int long long handle, int NodeIndex) 
	{
		VALIDATE_HANDLE_BOOL

		handleLocation->second->RemoveNode(NodeIndex);
		return true;
	}
	//removes a connection from the network
	bool NEURALNET_API REMOVE_CONNECTION(unsigned long long int handle, int NodeIndex, int ConnectionIndex) 
	{
		VALIDATE_HANDLE_BOOL

		handleLocation->second->RemoveConnection(NodeIndex, ConnectionIndex);
		return true;
	}

	//gets the bias of a node
	float NEURALNET_API GET_NODE_BIAS(unsigned long long int handle, int NodeIndex) 
	{
		VALIDATE_HANDLE_BOOL

		return handleLocation->second->GetNodeBias(NodeIndex);
	}
	//sets the bias of a node
	bool NEURALNET_API SET_NODE_BIAS(unsigned long long int handle, int NodeIndex, float bias)
	{
		VALIDATE_HANDLE_BOOL

		handleLocation->second->SetNodeBias(NodeIndex, bias);
		return true;
	}

	//gets the total number of connections going into a connection
	int NEURALNET_API GET_NODE_CONNECTION_COUNT(unsigned long long int handle, int TargetNodeIndex) 
	{
		VALIDATE_HANDLE_INT

		return handleLocation->second->GetTotalNodeConnections(TargetNodeIndex);
	}

	//gets the weight of a connection
	float NEURALNET_API GET_CONNECTION_WEIGHT(unsigned long long int handle, int TargetNodeIndex, int ConnectionIndex) 
	{
		VALIDATE_HANDLE_BOOL

		return handleLocation->second->GetConnectionWeight(TargetNodeIndex, ConnectionIndex);
	}
	//sets the weight of a connection
	bool NEURALNET_API SET_CONNECTION_WEIGHT(unsigned long long int handle, int TargetNodeIndex, int ConnectionIndex, float weight)
	{
		VALIDATE_HANDLE_BOOL

		handleLocation->second->SetConnectionWeight(TargetNodeIndex, ConnectionIndex, weight);
		return true;
	}

	//saves a network
	bool NEURALNET_API SAVE_NETWORK(unsigned long long int handle, const char* path) 
	{
		VALIDATE_HANDLE_BOOL

		handleLocation->second->SaveNetwork(std::string(path));
		return true;
	}

    //breeds two networks together
    bool NEURALNET_API BREED_NETWORKS(unsigned long long int handleA, unsigned long long int handleB, int inputs, int outputs, int ActivationFunctionIndex, float* _CrossoverPoints, int nCrossoverPoints, float MutationChance, bool verbose)
    {
        //validate the network handles
        std::map<unsigned long long int, Network*>::iterator handleALocation = __NETWORKS.find(handleA); 
        if (handleALocation == __NETWORKS.end()) 
        { 
            /*invalid handle, indicate failure*/ 
            return false; 
        } 
        std::map<unsigned long long int, Network*>::iterator handleBLocation = __NETWORKS.find(handleB); 
        if (handleBLocation == __NETWORKS.end()) 
        { 
            /*invalid handle, indicate failure*/ 
            return false; 
        }
        //save the networks to be bred
        handleALocation->second->SaveNetwork("Networks/BreedA");
        handleBLocation->second->SaveNetwork("Networks/BreedB");

        //copy over the crossover points into a vector
        std::vector<float> CrossoverPoints;
        std::copy(_CrossoverPoints, _CrossoverPoints + nCrossoverPoints, std::back_inserter(CrossoverPoints));
        //create the breed settings struct
        Network::BreedSettings Settings {CrossoverPoints, MutationChance};

        //breed the networks
        Network* ChildNet = new Network("Networks/BreedA", "Networks/BreedB", Settings, inputs, outputs, verbose);

        //insert it into the map
		__NETWORKS[(unsigned long long int)ChildNet] = ChildNet;

		//return the "handle" which is really just the memory address of the network(guaranteed to be unique)
		return (unsigned long long int)ChildNet;

    }
}

//TODO: throw some definitions for these functions in a header and split this file into multiple files(maybe?)
//TODO: write bindings for most of these in python