#include "SpikingNetwork.h"

#include <iostream>
#include <vector>
#include <CL/cl.hpp>

int main()
{
	//create a new handler instance
	STDPHandler Handler = STDPHandler();

	//dummy data
	std::vector<unsigned long long> TargetTimes = {0, 11, 50, 0, 5};
	std::vector<unsigned long long> SourceTimes = {5, 0, 25, 11, 0};
	std::vector<float> Weights = {2.0f, 0.5f, 3.0f, 3.5f, 1.75f};

	//run stdp
	std::vector<float> NewWeights = Handler.DoSTDP(SourceTimes, TargetTimes, Weights);
	//output the vector contents
	for(int i = 0; i < 5; i++)
	{
		std::cout << "Old Weight: " << Weights[i] << " \n\tNew Weight: " << NewWeights[i] << "\n" << std::endl;
	}

	//TODO(aria): Write SpikingNetwork tests
}