#ifndef NEURON_H
#define NEURON_H

#include <vector>

#include "Connection.h"
#include "SpikingNetwork.h"

//forward definitions
class Connection;
class SpikingNetwork;

class Neuron 
{
	//connections need special access to neurons
	friend Connection;

	bool NeuronType; //the type of the neuron. false == spiking, true == capacitor
	bool OutputType; //the output type of the neuron. false == depressive, true == potentiating
	float Value; //the current value of the neuron
	unsigned long long int TimeSinceLastFire; //the time sicne the neuron last fired(s^-5)
	float MembraneResistance; //the neuron's membrane resistance
	float ThresholdOffset; //the neuron's spike potential threshold offset

	//the connections to the neuron's sources and targets
	std::vector<Connection> SourceConnections;

public:

	//calculates and returns(but does not store!) the new value of the neuron
	std::pair<float, unsigned long long int> CalculateNewValue(SpikingNetwork* Network, bool verbose = false);
	//sets the value of the neuron
	void SetState(std::pair<float, unsigned long long int> ValuePair);
    //returns the value of the neuron
    float GetValue();

	//creates a neuron from a byte sequence
	Neuron(unsigned char* bytes);
};

#endif