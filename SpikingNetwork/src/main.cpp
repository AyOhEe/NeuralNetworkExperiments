#include "STDPHandler.h"

#include <iostream>
#include <vector>

int main()
{
	//create a new handler instance
	STDPHandler Handler = STDPHandler();

	//dummy data
	std::vector<unsigned long long> TargetTimes = {0, 11, 50, 0, 5};
	std::vector<unsigned long long> SourceTimes = {5, 0, 25, 11, 0};
	std::vector<float> Weights = {2, -1.5, 3, 1.25, -1.75};

	//run stdp
	std::vector<float> NewWeights = Handler.DoSTDP(SourceTimes, TargetTimes, Weights);
	//output the vector contents
	for(int i = 0; i < 5; i++)
	{
		std::cout << "Old Weight: " << Weights[i] << " New Weight: " << NewWeights[i] << std::endl;
	}
}