#include "../Include/Lobe.h"

//add a neuron to the lobe
void Lobe::AddNeuron(Neuron* NewNeuron)
{
    //add the neuron to the lobe
    Neurons.push_back(NewNeuron);
    //and register this lobe as it's parent lobe
    NewNeuron->ParentLobe = this;
}

//remove a neuron from the lobe if it is in the lobe
void Lobe::RemoveNeuron(Neuron* OldNeuron)
{
    //remove the neuron from the lobe if it exists
    std::vector<Neuron*>::iterator NeuronIter = std::find(Neurons.begin(), Neurons.end(), OldNeuron);
    if(NeuronIter != Neurons.end())
    {
        OldNeuron->ParentLobe = (Lobe*)nullptr;
        Neurons.erase(NeuronIter);
    }
}

//writes the lobe to File
void Lobe::WriteLobeToChromosome(std::ofstream& File) 
{
    //convert the lobe size into bytes and write it to the file
    unsigned int LobeSize = Neurons.size();
    char LobeSizeBytes[4] = {
        (char)(LobeSize >> 24),
        (char)(LobeSize >> 16),
        (char)(LobeSize >> 8),
         (char)LobeSize,
    };
    File.write(LobeSizeBytes, 4);
}