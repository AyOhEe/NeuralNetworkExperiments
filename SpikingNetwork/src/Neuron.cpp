#include <Neuron.h>

//sets the parameters of this neuron
void Neuron::SetParams(NeuronParams Params) 
{
    //set our parameters
    OutputType = Params.OutputType;
    MembraneResistance = Params.MembraneResistance;
    ThresholdOffset = Params.ThresholdOffset;
}

//returns the parameters of this neuron
NeuronParams Neuron::GetParams()
{
    //return our parameters
    return NeuronParams { OutputType, MembraneResistance, ThresholdOffset };
}

//sets the parameters of connection at index
void Neuron::SetConnectionParams(unsigned int Index, SpikingNetwork* Net, ConnectionParams Params, int* ErrCode, bool verbose)
{
    //set the connection's parameters
    SourceConnections[Index % SourceConnections.size()].SetParams(Params, Net, ErrCode, verbose);
}
//gets the parameters of connection at index
ConnectionParams Neuron::GetConnectionParams(unsigned int Index, SpikingNetwork* Net, int *ErrCode, bool verbose) 
{
    //return the connection's parameters
    return SourceConnections[Index % SourceConnections.size()].GetParams(Net, ErrCode, verbose);
}

//calculates and returns(but does not store!) the new value of the neuron
std::pair<float, unsigned long long int> Neuron::CalculateNewValue(SpikingNetwork* Network, bool verbose)
{
    //we're currently spiking, set the new value to zero and return
    if(GetSpikeState())
    {
        return std::make_pair(0.0f, TimeSinceLastFire + 1);
    }

    //calculate and apply value falloff
    float NewValue = Value - (Value / MembraneResistance);
    unsigned long long int NewTimeSinceLastFire = TimeSinceLastFire + 1;
    
    //iterate through our connections
    for(std::vector<Connection>::iterator ConnIter = SourceConnections.begin(); 
        ConnIter != SourceConnections.end(); 
        ConnIter++)
    {
        //attempt to add the connection's value to our value
        if(!ConnIter->TryAddValue(Network, &NewValue))
        {
            //invalid connection, remove it
            SourceConnections.erase(ConnIter);
            //move the iterator back one so that we'll be at the same place next iteration
            ConnIter--;
        }
    }

    //are we past our spike threshold?
    if(NewValue > ThresholdOffset)
    {
        //yes. reset the time since last spike
        NewTimeSinceLastFire = 0;
    }

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

//returns the current spike state of the neuron
bool Neuron::GetSpikeState() 
{
    return Value > ThresholdOffset;
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

//loads a state into the neuron from a byte sequence
void Neuron::LoadStateFromBytes(char* StateBytes) 
{
    //load in the value
    unsigned int ValueInt = 
         (StateBytes[0] << 24) 
        +(StateBytes[1] << 16) 
        +(StateBytes[2] << 8) 
        + StateBytes[3];
    Value = *(float*)&ValueInt;

    //load in the time since last fire
    TimeSinceLastFire =
         ((long long)StateBytes[4] << 56)
        +((long long)StateBytes[5] << 48)
        +((long long)StateBytes[6] << 40)
        +((long long)StateBytes[7] << 32)
        +((long long)StateBytes[8] << 24)
        +((long long)StateBytes[9] << 16)
        +((long long)StateBytes[10] << 8)
        + (long long)StateBytes[11];
}

//writes the neuron to File
void Neuron::WriteNeuronToFile(std::ofstream& File) 
{
    //byte array to write to the file at the end
    char Bytes[9];

    //convert the membrane resistance and the threshold offset to integers(for binary access)
    unsigned int UIntMembraneResistance = *(unsigned int*)&MembraneResistance;
    unsigned int UIntThresholdOffset = *(unsigned int*)&ThresholdOffset;

    //store our data into the byte array
    Bytes[0] = (OutputType ? 0b01000000 : 0);

    Bytes[1] = (UIntMembraneResistance >> 24) & 0xff;
    Bytes[2] = (UIntMembraneResistance >> 16) & 0xff;
    Bytes[3] = (UIntMembraneResistance >> 8) & 0xff;
    Bytes[4] = (UIntMembraneResistance) & 0xff;

    Bytes[5] = (UIntThresholdOffset >> 24) & 0xff;
    Bytes[6] = (UIntThresholdOffset >> 16) & 0xff;
    Bytes[7] = (UIntThresholdOffset >> 8) & 0xff;
    Bytes[8] = (UIntThresholdOffset ) & 0xff;

    //write the bytes to the file
    File.write(Bytes, 9);
}
//writes the neuron's connections to File
void Neuron::WriteConnectionsToFile(unsigned int Index, unsigned int Type, SpikingNetwork* Net, std::ofstream& File) 
{
    //iterate through our connections
    for(std::vector<Connection>::iterator ConnIter = SourceConnections.begin();
        ConnIter != SourceConnections.end();
        ConnIter++)
    {
        //write each connection to the file
        ConnIter->WriteConnectionToFile(Index, Type, Net, File);
    }
}

//returns the number of source connections the neuron has
unsigned int Neuron::GetConnectionCount() 
{
    return SourceConnections.size();
}

//creates a neuron from a byte sequence
Neuron::Neuron(unsigned char* bytes) 
{
	//calculate the membrane resistance and threshold offset
	unsigned int IntMembraneResistance = (bytes[1] << 24) + (bytes[2] << 16) + (bytes[3] << 8) + bytes[4];
	unsigned int IntThresholdOffset = (bytes[5] << 24) + (bytes[6] << 16) + (bytes[7] << 8) + bytes[8];

	//get our values
	OutputType = (bytes[0] & 0b01000000) == 0b01000000;
	MembraneResistance = *((float*)&IntMembraneResistance);
	ThresholdOffset = *((float*)&IntThresholdOffset);
}
//creates a neuron from a parameter set
Neuron::Neuron(NeuronParams Params) 
{
    OutputType = Params.OutputType;
    MembraneResistance = Params.MembraneResistance;
    ThresholdOffset = Params.ThresholdOffset;
}

//adds a connection to the neuron
void Neuron::AddConnection(Connection NewConn) 
{
    //add the connection
    SourceConnections.push_back(NewConn);
}
//removes a connection from the neuron
void Neuron::RemoveConnection(unsigned int Index) 
{
    //get the connection at index mod ConnectionCount
    std::vector<Connection>::iterator ConnIter = SourceConnections.begin();
    std::advance(ConnIter, Index % SourceConnections.size());
    //remove the connection
    SourceConnections.erase(ConnIter);
}