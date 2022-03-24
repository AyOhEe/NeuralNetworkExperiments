#include <Connection.h>

//sets the parameters of the connection
void Connection::SetParams(ConnectionParams Params, SpikingNetwork* Net, int* ErrCode, bool verbose) 
{
    //set our parameters
    Weight = Params.Weight;
    SourceNeuron = Net->GetNeuronID(Params.SourceNeuronIndex, Params.SourceNeuronType, ErrCode, verbose);
    SourceNeuronType = Params.SourceNeuronType;
}
//gets the parameters of the connection
ConnectionParams Connection::GetParams(SpikingNetwork* Net, int* ErrCode, bool verbose) 
{
    //return our parameters
    return ConnectionParams { 
        Weight, 
        Net->GetNeuronIndex(SourceNeuron, SourceNeuronType, ErrCode, verbose), 
        SourceNeuronType
    };
}

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
    //TODO(aria): handle error codes here
    int ErrCode = 0;
    SourceNeuron = Net->GetNeuronID(SourceIndex, SourceType ? 1 : 0, &ErrCode);
    SourceNeuronType = SourceType;
}

//creates a connection with source id ID and type Type and weight Weight
Connection::Connection(unsigned int ID, bool Type, float NewWeight) 
{
    //store our values
    SourceNeuron = ID;
    SourceNeuronType = Type;
    Weight = NewWeight;
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
    //TODO(aria): handle error code here
    int ErrCode = 0;
    Net->GetNeuronPtr(TargetIndex, TargetType ? 2 : 1, &ErrCode)->SourceConnections.push_back(NewConnection);
}

//attempts to add the value of this connection to OutVal. returns false if the source doesn't exist
bool Connection::TryAddValue(SpikingNetwork* Network, float* OutVal)
{
    try
    {
        //increase outval by the weighted value of out source node
        //TODO(aria): handle error codes here
        //TODO(aria): alter this to use a constant value along with spike state
        int ErrCode = 0;
        Neuron* NeuronPtr = Network->GetNeuronPtr(SourceNeuron, SourceNeuronType ? 1 : 0, &ErrCode);
        *OutVal += Weight * (NeuronPtr->GetValue()) * (NeuronPtr->OutputType ? 1 : -1);
        
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

//writes the connection to a file
void Connection::WriteConnectionToFile(unsigned int TargetIndex, unsigned int TargetType, SpikingNetwork* Net, std::ofstream& File) 
{
    //the bytes to write to the file
    char Bytes[13];
    
    //convert the weight to an integer(binary access)
    unsigned int UIntWeight = *(unsigned int*)&Weight;
    //get the source neuron index
    //TODO(aria): error codes here
    int ErrCode = 0;
    unsigned int SourceNeuronIndex = Net->GetNeuronIndex(SourceNeuron, SourceNeuronType, &ErrCode);

    //store our data
    Bytes[0] = (SourceNeuronType ? 0b10000000 : 0) + (TargetType ? 0b01000000 : 0);

    Bytes[1] = (SourceNeuronIndex >> 24) & 0xff;
    Bytes[2] = (SourceNeuronIndex >> 16) & 0xff;
    Bytes[3] = (SourceNeuronIndex >> 8) & 0xff;
    Bytes[4] = (SourceNeuronIndex) & 0xff;

    Bytes[5] = (TargetIndex >> 24) & 0xff;
    Bytes[6] = (TargetIndex >> 16) & 0xff;
    Bytes[7] = (TargetIndex >> 8) & 0xff;
    Bytes[8] = (TargetIndex) & 0xff;

    Bytes[9] = (UIntWeight >> 24) & 0xff;
    Bytes[10] = (UIntWeight >> 16) & 0xff;
    Bytes[11] = (UIntWeight >> 8) & 0xff;
    Bytes[12] = (UIntWeight) & 0xff;

    //write the data
    File.write(Bytes, 13);
}