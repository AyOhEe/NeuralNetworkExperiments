#ifndef FREQUENCY_CONVERTER_H
#define FREQUENCY_CONVERTER_H

#include <CL/cl.hpp>
#include <deque>

#include "CLUtil.h"

//the actual manager for OpenCL and frequency conversion
class __FrequencyConverter
{
	//OpenCL Variables
	cl::Context CLContext;
	cl::Kernel FrequencyConvertKernel;
	cl::Kernel OutputConvertKernel;
	cl::CommandQueue CLCommandQueue;

public:
	//creates the __FrequencyConverter
	__FrequencyConverter();

	//returns the values for input frequencies 
	std::vector<float> FreqToOutput(std::vector<float> &Frequencies, unsigned long long UpdateStep);

	//returns the most prevalent frequencies for the output histories
	std::vector<float> OutputHistToFreq(std::vector<std::deque<float>*> &History);
};

//the interface class for the frequency converter
class FrequencyConverter
{
	static __FrequencyConverter* Converter;

	//current update step
	unsigned long long UpdateStep;

public:
	//creates the FrequencyConverter
	FrequencyConverter();

	//returns the values for input frequencies 
	std::vector<float> FreqToOutput(std::vector<float>& Frequencies);

	//returns the most prevalent frequencies for the output histories
	std::vector<float> OutputHistToFreq(std::vector<std::deque<float>*>& History);

	//increments updatestep by 1
	void TriggerUpdate();
};

#endif