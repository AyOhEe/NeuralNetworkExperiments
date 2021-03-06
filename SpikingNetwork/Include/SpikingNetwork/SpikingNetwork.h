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

#include "Definitions.h"
#include "Neuron.h"
#include "Lobe.h"
#include "FrequencyConverter.h"

//forward definitions
class Neuron;
struct NeuronParams;
class Connection;
struct ConnectionParams;
class Lobe;
class FrequencyConverter;

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
	//the lobe containing the output neurons
	Lobe* OutputLobe;

	//the past values of the output neurons
	std::vector<std::deque<float>*> OutputHistory;
 
	//the index for all neurons ever added to this network
	unsigned int UniqueNeuronIndex;
	//the index for all lobes ever added to this network
	unsigned int UniqueLobeIndex;

	//the frequency converter instance for this network
	FrequencyConverter FreqConverter;

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

	//the number of inputs, outputs, neurons and lobes in the network
	unsigned int InputCount();
	unsigned int OutputCount();
	unsigned int NeuronCount();
	unsigned int LobeCount();

	//adds a lobe to the network
	void AddLobe();
	//removes a lobe from the network
	void RemoveLobe(unsigned int Index);
	//gets the number of neurons in a lobe
	unsigned int GetLobeNeuronCount(unsigned int Index);

	//adds a neuron to a lobe in the network and to the network
	void AddNeuronToLobe(unsigned int LobeID, Neuron NewNeuron, int* ErrCode, bool verbose = false);

	//adds a neuron to the network
	void AddNeuron(unsigned int ID, unsigned int Type, unsigned int ConnectionIndex, Neuron NewNeuron, int* ErrCode, bool verbose = false);
	//removes the nth neuron
	void RemoveNeuron(unsigned int ID, int* ErrCode, bool verbose = false);
	//adds a connection to the neuron
	void AddConnection(unsigned int ID, unsigned int Type, Connection NewConn, int* ErrCode, bool verbose = false);
	//removes a connection from the neuron
	void RemoveConnection(unsigned int ID, unsigned int Type, unsigned int Index, int* ErrCode, bool verbose = false);
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
	//returns the neuron's index
	unsigned int GetNeuronIndex(unsigned int ID, unsigned int Type, int* ErrCode, bool verbose = false);

	//sets the parameters of the internal neuron to those in Params
	void SetNeuronParams(unsigned int ID, NeuronParams Params, int* ErrCode, bool verbose);
	//gets the parameters of the internal neuron
	NeuronParams GetNeuronParams(unsigned int ID, int* ErrCode, bool verbose);

	//sets the parameters of a connection in a neuron
	void SetConnectionParams(unsigned int ID, unsigned int Type, unsigned int index, ConnectionParams Params, int* ErrCode, bool verbose = false);
	//gets the parameters of a connection in a neuron
	ConnectionParams GetConnectionParams(unsigned int ID, unsigned int Type, unsigned int index, int* ErrCode, bool verbose = false);
};

#endif