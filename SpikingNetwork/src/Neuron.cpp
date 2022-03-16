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

//sets the state of the neuron
void Neuron::SetState(std::pair<float, unsigned long long int> ValuePair)
{
	Value = ValuePair.first;
    TimeSinceLastFire = ValuePair.second;
}

//sets the value of the neuron
void Neuron::SetValue(float value)
{
    Value = value;
}

//returns the value of the neuron
float Neuron::GetValue()
{
    return Value;
}

//write the state of the neuron to File
void Neuron::WriteStateToFile(std::ofstream &File) 
{
    //the bytes to write to the file at the end
    char NeuronStateBytes[12];

    //store the value of the neuron
    NeuronStateBytes[3] = (*(unsigned int*)&Value) & 0xff;
    NeuronStateBytes[2] = (*(unsigned int*)&Value >> 8) & 0xff;
    NeuronStateBytes[1] = (*(unsigned int*)&Value >> 16) & 0xff;
    NeuronStateBytes[0] = (*(unsigned int*)&Value >> 24) & 0xff;

    //store the time since the neuron last spiked
    NeuronStateBytes[11] = (TimeSinceLastFire) & 0xff;
    NeuronStateBytes[10] = (TimeSinceLastFire >> 8) & 0xff;
    NeuronStateBytes[9] = (TimeSinceLastFire >> 16) & 0xff;
    NeuronStateBytes[8] = (TimeSinceLastFire >> 24) & 0xff;
    NeuronStateBytes[7] = (TimeSinceLastFire >> 32) & 0xff;
    NeuronStateBytes[6] = (TimeSinceLastFire >> 40) & 0xff;
    NeuronStateBytes[5] = (TimeSinceLastFire >> 48) & 0xff;
    NeuronStateBytes[4] = (TimeSinceLastFire >> 56) & 0xff;

    //write the bytes to the file
    File.write(NeuronStateBytes, 12);
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