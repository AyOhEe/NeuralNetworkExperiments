// dllmain.cpp : Defines the entry point for the DLL application.
#include "../Include/framework.h"

#include <map>
#include <algorithm>

#include "../../NeuralNetwork/Include/Network.h"

#ifdef NEURALNETDLL_EXPORTS
#define NEURALNET_API __declspec(dllexport)
#else
#define NEURALNET_API __declspec(dllimport)
#endif

//all networks alive
std::map<unsigned int, Network> __NETWORKS = std::map<unsigned int, Network>();

extern "C"
{
	//creates a network and returns a handle
	unsigned int NEURALNET_API CREATE_NETWORK(const char* GenomePath, int Inputs, int Outputs, int ActivationFunctionIndex)
	{
		//create the new network
		Network Net = Network(std::string(GenomePath), Inputs, Outputs, ActivationFunctionIndex);

		//insert it into the vector
		__NETWORKS[(unsigned int)&Net] = Net;

		//return the "handle" which is really just the memory address of the network(guaranteed to be unique)
		return (unsigned int)&Net;
	}

	//removes a network if it exists
	BOOL NEURALNET_API DESTROY_NETWORK(unsigned int handle)
	{
		//ensure the handle is valid
		std::map<unsigned int, Network>::iterator handleLocation = __NETWORKS.find(handle);
		if (handleLocation == __NETWORKS.end()) 
		{
			//invalid handle, indicate failure
			return FALSE;
		}

		//valid handle, delete the network and remove it from the vector
		__NETWORKS.erase(handleLocation);

		//indicate success
		return TRUE;
	}

	//sets the inputs of a network
	BOOL NEURALNET_API SET_NETWORK_INPUTS(unsigned int handle, float* inputs, int n_inputs)
	{	
		//ensure the handle is valid
		std::map<unsigned int, Network>::iterator handleLocation = __NETWORKS.find(handle);
		if (handleLocation == __NETWORKS.end())
		{
			//invalid handle, indicate failure
			return FALSE;
		}

		//validate n_inputs
		if (n_inputs != handleLocation->second.Inputs())
		{
			return FALSE;
		}

		//convert the inputs into a vector
		std::vector<float> Inputs;
		std::copy(inputs, inputs + n_inputs, std::back_inserter(Inputs));

		//set the inputs
		handleLocation->second.SetInputs(Inputs);
		return TRUE;
	}

	//calculates the values of a network
	BOOL NEURALNET_API GET_NETWORK_OUTPUTS(unsigned int handle, float* outputs, int n_outputs)
	{
		//ensure the handle is valid
		std::map<unsigned int, Network>::iterator handleLocation = __NETWORKS.find(handle);
		if (handleLocation == __NETWORKS.end())
		{
			//invalid handle, indicate failure
			return FALSE;
		}

		//validate n_outputs
		if (n_outputs != handleLocation->second.Outputs()) 
		{
			return FALSE;
		}

		//copy over the network outputs to the output array
		std::vector<float> Outputs = handleLocation->second.GetResults();
		std::copy(Outputs.begin(), Outputs.end(), outputs);

		return TRUE;
	}
}