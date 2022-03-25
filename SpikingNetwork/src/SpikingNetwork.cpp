#include <SpikingNetwork.h>

//TODO(aria): error codes here
//creates a spiking network based on the genome at genomepath
SpikingNetwork::SpikingNetwork(std::string GenomePath, int inputs, int outputs, int* ErrCode, bool verbose)
{
	//initialise unique counters
	UniqueNeuronIndex = 0;
	UniqueLobeIndex = 0;

	//try to open the files
	//TODO(aria): replace "/" with system path separator
	std::ifstream NeuronChromosome = std::ifstream(
		GenomePath + "/" + "Neurons.chr2",
		std::ios::binary | std::ios::in
	);
	//TODO(aria): replace "/" with system path separator
	std::ifstream ConnectionChromosome = std::ifstream(
		GenomePath + "/" + "Connections.chr2",
		std::ios::binary | std::ios::in
	);
	if(!NeuronChromosome)
	{
		//TODO(aria): error codes here
		throw std::invalid_argument("Failed to open Neuron Chromosome at path: " + GenomePath);
	}
	if (!ConnectionChromosome)
	{
		//TODO(aria): error codes here
		throw std::invalid_argument("Failed to open Connection Chromosome at path: " + GenomePath);
	}

	//read in the number of lobes
	unsigned char* Bytes = new unsigned char[4];
	NeuronChromosome.read((char*)Bytes, 4);
	unsigned int LobeCount = (Bytes[0] << 24) + (Bytes[1] << 16) + (Bytes[2] << 8) + Bytes[3];
	if (verbose)
	{
		std::cout << "LobeCount bytes: " << (int)Bytes[0] << ", " << (int)Bytes[1] << ", " << (int)Bytes[2] << ", " << (int)Bytes[3] << std::endl;
	}

	//read in the lobe sizes
	unsigned int* LobeSizes = new unsigned int[LobeCount];
	for (unsigned int i = 0; i < LobeCount; i++)
	{
		NeuronChromosome.read((char*)Bytes, 4);
		LobeSizes[i] = (Bytes[0] << 24) + (Bytes[1] << 16) + (Bytes[2] << 8) + Bytes[3];
		if (verbose)
		{
			std::cout << "LobeSize bytes: " << (int)Bytes[0] << ", " << (int)Bytes[1] << ", " << (int)Bytes[2] << ", " << (int)Bytes[3] << std::endl;
		}
	}
	delete[] Bytes;
	if (LobeCount != 0 && NeuronChromosome.eof())
	{
		//it was, we were fed a malformed file
		throw std::invalid_argument("Malformed Neuron Chromosome! Invalid lobes!");
	}

	if (verbose) 
	{
		std::cout << "Lobe Count: " << LobeCount << std::endl;
		for (unsigned int i = 0; i < LobeCount; i++)
		{
			std::cout << "\t Lobe " << i << " Size: " << LobeSizes[i] << std::endl;
		}
	}

	//read the neuron chromosome
	Bytes = new unsigned char[9];
	for(unsigned int i = 0; i < LobeCount; i++)
	{
		//create the lobe to contain the next neurons
		Lobes.insert(std::make_pair(UniqueLobeIndex, Lobe()));
		Lobe* CurrentLobe = &(Lobes[UniqueLobeIndex++]);

		//begin reading the genes
		for(unsigned int j = 0; j < LobeSizes[i]; j++)
		{
			//read the gene
			NeuronChromosome.read((char*)Bytes, 9);
			//check if we've hit eof
			if(NeuronChromosome.eof())
			{
				//yep, disregrd this gene
				break;
			}

			//create the neuron and store it's pointer
			Neuron* NewNeuron = new Neuron(Bytes);
			Neurons.insert(std::make_pair(UniqueNeuronIndex, NewNeuron));
			//register it with the lobe
			CurrentLobe->AddNeuron(Neurons[UniqueNeuronIndex++]);
		}
		//check if we've hit eof
		if (NeuronChromosome.eof())
		{
			//yep, stop reading
			break;
		}
	}
	delete[] Bytes;

	//create the input and output neurons and the output lobe
	for(int i = 0; i < inputs; i++)
	{
		InputNeurons.push_back(Neuron(NeuronParams{ true, 1, 0 }));
	}
	OutputLobe = new Lobe();
	for (int i = 0; i < outputs; i++)
	{
		OutputNeurons.push_back(Neuron(NeuronParams{ true, 1, 0 }));
	}
	for(int i = 0; i < outputs; i++)
	{
		OutputLobe->AddNeuron(&OutputNeurons[i]);
	}

	//read the connection chromosome
	Bytes = new unsigned char[13];
	while (!ConnectionChromosome.eof()) 
	{
		//read in the gene
		ConnectionChromosome.read((char*)Bytes, 13);
		if (!ConnectionChromosome.eof()) 
		{
			//create the connection
			Connection::CreateConnection(Bytes, this);
		}
	}
	delete[] Bytes;

	//close the files
	NeuronChromosome.close();
	ConnectionChromosome.close();
}
//destructor to handle potential data leaks
SpikingNetwork::~SpikingNetwork()
{
	//destroy every neuron that we own
	for(std::map<unsigned int, Neuron*>::iterator NeuronIter = Neurons.begin(); 
		NeuronIter != Neurons.end(); 
		NeuronIter++)
	{
		delete NeuronIter->second;
	}

	//and destroy the output lobe
	delete OutputLobe;
}

