#ifndef NEURON_H
#define NEURON_H

#include <vector>

class Neuron 
{
	//the value of the neuron
	float Value;

public:

	//calculates and returns(but does not store!) the new value of the neuron
	float CalculateNewValue(bool verbose = false);
	//sets the value of the neuron
	void SetValue(float NewValue);

};

#endif