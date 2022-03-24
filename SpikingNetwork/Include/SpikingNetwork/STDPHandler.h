#ifndef STDP_HANDLER_H
#define STDP_HANDLER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <CL/cl.hpp>

//the actual manager for OpenCL and STDP
class __STDPHandler
{
	//OpenCL Variables
	cl::Context CLContext;
	cl::Kernel STDPKernel;
	cl::CommandQueue CLCommandQueue;

public:
	//creates the __STDPHandler
	__STDPHandler();

	//returns values in weights processed by STDP
	std::vector<float> DoSTDP(
		std::vector<unsigned long long>& SourceTimes,
		std::vector<unsigned long long>& TargetTimes,
		std::vector<float>& Weights);
};

//the interface class for the STDP handler
class STDPHandler
{
	//the singleton handler instance 
	static __STDPHandler* Handler;

public:
	//creates the STDPHandler
	STDPHandler();

	//returns values in weights processed by STDP
	std::vector<float> DoSTDP(
		std::vector<unsigned long long>& SourceTimes,
		std::vector<unsigned long long>& TargetTimes,
		std::vector<float>& Weights);
};

#endif