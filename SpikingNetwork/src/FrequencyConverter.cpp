#include <FrequencyConverter.h>

using namespace CLUtil;

//creates the __STDPHandler
__FrequencyConverter::__FrequencyConverter() 
{
	//create the program
	cl::Program FrequencyProgram = CreateProgram("FrequencyToOutput.cl");

	//get context and device
	CLContext = FrequencyProgram.getInfo<CL_PROGRAM_CONTEXT>();
	std::vector<cl::Device> devices = FrequencyProgram.getInfo<CL_PROGRAM_DEVICES>();
	cl::Device device = devices.front();

	//create the other program with the same context
	cl::Program OutputProgram = CreateProgram("OutputToFrequency.cl", CLContext);

	//create the kernels
	FrequencyConvertKernel = cl::Kernel(FrequencyProgram, "Main");
	OutputConvertKernel = cl::Kernel(OutputProgram, "Main");
	//create the command queue
	CLCommandQueue = cl::CommandQueue(CLContext, device);
}

//returns the values for input frequencies 
std::vector<float> __FrequencyConverter::FreqToOutput(std::vector<float>& Frequencies, unsigned long long UpdateStep)
{

}

//returns the most prevalent frequencies for the output histories
std::vector<float> __FrequencyConverter::OutputHistToFreq(std::vector<std::deque<float>*>& History) 
{

}


__FrequencyConverter* FrequencyConverter::Converter = (__FrequencyConverter*)nullptr;

//creates the FrequencyConverter
FrequencyConverter::FrequencyConverter() 
{
	//if we don't have a frequency converter, create one
	if (Converter == (__FrequencyConverter*)nullptr)
		Converter = new __FrequencyConverter();
}

//returns the values for input frequencies 
std::vector<float> FrequencyConverter::FreqToOutput(std::vector<float>& Frequencies) 
{
	return Converter->FreqToOutput(Frequencies, UpdateStep);
}

//returns the most prevalent frequencies for the output histories
std::vector<float> FrequencyConverter::OutputHistToFreq(std::vector<std::deque<float>*>& History) 
{
	return Converter->OutputHistToFreq(History);
}

//increments updatestep by 1
void FrequencyConverter::TriggerUpdate() 
{
	UpdateStep++;
}