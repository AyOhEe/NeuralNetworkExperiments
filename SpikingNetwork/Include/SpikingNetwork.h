#ifndef SPIKING_NETWORK_H
#define SPIKING_NETWORK_H

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <stack>
#include <algorithm>
#include <iterator>

#include "Neuron.h"
#include "Lobe.h"

//forward definitions
class Neuron;
class Connection;

//network class that uses spiking neurons and STDP
class SpikingNetwork 
{
    //connections need special access to the neuron and lobe maps
    friend Connection;

	//all neurons in the network
	std::map<unsigned int, Neuron*> Neurons;
	//all of the input and output neurons in this network
	std::vector<Neuron> InputNeurons;
	std::vector<Neuron> OutputNeurons;
	//all of the lobes in the network
	std::map<unsigned int, Lobe> Lobes;

	//the index for all neurons ever added to this network
	unsigned int UniqueNeuronIndex;
	//the index for all lobes ever added to this network
	unsigned int UniqueLobeIndex;

public:
	
	//creates a spiking network based on the genome at genomepath
	SpikingNetwork(std::string GenomePath, int inputs, int outputs, int* ErrCode, bool verbose = false);
	//destructor to handle potential data leaks
	~SpikingNetwork();

	//loads a mental state from a file
	void LoadMentalState(std::string StatePath, int* ErrCode, bool verbose = false);
	//stores a mental state to a file
	void StoreMentalState(std::string StatePath, int* ErrCode, bool verbose = false);

	//saves the network to the genome at genomepath
	void SaveNetwork(std::string GenomePath, int* ErrCode, bool verbose = false);

	//the number of inputs, outputs and neurons in the network
	int InputCount();
	int OutputCount();
	int NeuronCount();

	//adds a neuron to the network
	void AddNeuron(unsigned int NeuronIndex, unsigned int ConnectionIndex, Neuron NewNeuron, int* ErrCode, bool verbose = false);
	//removes the nth neuron
	void RemoveNeuron(unsigned int Index, int* ErrCode, bool verbose = false);
	//get the number of connections in a neuron
	unsigned int GetNeuronConnectionCount(unsigned int ID, unsigned int Type, int* ErrCode, bool verbose = false);

	//sets the input values
	void SetInputs(std::vector<float> inputs, int* ErrCode, bool verbose = false);
	//gets the output values
	std::vector<float> GetOutputs(int* ErrCode, bool verbose = false);
	//performs an update on the network
	void PerformUpdate(int* ErrCode, bool verbose = false);

	//returns a pointer to a neuron from an id and type
	Neuron* GetNeuronPtr(unsigned int ID, unsigned int Type, int* ErrCode, bool verbose = false);
	//returns the neuron's id(or index if input/output)
	unsigned int GetNeuronID(unsigned int Index, unsigned int Type, int* ErrCode, bool verbose = false);
};

#endif