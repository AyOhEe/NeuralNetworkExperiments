#include <STDPHandler.h>

void errCheck(cl_int errCode, int l)
{
	int line = l;
	_ASSERT(errCode == 0);
}

//creates the __STDPHandler
__STDPHandler::__STDPHandler() 
{
	//get all platforms supported on this computer
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	//get the gpu devices in the first platform
	cl::Platform platform = platforms.front();
	std::vector<cl::Device> devices;
	errCheck(platform.getDevices(CL_DEVICE_TYPE_GPU, &devices), __LINE__);

	//get the .cl file as a string
	std::ifstream programFile("STDP.cl");
	std::string src(std::istreambuf_iterator<char>(programFile), (std::istreambuf_iterator<char>()));

	//define program sources
	cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));

	//make the context and program from the device and sources
	CLContext =  cl::Context(devices);
	cl::Program program(CLContext, sources);

	//build the program and store the error code
	errCheck(program.build("-cl-std=CL1.2"), __LINE__);

	//get device to use
	cl::Device device = devices.front();


	//create the kernel
	STDPKernel = cl::Kernel(program, "Main");
	//create the command queue
	CLCommandQueue = cl::CommandQueue(CLContext, device);
}

//returns values in weights processed by STDP
std::vector<float> __STDPHandler::DoSTDP(
	std::vector<unsigned long long>& SourceTimes,
	std::vector<unsigned long long>& TargetTimes,
	std::vector<float>& Weights) 
{
	//make sure the source and target times and weights all have the same length
	if (!(SourceTimes.size() == TargetTimes.size() && SourceTimes.size() == Weights.size()))
		throw std::invalid_argument("STDPHandler::DoSTDP: SourceTimes, TargetTimes and Weights must match in size!");

	//create the buffers
	cl::Buffer SourceTimeBuffer(
		CLContext,
		CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(unsigned long long) * SourceTimes.size(),
		SourceTimes.data()
	);
	cl::Buffer TargetTimeBuffer(
		CLContext,
		CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(unsigned long long) * TargetTimes.size(),
		TargetTimes.data()
	);
	cl::Buffer WeightsBuffer(
		CLContext,
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		sizeof(float) * Weights.size(),
		Weights.data()
	);

	//set kernel args
	STDPKernel.setArg(0, SourceTimeBuffer);
	STDPKernel.setArg(1, TargetTimeBuffer);
	STDPKernel.setArg(2, WeightsBuffer);

	//run the kernel
	CLCommandQueue.enqueueNDRangeKernel(STDPKernel, cl::NullRange, cl::NDRange(SourceTimes.size()));
	//wait for it to finish
	CLCommandQueue.finish();
	//read back the results
	std::vector<float> NewWeights(Weights.size());
	CLCommandQueue.enqueueReadBuffer(WeightsBuffer, CL_TRUE, 0, sizeof(float) * NewWeights.size(), NewWeights.data());
	
	//return the new weights
	return NewWeights;
}

//the singleton handler instance 
__STDPHandler* STDPHandler::Handler = NULL;

//creates the STDPHandler
STDPHandler::STDPHandler() 
{
	//if we don't have a handler interface, make one
	if (Handler == NULL) 
		Handler = new __STDPHandler();
}

//returns values in weights processed by STDP
std::vector<float> STDPHandler::DoSTDP(
	std::vector<unsigned long long>& SourceTimes,
	std::vector<unsigned long long>& TargetTimes,
	std::vector<float>& Weights) 
{
	//return the handler's result
	return Handler->DoSTDP(SourceTimes, TargetTimes, Weights);
}