#include <Lobe.h>

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


//TODO(aria): Error codes here
//does STDP to all neurons in this lobe
void Lobe::DoSTDP(SpikingNetwork* Net, int* ErrCode, bool verbose) 
{
    //get an stdp handler
    STDPHandler Handler = STDPHandler();

    //store all of the necessary data
    std::vector<unsigned long long> TargetTimes;
    std::vector<unsigned long long> SourceTimes;
    std::vector<float> Weights;
    //iterate through all neurons in the lobe
    for(std::vector<Neuron*>::iterator NeuronIter = Neurons.begin();
        NeuronIter != Neurons.end();
        NeuronIter++)
    {
        //iterate through it's connections
        for(std::vector<Connection>::iterator ConnIter = (*NeuronIter)->SourceConnections.begin();
            ConnIter != (*NeuronIter)->SourceConnections.begin();
            ConnIter++)
        { 
            //store the target time, source time and connection weight
            TargetTimes.push_back((*NeuronIter)->TimeSinceLastFire);
            SourceTimes.push_back(
                Net->GetNeuronPtr(ConnIter->SourceNeuron, ConnIter->SourceNeuronType, ErrCode, verbose)->TimeSinceLastFire
            );
            Weights.push_back(ConnIter->Weight);
        }
    }

    //hand it to the STDPHandler
    Weights = Handler.DoSTDP(SourceTimes, TargetTimes, Weights);

    //replace the weights
    std::vector<float>::iterator WeightIter = Weights.begin(); 
    std::vector<Neuron*>::iterator NeuronIter = Neurons.begin();
    while(WeightIter != Weights.end() && NeuronIter != Neurons.end())
    {
        //iterate through the connections in the current neuron
        for(std::vector<Connection>::iterator ConnIter = (*NeuronIter)->SourceConnections.begin();
            ConnIter != (*NeuronIter)->SourceConnections.begin();
            ConnIter++)
        {
            ConnIter->Weight = *WeightIter++;
        }
        NeuronIter++;
    }
}