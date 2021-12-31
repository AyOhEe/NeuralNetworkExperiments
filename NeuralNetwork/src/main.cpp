#include "../Include/Network.h"
#include "../Include/BinaryReader.h"

#include <math.h>
#include <iostream>
#include <vector>

int main()
{
	//create a network
	Network* network = new Network("Genomes/test_genome.genome", 2, 1, 3);

	std::vector<std::vector<float>> Inputs = {
		{0, 0},
		{0, 1},
		{1, 0},
		{1, 1}
	};
	std::vector<float> Results;

	network->SetInputs(Inputs[0]);
	Results = network->GetResults();
	std::cout << Results[0] << std::endl;

	network->SetInputs(Inputs[1]);
	Results = network->GetResults();
	std::cout << Results[0] << std::endl;

	network->SetInputs(Inputs[2]);
	Results = network->GetResults();
	std::cout << Results[0] << std::endl;

	network->SetInputs(Inputs[3]);
	Results = network->GetResults();
	std::cout << Results[0] << std::endl;
    
    return 0;
} 