//TODO(aria): error codes here
//loads a mental state from a file
void SpikingNetwork::LoadMentalState(std::string StatePath, int* ErrCode, bool verbose)
{
	//try to open the file
	//TODO(aria): replace "/" with system path separator
	std::ifstream StateFile(
		StatePath + "/" + "MentalState.chr2",
		std::ios::binary | std::ios::in
	);
	//make sure that the file is open
	if (!StateFile)
	{
		//TODO(aria): error codes
		return;
	}

	//iterate through all of the internal neurons and store their states
	char StateBytes[12];
	for (std::map<unsigned int, Neuron*>::iterator NeuronIter = Neurons.begin();
		NeuronIter != Neurons.end();
		NeuronIter++)
	{
		StateFile.read(StateBytes, 12);
		NeuronIter->second->LoadStateFromBytes(StateBytes);
	}
	//and do the same for the output neurons
	for(std::vector<Neuron>::iterator NeuronIter = OutputNeurons.begin();
		NeuronIter != OutputNeurons.end();
		NeuronIter++)
	{
		StateFile.read(StateBytes, 12);
		NeuronIter->LoadStateFromBytes(StateBytes);
	}

	//close the file
	StateFile.close();
}
//TODO(aria): error codes here
//stores a mental state to a file
void SpikingNetwork::StoreMentalState(std::string StatePath, int* ErrCode, bool verbose) 
{
	//try to open the file
	//TODO(aria): replace "/" with system path separator
	std::ofstream StateFile(
		StatePath + "/" + "MentalState.chr2", 
		std::ios::binary | std::ios::out
	);
	//make sure that the file is open
	if(!StateFile)
	{
		//TODO(aria): error codes
		return;
	}

	//iterate through all of the internal neurons and write their states to the file
	for (std::map<unsigned int, Neuron*>::iterator NeuronIter = Neurons.begin();
		NeuronIter != Neurons.end();
		NeuronIter++) 
	{
		NeuronIter->second->WriteStateToFile(StateFile);
	}
	//and do the same for the output neurons
	for(std::vector<Neuron>::iterator NeuronIter = OutputNeurons.begin();
		NeuronIter != OutputNeurons.end();
		NeuronIter++)
	{
		NeuronIter->WriteStateToFile(StateFile);
	}

	//close the file
	StateFile.close();
}

