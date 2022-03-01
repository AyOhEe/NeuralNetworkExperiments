#ifndef SPIKING_NETWORK_H
#define SPIKING_NETWORK_H

#include <vector>
#include <string>

#include "Neuron.h"

//forward definitions
class Neuron;

//network class that uses spiking neurons and STDP
class SpikingNetwork 
{
	//all neurons in the network
	std::vector<Neuron> Neurons;

public:
	
	//creates a spiking network based on the genome at genomepath
	SpikingNetwork(std::string GenomePath, int inputs, int outputs, int* ErrCode, bool verbose = false);

	//loads a mental state from a file
	void LoadMentalState(std::string StatePath, int* ErrCode, bool verbose = false);
	//stores a mental state to a file
	void StoreMentalState(std::string StatePath, int* ErrCode, bool verbose = false);

	//saves the network to the genome at genomepath
	void SaveNetwork(std::string GenomePath, int* ErrCode, bool verbose = false);

	//sets the input values
	void SetInputs(std::vector<float> inputs, int* ErrCode, bool verbose = false);
	//gets the output values
	std::vector<float> GetOutputs(int* ErrCode, bool verbose = false);
	//performs an update on the network
	void PerformUpdate(int* ErrCode, bool verbose = false);
};

#endif