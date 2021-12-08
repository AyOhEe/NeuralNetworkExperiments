#include "Include/Network.h"
#include <math.h>

//returns sigmoid(x)
float Sigmoid(float x) 
{
	return 1 / (1 + pow(2.718, -x));
}

int main()
{
	Network network;
	try 
	{
		network = Network("Genomes/test_genome_smallMOD.genome", 2, 1, Sigmoid);
	}
	catch (std::exception &exception)
	{
		std::cout << exception.what() << std::endl << std::endl;
		return -1;
	}

	std::vector<std::vector<float>> Inputs = {
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
	std::cout << Results[0] << std::endl;
    
    return 0;
} 