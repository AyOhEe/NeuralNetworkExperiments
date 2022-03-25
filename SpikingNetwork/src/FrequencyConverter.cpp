#include <FrequencyConverter.h>

using namespace CLUtil;

//creates the __STDPHandler
__FrequencyConverter::__FrequencyConverter(bool verbose) 
{
	//create the program
	cl::Program FrequencyProgram = CreateProgram("FrequencyToOutput.cl");

	//get context and device
	CLContext = FrequencyProgram.getInfo<CL_PROGRAM_CONTEXT>();
	std::vector<cl::Device> devices = FrequencyProgram.getInfo<CL_PROGRAM_DEVICES>();
	cl::Device device = devices.front();

	//create the other program with the same context
	cl::Program OutputProgram = CreateProgram("OutputToFrequency.cl", CLContext);

	if(verbose)
	{
		//output build logs
		std::cout << "FrequencyProgram Build Logs" << std::endl;
		std::cout << FrequencyProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
		std::cout << "OutputProgram Build Logs" << std::endl;
		std::cout << OutputProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
	}

	//create the kernels
	FrequencyConvertKernel = cl::Kernel(FrequencyProgram, "Main");
	OutputConvertKernel = cl::Kernel(OutputProgram, "Main");
	//create the command queue
	CLCommandQueue = cl::CommandQueue(CLContext, device);
}

//returns the values for input frequencies 
std::vector<float> __FrequencyConverter::FreqToOutput(std::vector<float>& Frequencies, unsigned long long UpdateStep)
{
	//make sure we're being fed valid input
	if(Frequencies.size() == 0)
	{
		throw std::invalid_argument("FrequencyConverter::FreqToOutput: Frequencies must not be empty!");
	}

	//vector to store the outputs in
	std::vector<float> Outputs = std::vector<float>(Frequencies.size());

	//create the buffers
	cl::Buffer FrequencyBuffer = cl::Buffer(
		CLContext,
		CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(float) * Frequencies.size(),
		Frequencies.data()
	);
	cl::Buffer OutputBuffer = cl::Buffer(
		CLContext,
		CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(float) * Outputs.size(),
		Outputs.data()
	);

	//set the arguments
	cl_ulong cl_UpdateStep = UpdateStep;
	errCheck(FrequencyConvertKernel.setArg(0, FrequencyBuffer), __LINE__);
	errCheck(FrequencyConvertKernel.setArg(1, OutputBuffer), __LINE__);
	errCheck(FrequencyConvertKernel.setArg(2, cl_UpdateStep), __LINE__);

	//run the kernel
	errCheck(CLCommandQueue.enqueueNDRangeKernel(FrequencyConvertKernel, cl::NullRange, cl::NDRange(Frequencies.size())), __LINE__);
	errCheck(CLCommandQueue.finish(), __LINE__);
	//read back the results
	errCheck(CLCommandQueue.enqueueReadBuffer(OutputBuffer, CL_TRUE, 0, sizeof(float) * Outputs.size(), Outputs.data()), __LINE__);

	//return the output values
	return Outputs;
}

//returns the most prevalent frequencies for the output histories
std::vector<float> __FrequencyConverter::OutputHistToFreq(std::vector<std::deque<float>*>& History, unsigned int NFrequencyBands) 
{
	//make sure we're being fed valid input
	if (History.size() == 0)
	{
		throw std::invalid_argument("FrequencyConverter::OutputHistToFreq: History must not be empty!");
	}
	for(std::vector<std::deque<float>*>::iterator HistoryIter = History.begin();
		HistoryIter != History.end();
		HistoryIter++)
	{
		if((*HistoryIter)->size() == 0)
		{
			throw std::invalid_argument("FrequencyConverter::OutputHistToFreq: History must not contain empty deques!");
		}
	}

	//vector to store the frequencies
	std::vector<float> Frequencies = std::vector<float>(History.size());
	//iterate through the histories
	for (std::vector<std::deque<float>*>::iterator HistoryIter = History.begin();
		HistoryIter != History.end();
		HistoryIter++)
	{
		//copy the deque data into a vector
		std::vector<float> HistoryVector = std::vector<float>(
			(*HistoryIter)->begin(),
			(*HistoryIter)->end()
		);

		//create the buffers
		cl::Buffer HistoryBuffer(
			CLContext,
			CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,
			sizeof(float) * HistoryVector.size(),
			HistoryVector.data()
		);
		cl::Buffer WorkingBuffer(
			CLContext,
			CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS | CL_MEM_ALLOC_HOST_PTR,
			sizeof(float) * NFrequencyBands,
			nullptr
		);

		//set kernel args
		cl_float Result = 0; //where the final result will be stored
		errCheck(OutputConvertKernel.setArg(0, HistoryBuffer), __LINE__);
		errCheck(OutputConvertKernel.setArg(1, WorkingBuffer), __LINE__);
		errCheck(OutputConvertKernel.setArg(2, Result), __LINE__);

		//run the kernel
		errCheck(CLCommandQueue.enqueueNDRangeKernel(
			OutputConvertKernel, 
			cl::NullRange, 
			cl::NDRange(HistoryVector.size(), NFrequencyBands)), __LINE__
		);
		errCheck(CLCommandQueue.finish(), __LINE__);

		//append the result to the frequency vector
		Frequencies.push_back(Result);
	}

	//return the frequencies
	return Frequencies;
}


__FrequencyConverter* FrequencyConverter::Converter = (__FrequencyConverter*)nullptr;

//creates the FrequencyConverter
FrequencyConverter::FrequencyConverter(bool verbose) 
{
	//if we don't have a frequency converter, create one
	if (Converter == (__FrequencyConverter*)nullptr)
		Converter = new __FrequencyConverter(verbose);
}

//returns the values for input frequencies 
std::vector<float> FrequencyConverter::FreqToOutput(std::vector<float>& Frequencies) 
{
	return Converter->FreqToOutput(Frequencies, UpdateStep);
}

//returns the most prevalent frequencies for the output histories
std::vector<float> FrequencyConverter::OutputHistToFreq(std::vector<std::deque<float>*>& History, unsigned int NFrequencyBands)
{
	return Converter->OutputHistToFreq(History, NFrequencyBands);
}

//increments updatestep by 1
void FrequencyConverter::TriggerUpdate() 
{
	UpdateStep++;
}