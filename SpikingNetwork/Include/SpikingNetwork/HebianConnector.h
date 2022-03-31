#ifndef HEBIAN_CONNECTOR_H
#define HEBIAN_CONNECTOR_H

#include <vector>

#include <CL/cl.hpp>

//the actual manager for OpenCL and Hebian Connection
class __HebianConnector
{
	//OpenCL Variables
	cl::Context CLContext;
	cl::Kernel HebConnKernel;
	cl::CommandQueue CLCommandQueue;

public:
	//creates the __HebianConnector
	__HebianConnector();

	//returns what connections should be made
	std::vector<float> DoHebianConnection(
		std::vector<bool> LobeASpikeHistory,
		std::vector<bool> LobeBSpikeHistory,
		unsigned int LobeANeuronCount,
		unsigned int LobeBNeuronCount,
		std::vector<float> ConnectionWeights
	);
};

//the interface class for Hebian Connection
class HebianConnector
{
	//the singleton handler instance 
	static __HebianConnector* Connector;

public:
	//creates the HebianConnector
	HebianConnector();

	//returns what connections should be made
	std::vector<float> DoHebianConnection(
		std::vector<bool> LobeASpikeHistory,
		std::vector<bool> LobeBSpikeHistory,
		unsigned int LobeANeuronCount,
		unsigned int LobeBNeuronCount,
		std::vector<float> ConnectionWeights
	);
};

#endif