#include <STDPHandler.h>

using namespace CLUtil;

//creates the __STDPHandler
__STDPHandler::__STDPHandler() 
{
	//create the program
	std::pair<cl_int, cl::Program> ProgramPair = CreateProgram("STDP.cl");
	cl::Program Program = ProgramPair.second;

	//get context and device
	CLContext = Program.getInfo<CL_PROGRAM_CONTEXT>();
	std::vector<cl::Device> devices = Program.getInfo<CL_PROGRAM_DEVICES>();
	cl::Device device = devices.front();

	//output build logs if the build failed
	if (ProgramPair.first == -11)
	{
		std::cout << "STDPHandler Build Logs" << std::endl;
		std::cout << Program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
		throw std::exception();
	}
	errCheck(ProgramPair.first, __LINE__);

	//create the kernel
	STDPKernel = cl::Kernel(Program, "Main");
	//create the command queue
	CLCommandQueue = cl::CommandQueue(CLContext, device);
}

//returns values in weights processed by STDP
std::vector<float> __STDPHandler::DoSTDP(
	std::vector<unsigned long long>& SourceTimes,
	std::vector<unsigned long long>& TargetTimes,
	std::vector<float>& Weights) 
{
	//make sure the source and target times and weights all have the same, non-zero length
	if (!(SourceTimes.size() == TargetTimes.size() && SourceTimes.size() == Weights.size())) 
	{
		throw std::invalid_argument("STDPHandler::DoSTDP: SourceTimes, TargetTimes and Weights must match in size!");
	}
	if (SourceTimes.size() == 0)
	{
		throw std::invalid_argument("STDPHandler::DoSTDP: Must be given data! Was given vectors of length 0!");
	}
		
	//vector to store the new weights in
	std::vector<float> NewWeights = std::vector<float>(Weights.size());

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
		CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(float) * Weights.size(),
		Weights.data()
	);
	cl::Buffer NewWeightsBuffer(
		CLContext,
		CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(float) * NewWeights.size(),
		NewWeights.data()
	); 

	//set kernel args
	errCheck(STDPKernel.setArg(0, SourceTimeBuffer), __LINE__);
	errCheck(STDPKernel.setArg(1, TargetTimeBuffer), __LINE__);
	errCheck(STDPKernel.setArg(2, WeightsBuffer), __LINE__);
	errCheck(STDPKernel.setArg(3, NewWeightsBuffer), __LINE__);

	//run the kernel
	errCheck(CLCommandQueue.enqueueNDRangeKernel(STDPKernel, cl::NullRange, cl::NDRange(SourceTimes.size())), __LINE__);
	errCheck(CLCommandQueue.finish(), __LINE__);
	//read back the results
	errCheck(CLCommandQueue.enqueueReadBuffer(NewWeightsBuffer, CL_TRUE, 0, sizeof(float) * NewWeights.size(), NewWeights.data()), __LINE__);
	//errCheck(CLCommandQueue.finish(), __LINE__);

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