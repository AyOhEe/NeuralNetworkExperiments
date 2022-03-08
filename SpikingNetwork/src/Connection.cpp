#include "../Include/Connection.h"

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

    //this should be impossible to reach, but just incase, return false
    return false;
}