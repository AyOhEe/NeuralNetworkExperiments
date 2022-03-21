#ifndef NEURON_H
#define NEURON_H

#include <vector>
#include <fstream>

#include "Connection.h"
#include "SpikingNetwork.h"
#include "Lobe.h"

//forward definitions
class Connection;
class SpikingNetwork;
class Lobe;

class Neuron 
{
	//connections need special access to neurons
	friend Connection;
	//as do spikingnetworks
	friend SpikingNetwork;
	//as do lobes
	friend Lobe;

	bool OutputType; //the output type of the neuron. false == depressive, true == potentiating
	float Value; //the current value of the neuron
	unsigned long long int TimeSinceLastFire; //the time sicne the neuron last fired(s^-5)
	float MembraneResistance; //the neuron's membrane resistance
	float ThresholdOffset; //the neuron's spike potential threshold offset
	Lobe* ParentLobe; //the lobe that the neuron belongs to

	//the connections to the neuron's sources and targets
	std::vector<Connection> SourceConnections;

public:

	//the parameters that define a neuron
	struct NeuronParams 
	{
		bool OutputType; //the output type of the neuron. false == depressive, true == potentiating
		float MembraneResistance; //the neuron's membrane resistance
		float ThresholdOffset; //the neuron's spike potential threshold offset
	};
	//sets the parameters of this neuron
	void SetParams(NeuronParams Params);
	//returns the parameters of this neuron
	NeuronParams GetParams();

	//calculates and returns(but does not store!) the new value of the neuron
	std::pair<float, unsigned long long int> CalculateNewValue(SpikingNetwork* Network, bool verbose = false);
	
	//sets the value of the neuron
	void SetState(std::pair<float, unsigned long long int> ValuePair);
	//sets the value of the neuron
	void SetValue(float value);
    //returns the value of the neuron
    float GetValue();
	//returns the current spike state of the neuron
	bool GetSpikeState();

	//write the state of the neuron to File
	void WriteStateToFile(std::ofstream &File);
	//loads a state into the neuron from a byte sequence
	void LoadStateFromBytes(char* StateBytes);

	//writes the neuron to File
	void WriteNeuronToFile(std::ofstream &File);
	//writes the neuron's connections to File
	void WriteConnectionsToFile(unsigned int Index, unsigned int Type, SpikingNetwork* Net, std::ofstream& File);

	//returns the number of source connections the neuron has
	unsigned int GetConnectionCount();

	//creates a neuron from a byte sequence
	Neuron(unsigned char* bytes);
};

#endif