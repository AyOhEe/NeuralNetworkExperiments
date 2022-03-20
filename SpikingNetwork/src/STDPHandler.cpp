#include <STDPHandler.h>

void errCheck(cl_int errCode, int l)
{
	int line = l;
	_ASSERT(errCode == 0);
}

namespace
{
	//creates a program in opencl from the source file at file
	cl::Program CreateProgram(std::string file)
	{
		/*-----GET DEVICE FOR USE-----*/
		//get all platforms supported on this computer
		std::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);

		//get the gpu devices in the first platform
		cl::Platform platform = platforms.front();
		std::vector<cl::Device> devices;
		errCheck(platform.getDevices(CL_DEVICE_TYPE_GPU, &devices), __LINE__);

		/*-----KERNEL IMPORT-----*/
		//get the .cl file as a string
		std::ifstream helloWorldFile(file);
		std::string src(std::istreambuf_iterator<char>(helloWorldFile), (std::istreambuf_iterator<char>()));

		//define program sources
		cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));

		//make the context and program from the device and sources
		cl::Context context(devices);
		cl::Program program(context, sources);

		//build the program and output the build log
		errCheck(program.build("-cl-std=CL1.2"), __LINE__);

		//done!
		return program;
	}
};

//creates the __STDPHandler
__STDPHandler::__STDPHandler() 
{
	//create the program
	cl::Program Program = CreateProgram("STDP.cl");

	//get context and device
	CLContext = Program.getInfo<CL_PROGRAM_CONTEXT>();
	std::vector<cl::Device> devices = Program.getInfo<CL_PROGRAM_DEVICES>();
	cl::Device device = devices.front();

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
	//make sure the source and target times and weights all have the same length
	if (!(SourceTimes.size() == TargetTimes.size() && SourceTimes.size() == Weights.size()))
		throw std::invalid_argument("STDPHandler::DoSTDP: SourceTimes, TargetTimes and Weights must match in size!");
		
	//vector to store the new weights in
	std::vector<float> NewWeights = std::vector<float>(Weights.size());
	//vector to store the weight deltas in
	std::vector<float> Deltas = std::vector<float>(Weights.size());

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
	errCheck(CLCommandQueue.enqueueReadBuffer(NewWeightsBuffer, CL_FALSE, 0, sizeof(float) * NewWeights.size(), NewWeights.data()), __LINE__);
	errCheck(CLCommandQueue.finish(), __LINE__);

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