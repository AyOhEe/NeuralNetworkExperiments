#include "../Include/Network.h"
#include <math.h>

//returns sigmoid(x)
float Sigmoid(float x) 
{
	return 1 / (1 + pow(2.718, -x));
}

int main()
{
	//create a network
	Network* network = new Network("Genomes/test_genome_smallMOD.genome", 2, 1, Sigmoid);

	/*//open and save the network 500 times to see if the data gets warped at all
	for (int i = 1; i < 101; i++) 
	{
		network->SaveNetwork("Genomes/test_genome_rand_cpy.genome");
		delete network;
		network = new Network("Genomes/test_genome_rand_cpy.genome", 2, 1, Sigmoid);
		std::cout << i << "/100" << std::endl;
	}*/

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