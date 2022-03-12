#include "../Include/Connection.h"

//creates a connection in Network from bytes
Connection::Connection(unsigned char* bytes, SpikingNetwork* Net) 
{
    //calculate our information
    bool SourceType = (bytes[0] & 0b10000000) == 0b10000000;
    unsigned int SourceIndex = (bytes[1] << 24) + (bytes[2] << 16) + (bytes[3] << 8) + bytes[5];
    unsigned int IntWeight = (bytes[9] << 24) + (bytes[10] << 16) + (bytes[11] << 8) + bytes[12];

    //store our weight
    Weight = *(float*)(&IntWeight);
    
    //get our source neuron
    SourceNeuron = std::next(Net->Neurons.begin(), SourceIndex)->first;
}

//creates a connection
void Connection::CreateConnection(unsigned char* bytes, SpikingNetwork* Net) 
{
    //calculate our information
    bool TargetType = (bytes[0] & 0b01000000) == 0b01000000;
    unsigned int TargetIndex = (bytes[5] << 24) + (bytes[6] << 16) + (bytes[7] << 8) + bytes[8];

    //create a connection from our data
    Connection NewConnection = Connection(bytes, Net);

    //add the connection to the correct node
    std::next(Net->Neurons.begin(), TargetIndex)->second->SourceConnections.push_back(NewConnection);
}

//attempts to add the value of this connection to OutVal. returns false if the source doesn't exist
bool Connection::TryAddValue(SpikingNetwork* Network, float* OutVal)
{
    try
    {
        //increase outval by the weighted value of out source node
        *OutVal += Weight * (Network->Neurons[SourceNeuron]->GetValue());
        
        //if we reached this point, the connection was valid. return true
        return true;
    }
    catch(std::exception &ex)
    {
        //if we catch an exception, that means the connection was invalid. return false
        return false;
    }

    //this should be impossible to reach, but just in case, return false
    return false;
}