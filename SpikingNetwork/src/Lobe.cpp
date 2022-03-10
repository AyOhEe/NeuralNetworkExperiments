#include "../Include/Lobe.h"

//add a neuron to the lobe
void Lobe::AddNeuron(Neuron* NewNeuron)
{
    //add the neuron to the lobe
    Neurons.push_back(NewNeuron);
}

//remove a neuron from the lobe if it is in the lobe
void Lobe::RemoveNeuron(Neuron* OldNeuron)
{
    //remove the neuron from the lobe if it exists
    std::remove(Neurons.begin(), Neurons.end(), OldNeuron);
}