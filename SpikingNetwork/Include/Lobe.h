#ifndef LOBE_H
#define LOBE_H

#include <algorithm>

#include "Neuron.h"

//forward definitions
class Neuron;

class Lobe
{
	//the neurons that belong to this lobe
	std::vector<Neuron*> Neurons;
public:
	
	//add a neuron to the lobe
	void AddNeuron(Neuron* NewNeuron);
	//remove a neuron from the lobe if it is in the lobe
	void RemoveNeuron(Neuron* OldNeuron);
};

#endif