//TODO(aria): error codes here
//saves the network to the genome at genomepath
void SpikingNetwork::SaveNetwork(std::string GenomePath, int* ErrCode, bool verbose)
{
	//try to open the files
	//TODO(aria): replace "/" with system path separator
	std::ofstream NeuronChromosome(
		GenomePath + "/" + "Neurons.chr2",
		std::ios::binary | std::ios::out
	);
	//TODO(aria): replace "/" with system path separator
	std::ofstream ConnectionChromosome(
		GenomePath + "/" + "Connections.chr2",
		std::ios::binary | std::ios::out
	);
	if (!NeuronChromosome)
	{
		//TODO(aria): error codes here
		throw std::invalid_argument("Failed to open Neuron Chromosome at path: " + GenomePath);
	}
	if (!ConnectionChromosome)
	{
		//TODO(aria): error codes here
		throw std::invalid_argument("Failed to open Connection Chromosome at path: " + GenomePath);
	}

	//write the lobe count and sizes to the neuron chromosome
	unsigned int LobeCount = Lobes.size();
	char LobeCountBytes[4] = { 
		(char)(LobeCount >> 24),
		(char)(LobeCount >> 16),
		(char)(LobeCount >> 8),
		(char)LobeCount
	};
	NeuronChromosome.write(LobeCountBytes, 4);
	delete[] &LobeCountBytes;
	for(std::map<unsigned int, Lobe>::iterator LobeIter = Lobes.begin(); 
		LobeIter != Lobes.end(); 
		LobeIter++)
	{
		LobeIter->second.WriteLobeToChromosome(NeuronChromosome);
	}

	//write the internal neurons and their connections to the respective chromosomes
	int NeuronIndex = 0;
	for(std::map<unsigned int, Neuron*>::iterator NeuronIter = Neurons.begin();
		NeuronIter != Neurons.end();
		NeuronIter++)
	{
		NeuronIter->second->WriteNeuronToFile(NeuronChromosome);
		NeuronIter->second->WriteConnectionsToFile(NeuronIndex++, 1, this, ConnectionChromosome);
	}
	//close the neuron chromosome
	NeuronChromosome.close();

	//write the output neurons connections to the connection chromosome
	NeuronIndex = 0;
	for(std::vector<Neuron>::iterator NeuronIter = OutputNeurons.begin();
		NeuronIter != OutputNeurons.end();
		NeuronIter++)
	{
		NeuronIter->WriteConnectionsToFile(NeuronIndex++, 2, this, ConnectionChromosome);
	}
	//close the connection chromosome
	ConnectionChromosome.close();
}

//the number of inputs, outputs, neurons and lobes in the network
unsigned int SpikingNetwork::InputCount()
{
	return InputNeurons.size();
}
unsigned int SpikingNetwork::OutputCount()
{
	return OutputNeurons.size();
}
unsigned int SpikingNetwork::NeuronCount()
{
	return Neurons.size();
}
unsigned int SpikingNetwork::LobeCount() 
{
	return Lobes.size();
}

//adds a lobe to the network
void SpikingNetwork::AddLobe()
{
	Lobes.emplace(std::make_pair(UniqueLobeIndex++, Lobe()));
}
//removes a lobe from the network
void SpikingNetwork::RemoveLobe(unsigned int Index)
{
	//get the lobe at index
	std::map<unsigned int, Lobe>::iterator LobeIter = Lobes.begin();
	std::advance(LobeIter, Index % Lobes.size());
	//remove it
	Lobes.erase(LobeIter);
}
//gets the number of neurons in a lobe
unsigned int SpikingNetwork::GetLobeNeuronCount(unsigned int Index)
{
	//get the lobe at index
	std::map<unsigned int, Lobe>::iterator LobeIter = Lobes.begin();
	std::advance(LobeIter, Index % Lobes.size());
	//return the number of neurons in the lobe
	return LobeIter->second.GetNeuronCount();
}

//adds a neuron to a lobe in the network and to the network
void SpikingNetwork::AddNeuronToLobe(unsigned int LobeID, Neuron NewNeuron, int* ErrCode, bool verbose) 
{
	//add the neuron to the network
	Neuron* NewNeuronPtr = new Neuron(NewNeuron);
	Neurons.emplace(UniqueNeuronIndex, NewNeuronPtr);

	//get the lobe at LobeID
	std::map<unsigned int, Lobe>::iterator LobeIter = Lobes.begin();
	std::advance(LobeIter, LobeID % Lobes.size());

	//add the neuron to the lobe
	LobeIter->second.AddNeuron(NewNeuronPtr);
}

