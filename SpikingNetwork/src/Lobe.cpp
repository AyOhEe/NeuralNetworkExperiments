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

//writes the lobe to File
void Lobe::WriteLobeToChromosome(std::ofstream& File) 
{
    //convert the lobe size into bytes and write it to the file
    unsigned int LobeSize = Neurons.size();
    char LobeSizeBytes[4] = {
        (LobeSize >> 24) & 0xff,
        (LobeSize >> 16) & 0xff,
        (LobeSize >> 8) & 0xff,
         LobeSize & 0xff,
    };
    File.write(LobeSizeBytes, 4);
}