#include "Include/Network.h"
#include <math.h>

//returns sigmoid(x)
float Sigmoid(float x) 
{
	return 1 / (1 + pow(2.718, -x));
}

int main()
{
	//create a network
	Network network = Network("Genomes/test_genome_rand.genome", 2, 1, Sigmoid);

	//open and save the network 500 times to see if the data gets warped at all
	for (int i = 0; i < 500; i++) 
	{
		network.SaveNetwork("Genomes/test_genome_rand_cpy.genome");
		network = Network("Genomes/test_genome_rand_cpy.genome", 65565, 65565, Sigmoid);
		std::cout << i << "/500" << std::endl;
	}

	/*std::vector<std::vector<float>> Inputs = {
		{0, 0},
		{0, 1},
		{1, 0},
		{1, 1}
	};
	std::vector<float> Results;

	network.SetInputs(Inputs[0]);
	Results = network.GetResults();
	std::cout << Results[0] << std::endl;

	network.SetInputs(Inputs[1]);
	Results = network.GetResults();
	std::cout << Results[0] << std::endl;

	network.SetInputs(Inputs[2]);
	Results = network.GetResults();
	std::cout << Results[0] << std::endl;

	network.SetInputs(Inputs[3]);
	Results = network.GetResults();
	std::cout << Results[0] << std::endl;*/
    
    return 0;
} 