//TODO(aria): error codes here
//adds a neuron to the network
void SpikingNetwork::AddNeuron(unsigned int ID, unsigned int Type, unsigned int ConnectionIndex, Neuron NewNeuron, int* ErrCode, bool verbose)
{
	//get the neuron and connection to add the new neuron into
	Neuron* NeuronPtr = GetNeuronPtr(ID, Type, ErrCode, verbose);
	Connection* ConnPtr = &(NeuronPtr->SourceConnections[ConnectionIndex % NeuronPtr->GetConnectionCount()]);

	//add the neuron to the neuron's lobe and the neuron map
	Neuron* NewNeuronPtr = new Neuron(NewNeuron);
	Neurons.emplace(UniqueNeuronIndex, NewNeuronPtr);
	NeuronPtr->ParentLobe->AddNeuron(Neurons[UniqueNeuronIndex]);

	//give the new neuron a connection to the original source neuron
	Neurons[UniqueNeuronIndex]->SourceConnections.push_back(Connection(ConnPtr->SourceNeuron, ConnPtr->SourceNeuronType, 1));
	//change the original neuron's source connection
	ConnPtr->SourceNeuron = UniqueNeuronIndex++;
}
//removes the nth neuron
void SpikingNetwork::RemoveNeuron(unsigned int ID, int* ErrCode, bool verbose) 
{
	//get the neuron to remove
	std::map<unsigned int, Neuron*>::iterator NeuronIter = Neurons.begin();
	std::advance(NeuronIter, ID % NeuronCount());

	//remove it from it's lobe and from the neuron map
	NeuronIter->second->ParentLobe->RemoveNeuron(NeuronIter->second);
	Neurons.erase(NeuronIter);
}
//adds a connection to the neuron
void SpikingNetwork::AddConnection(unsigned int ID, unsigned int Type, Connection NewConn, int* ErrCode, bool verbose) 
{
	//get the neuron and add the connection
	GetNeuronPtr(ID, Type, ErrCode, verbose)->AddConnection(NewConn);
}
//removes a connection from the neuron
void SpikingNetwork::RemoveConnection(unsigned int ID, unsigned int Type, unsigned int Index, int* ErrCode, bool verbose) 
{
	GetNeuronPtr(ID, Type, ErrCode, verbose)->RemoveConnection(Index);
}

//get the number of connections in a neuron
unsigned int SpikingNetwork::GetNeuronConnectionCount(unsigned int ID, unsigned int Type, int* ErrCode, bool verbose) 
{
	//is this an internal neuron?
	if(Type == 1)
	{
		//TODO(aria): error checks here
		//TODO(aria): error codes here
		//return the connection count for neuron at ID
		return Neurons[ID]->GetConnectionCount();
	}
	//is this an output neuron?
	else if (Type == 2) 
	{
		//TODO(aria): error checks here
		//TODO(aria): error codes here
		//return the connection count for the output neuron at index ID mod Output Count
		return OutputNeurons[ID % OutputNeurons.size()].GetConnectionCount();
	}
	else
	{
		//TODO(aria): error codes here
		//invalid type. return -1
		return (unsigned int)-1;
	}
}

