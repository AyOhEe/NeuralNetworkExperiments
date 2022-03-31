#ifndef LOBE_H
#define LOBE_H

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>

#include "Definitions.h"
#include "Neuron.h"
#include "STDPHandler.h"

//forward definitions
class Neuron;
class Connection;
class SpikingNetwork;

class Lobe
{
	//the neurons that belong to this lobe
	std::vector<Neuron*> Neurons;

	//returns the spike history of this lobe
	std::vector<std::vector<bool>> GetSpikeHistory();
public:
	
	//add a neuron to the lobe
	void AddNeuron(Neuron* NewNeuron);
	//remove a neuron from the lobe if it is in the lobe
	void RemoveNeuron(Neuron* OldNeuron);
	//returns the number of neurons in the lobe
	unsigned int GetNeuronCount();

	//writes the lobe to File
	void WriteLobeToChromosome(std::ofstream &File);

	//does STDP to all neurons in this lobe
	void DoSTDP(SpikingNetwork* Net, int* ErrCode, bool verbose);

	//adds connections between neurons that have consistently fired at the same or similar times
	static void DoHebianCheck(Lobe LobeA, Lobe LobeB, bool* ConnectionMapping);
};

#endif