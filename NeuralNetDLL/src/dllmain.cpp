// dllmain.cpp : Defines the entry point for the DLL application.
#include "../Include/framework.h"

#include <map>
#include <algorithm>

#include "../../NeuralNetwork/Include/Network.h"

//all networks alive
std::vector<Network*> __NETWORKS = std::vector<Network*>();

//creates a network and returns a handle
long long int APIENTRY CREATE_NETWORK(const char* GenomePath, int Inputs, int Outputs, int ActivationFunctionIndex) 
{
	//create the new network
	Network* Net = new Network(std::string(GenomePath), Inputs, Outputs, ActivationFunctionIndex);

	//insert it into the vector
	__NETWORKS.push_back(Net);

	//return the "handle" which is really just the memory address of the network(guaranteed to be unique)
	return (long long int)Net;
}

//removes a network if it exists
BOOL APIENTRY DESTROY_NETWORK(long long int handle) 
{
	//ensure the handle is valid
	std::vector<Network*>::iterator handleLocation = std::find(__NETWORKS.begin(), __NETWORKS.end(), (Network*)handle);
	if (handleLocation == __NETWORKS.end()) 
	{
		//invalid handle, indicate failure
		return FALSE;
	}

	//valid handle, delete the network and remove it from the vector
	__NETWORKS.erase(handleLocation);
	delete (Network*)handle;

	//indicate success
	return TRUE;
}

//sets the inputs of a network
BOOL APIENTRY SET_NETWORK_INPUTS(long long int handle, float* inputs, int n_inputs) 
{	
	//TODO: validate n_inputs

	//ensure the handle is valid
	std::vector<Network*>::iterator handleLocation = std::find(__NETWORKS.begin(), __NETWORKS.end(), (Network*)handle);
	if (handleLocation == __NETWORKS.end())
	{
		//invalid handle, indicate failure
		return FALSE;
	}

	//convert the inputs into a vector
	std::vector<float> Inputs;
	std::copy(inputs, inputs + (n_inputs / sizeof(float)), std::back_inserter(Inputs));

	//set the inputs
	(*handleLocation)->SetInputs(Inputs);
	return TRUE;
}

//calculates the values of a network
BOOL APIENTRY GET_NETWORK_OUTPUTS(long long int handle, float* outputs, int n_outputs) 
{
	//TODO: validate n_outputs

	//ensure the handle is valid
	std::vector<Network*>::iterator handleLocation = std::find(__NETWORKS.begin(), __NETWORKS.end(), (Network*)handle);
	if (handleLocation == __NETWORKS.end())
	{
		//invalid handle, indicate failure
		return FALSE;
	}

	//copy over the network outputs to the output array
	std::vector<float> Outputs = (*handleLocation)->GetResults();
	std::copy(Outputs.begin(), Outputs.end(), outputs);

	return TRUE;
}