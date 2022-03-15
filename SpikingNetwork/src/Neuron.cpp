#include "../Include/Neuron.h"

//calculates and returns(but does not store!) the new value of the neuron
std::pair<float, unsigned long long int> Neuron::CalculateNewValue(SpikingNetwork* Network, bool verbose)
{
    //calculate and apply value falloff
    float NewValue = Value - (Value / MembraneResistance);
    unsigned long long int NewTimeSinceLastFire = TimeSinceLastFire + 1;
    
    //iterate through our connections
    for(std::vector<Connection>::iterator ConnIter = SourceConnections.begin(); 
        ConnIter != SourceConnections.end(); 
        ConnIter++)
    {
        //attempt to add the connection's value to our value
        if(ConnIter->TryAddValue(Network, &NewValue))
        {
            //invalid connection, remove it
            SourceConnections.erase(ConnIter);
            //move the iterator back one so that we'll be at the same place next iteration
            ConnIter--;
        }
    }

    //if we're past our spike threshold, 
    if(NewValue > ThresholdOffset)
    {
        //multiply our value by 5
        NewValue *= 5;
        //reset the time since last spike
        NewTimeSinceLastFire = 0;
    }

    //TODO(aria): handle STDP here

    //return the new value and new time since last fire
    return std::make_pair(NewValue, NewTimeSinceLastFire);
}

//sets the value of the neuron
void Neuron::SetValue(float NewValue) 
{
	Value = NewValue;
}

//returns the value of the neuron
float Neuron::GetValue()
{
    return Value;
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