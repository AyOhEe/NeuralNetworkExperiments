#include "../Include/SpikingNetwork.h"

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
		Lobes.insert(std::make_pair(UniqueLobeIndex++, Lobe()));
		Lobe* CurrentLobe = &(Lobes[Lobes.size()]);

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
			Neurons.insert(std::make_pair(UniqueNeuronIndex, new Neuron(Bytes)));
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

	//TODO(aria): inputs and outputs here

	//read the connection chromosome
	Bytes = new unsigned char[13];
	while (!ConnectionChromosome.eof()) 
	{
		//read in the gene
		ConnectionChromosome.read((char*)Bytes, 13);
		//create the connection
		Connection::CreateConnection(Bytes, this);
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
	char NeuronStateBytes[12];
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
		(LobeCount >> 24) & 0xff,
		(LobeCount >> 16) & 0xff,
		(LobeCount >> 8)  & 0xff,
		LobeCount & 0xff 
	};
	NeuronChromosome.write(LobeCountBytes, 4);
	delete[] LobeCountBytes;
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

//the number of inputs, outputs and neurons in the network
int SpikingNetwork::InputCount()
{
	return InputNeurons.size();
}
int SpikingNetwork::OutputCount() 
{
	return OutputNeurons.size();
}
int SpikingNetwork::NeuronCount()
{
	return Neurons.size();
}

//TODO(aria): error codes here
//adds a neuron to the network
/*void SpikingNetwork::AddNeuron(unsigned int NeuronIndex, unsigned int ConnectionIndex, Neuron NewNeuron, int* ErrCode, bool verbose)
{

}
//removes the nth neuron
void SpikingNetwork::RemoveNeuron(unsigned int Index, int* ErrCode, bool verbose) 
{

}/**/
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
	for(std::vector<Neuron>::iterator NeuronIter = OutputNeurons.end();
		NeuronIter != OutputNeurons.end();
		NeuronIter--)
	{
		//set the value
		NeuronIter->SetState(NeuronValues.top());
		//remove it from the stack
		NeuronValues.pop();
	}
	//and do the same for the rest of the neurons
	for (std::map<unsigned int, Neuron*>::iterator NeuronIter = Neurons.begin();
		NeuronIter != Neurons.end();
		NeuronIter--)
	{
		//set the value
		NeuronIter->second->SetState(NeuronValues.top());
		//remove it from the stack
		NeuronValues.pop();
	}
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