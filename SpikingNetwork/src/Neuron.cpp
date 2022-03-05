#include "../Include/Neuron.h"

//sets the value of the neuron
void Neuron::SetValue(float NewValue) 
{
	Value = NewValue;
}

//creates a neuron from a byte sequence
Neuron::Neuron(unsigned char* bytes) 
{
	//calculate the membrane resistance and threshold offset
	unsigned int IntMembraneResistance = (bytes[1] << 24) + (bytes[2] << 16) + (bytes[3] << 8) + bytes[4];
	unsigned int IntThresholdOffset = (bytes[5] << 24) + (bytes[6] << 16) + (bytes[7] << 8) + bytes[8];

	//get our values
	NeuronType = (bytes[0] & 0b10000000) == 0b10000000;
	OutputType = (bytes[0] & 0b01000000) == 0b01000000;
	MembraneResistance = *((float*)&IntMembraneResistance);
	ThresholdOffset = *((float*)&IntThresholdOffset);
}