//TODO(aria): error codes here
//sets the input values
void SpikingNetwork::SetInputs(std::vector<float> inputs, int* ErrCode, bool verbose)
{
	//iterate through the input neurons and input vector. end when we hit the end of either
	std::vector<float>::iterator  InputIter  = inputs.begin();
	std::vector<Neuron>::iterator NeuronIter = InputNeurons.begin();
	for (; NeuronIter != InputNeurons.end() || InputIter != inputs.end(); NeuronIter++, InputIter++) 
	{
		NeuronIter->SetValue(*InputIter);
	}
}
//gets the output values
std::vector<float> SpikingNetwork::GetOutputs(int* ErrCode, bool verbose) 
{
	//iterate through the output neurons and store their values in a vector
	std::vector<float> Values;
	for(std::vector<Neuron>::iterator NeuronIter = OutputNeurons.begin(); 
		NeuronIter != OutputNeurons.end();
		NeuronIter++)
	{
		Values.push_back(NeuronIter->GetValue());
	}
	
	//return the value vector
	return Values;
}
//performs an update on the network
void SpikingNetwork::PerformUpdate(int* ErrCode, bool verbose) 
{
	//iterate through the neurons and store their recalculated values
	std::stack<std::pair<float, unsigned long long int>> NeuronValues;
	for(std::map<unsigned int, Neuron*>::iterator NeuronIter = Neurons.begin(); 
		NeuronIter != Neurons.end(); 
		NeuronIter++)
	{
		NeuronValues.push(NeuronIter->second->CalculateNewValue(this));
	}
	//and do the same for the output values
	for(std::vector<Neuron>::iterator NeuronIter = OutputNeurons.begin(); 
		NeuronIter != OutputNeurons.end(); 
		NeuronIter++)
	{
		NeuronValues.push(NeuronIter->CalculateNewValue(this));
	}

	//set the values of the output neurons
	std::vector<Neuron>::iterator OutputNeuronIter = OutputNeurons.end();
	unsigned int NeuronIndex = 0;
	while(OutputNeuronIter != OutputNeurons.begin())
	{
		OutputNeuronIter--;

		//set the value
		OutputNeuronIter->SetState(NeuronValues.top());
		if (verbose)
		{
			std::cout << "Setting values of Output Neuron " << NeuronIndex << "to " << NeuronValues.top().first << ", " << NeuronValues.top().second << std::endl;
		}
		//remove it from the stack
		NeuronValues.pop();
		NeuronIndex++;
	}
	//and do the same for the rest of the neurons
	NeuronIndex = 0;
	std::map<unsigned int, Neuron*>::iterator NeuronIter = Neurons.end();
	while (NeuronIter != Neurons.begin())
	{
		NeuronIter--;

		//set the value
		NeuronIter->second->SetState(NeuronValues.top());
		if(verbose)
		{
			std::cout << "Setting values of Neuron " << NeuronIndex << "to " << NeuronValues.top().first << ", " << NeuronValues.top().second << std::endl;
		}

		//remove it from the stack
		NeuronValues.pop();
		NeuronIndex++;
	}

	//iterate through the lobes and do stdp
	unsigned int LobeIndex = 0;
	for(std::map<unsigned int, Lobe>::iterator LobeIter = Lobes.begin();
		LobeIter != Lobes.end();
		LobeIter++)
	{
		LobeIter->second.DoSTDP(this, ErrCode, verbose);
		if(verbose)
		{
			std::cout << "Doing STDP on lobe " << LobeIndex++ << std::endl;
		}
	}
	//do stdp for output lobe
	OutputLobe->DoSTDP(this, ErrCode, verbose);
}

