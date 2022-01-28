#include "../Include/Network.h"
#include "../Include/ErrorCodes.h"

#include <math.h>
#include <iostream>
#include <vector>

#define ASSERT_ERR(err) _ASSERT(err == SUCCESS)

int main()
{
	//Network::BreedSettings Settings;
	//Settings.CrossoverPoints = { 0.5 };
	//Settings.MutationChance = 500;
	//Network* network = new Network("Genomes/test_genome", "Genomes/test_genome_smallMOD", Settings, 2, 1, 0, true);

	//create and copy the networks
	unsigned int Err = 0;
	Network* network = new Network(std::string("Genomes/test_genome"), 2, 1, 0, &Err, true);
	Network* networkCopy = new Network(*network);
	ASSERT_ERR(Err);

	//inputs and outputs
	std::vector<std::vector<float>> Inputs = {
		{0, 0},
		{0, 1},
		{1, 0},
		{1, 1}
	};
	std::vector<float> Results;

	//network tests
	network->SetInputs(Inputs[0], &Err);
	ASSERT_ERR(Err);
	Results = network->GetResults(&Err);
	ASSERT_ERR(Err);
	std::cout << Results[0] << std::endl;

	network->SetInputs(Inputs[1], &Err);
	ASSERT_ERR(Err);
	Results = network->GetResults(&Err);
	ASSERT_ERR(Err);
	std::cout << Results[0] << std::endl;

	network->SetInputs(Inputs[2], &Err);
	ASSERT_ERR(Err);
	Results = network->GetResults(&Err);
	ASSERT_ERR(Err);
	std::cout << Results[0] << std::endl;

	network->SetInputs(Inputs[3], &Err);
	ASSERT_ERR(Err);
	Results = network->GetResults(&Err);
	ASSERT_ERR(Err);
	std::cout << Results[0] << std::endl;

	//network copy tests
	networkCopy->SetInputs(Inputs[0], &Err);
	ASSERT_ERR(Err);
	Results = networkCopy->GetResults(&Err);
	ASSERT_ERR(Err);
	std::cout << Results[0] << std::endl;

	networkCopy->SetInputs(Inputs[1], &Err);
	ASSERT_ERR(Err);
	Results = networkCopy->GetResults(&Err);
	ASSERT_ERR(Err);
	std::cout << Results[0] << std::endl;

	networkCopy->SetInputs(Inputs[2], &Err);
	ASSERT_ERR(Err);
	Results = networkCopy->GetResults(&Err);
	ASSERT_ERR(Err);
	std::cout << Results[0] << std::endl;

	networkCopy->SetInputs(Inputs[3], &Err);
	ASSERT_ERR(Err);
	Results = networkCopy->GetResults(&Err);
	ASSERT_ERR(Err);
	std::cout << Results[0] << std::endl;
    
    return 0;
} 