//TODO(aria): error codes here
//returns a pointer to a neuron from an id and type
Neuron* SpikingNetwork::GetNeuronPtr(unsigned int ID, unsigned int Type, int* ErrCode, bool verbose)
{
	//TODO(aria): this shit is fucking stupid and i need to change it later

	//are we dealing with an input neuron?
	if(Type == 0)
	{
		//yes. check if ID is a valid input index
		if(InputNeurons.size() > ID)
		{
			//TODO(aria): error codes here
			//valid. return a pointer
			return &(InputNeurons[ID]);
		}
		else 
		{
			//TODO(aria): error codes here
			//invalid. return a null pointer
			return (Neuron*)nullptr;
		}
	}
	//are we dealing with an internal neuron
	else if(Type == 1)
	{
		//yes. check if ID is a valid internal id
		if(Neurons.find(ID) != Neurons.end())
		{
			//TODO(aria): error codes here
			//valid. return a pointer
			return Neurons[ID];
		}
		else 
		{
			//TODO(aria): error codes here
			//invalid. return a null pointer
			return (Neuron*)nullptr;
		}
	}
	else if(Type == 2)
	{
		//yes. check if ID is a valid output index
		if(OutputNeurons.size() > ID)
		{
			//TODO(aria): error codes here
			//valid. return a pointer
			return &(OutputNeurons[ID]);
		}
		else
		{
			//TODO(aria): error codes here
			//invalid. return a null pointer
			return (Neuron*)nullptr;
		}
	}
	else
	{
		//TODO(aria): error codes here
		//invalid. return a null pointer
		return (Neuron*)nullptr;
	}
}
//returns the neuron's id(or index if input/output)
unsigned int SpikingNetwork::GetNeuronID(unsigned int Index, unsigned int Type, int* ErrCode, bool verbose)
{
	//are we dealing with an input neuron?
	if(Type == 0)
	{
		//return index mod input count
		return Index % InputCount();
	}
	//are we dealing with an internal neuron?
	else if(Type == 1)
	{
		//TODO(aria): error codes here
		//skip forward index mod neuron count places from the first neuron and return the id
		std::map<unsigned int, Neuron*>::iterator NeuronIter = Neurons.begin();
		std::advance(NeuronIter, Index % NeuronCount());
		return NeuronIter->first;
	}
	//are we dealing with an output neuron?
	else if(Type == 2)
	{
		//return index mod output count
		return Index % OutputCount();
	}
	else
	{
		//TODO(aria): error codes here
		//invalid type. return -1
		return (unsigned int)-1;
	}
}
//returns the neuron's index
unsigned int SpikingNetwork::GetNeuronIndex(unsigned int ID, unsigned int Type, int* ErrCode, bool verbose) 
{
	//is it an input neuron?
	if(Type == 0)
	{
		//yes. return id mod input count
		return ID % InputCount();
	}
	//is it an internal neuron?
	else if(Type == 1)
	{
		//yes. iterate through the neuron map until we find the neuron
		std::map<unsigned int, Neuron*>::iterator NeuronIter = Neurons.begin();
		Neuron* Match = GetNeuronPtr(ID, Type, ErrCode, verbose);
		unsigned int i = 0;
		while(NeuronIter != Neurons.end())
		{
			//if we found it, return i
			if (NeuronIter->second == Match)
				return i;

			//we didn't. move to the next neuron in the map
			i++;
			NeuronIter++;
		}
	}
	//is it an output neuron?
	else if(Type == 2)
	{
		//yes. return id mod output count
		return ID % OutputCount();
	}
	else
	{
		//TODO(aria): error codes here
		//invalid type. return -1
		return (unsigned int)-1;
	}
}

//TODO(aria): error codes here
//sets the parameters of the internal neuron to those in Params
void SpikingNetwork::SetNeuronParams(unsigned int ID, NeuronParams Params, int* ErrCode, bool verbose)
{
	//TODO(aria): error codes here
	//set the neuron's params
	GetNeuronPtr(ID, 1, ErrCode, verbose)->SetParams(Params);
}
//TODO(aria): error codes here
//gets the parameters of the internal neuron
NeuronParams SpikingNetwork::GetNeuronParams(unsigned int ID, int* ErrCode, bool verbose)
{
	//TODO(aria): error codes here
	return GetNeuronPtr(ID, 1, ErrCode, verbose)->GetParams();
}

//sets the parameters of a connection in a neuron
void SpikingNetwork::SetConnectionParams(
	unsigned int ID,
	unsigned int Type,
	unsigned int index,
	ConnectionParams Params,
	int* ErrCode,
	bool verbose) 
{
	//set the parameters
	GetNeuronPtr(ID, Type, ErrCode, verbose)->SetConnectionParams(index, this, Params, ErrCode, verbose);
}
//gets the parameters of a connection in a neuron
ConnectionParams SpikingNetwork::GetConnectionParams(
	unsigned int ID,
	unsigned int Type,
	unsigned int index,
	int* ErrCode,
	bool verbose) 
{
	//return the parameters
	return GetNeuronPtr(ID, Type, ErrCode, verbose)->GetConnectionParams(index, this, ErrCode, verbose);
}