#include <HebianConnector.h>

using namespace CLUtil;

//creates the __HebianConnector
__HebianConnector::__HebianConnector() 
{
	//create the program
	//TODO(aria): Make HebianConnector.cl
	std::pair<cl_int, cl::Program> ProgramPair = CreateProgram("HebianConnector.cl");
	cl::Program Program = ProgramPair.second;

	//get context and device
	CLContext = Program.getInfo<CL_PROGRAM_CONTEXT>();
	std::vector<cl::Device> devices = Program.getInfo<CL_PROGRAM_DEVICES>();
	cl::Device device = devices.front();

	//output build logs if the build failed
	if (ProgramPair.first == -11)
	{
		std::cout << "HebianConnector Build Logs" << std::endl;
		std::cout << Program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
		throw std::exception();
	}
	errCheck(ProgramPair.first, __LINE__);

	//create the kernel
	HebConnKernel = cl::Kernel(Program, "Main");
	//create the command queue
	CLCommandQueue = cl::CommandQueue(CLContext, device);
}

/*//returns what connections should be made
std::vector<float> __HebianConnector::DoHebianConnection(
	std::vector<bool> LobeASpikeHistory,
	std::vector<bool> LobeBSpikeHistory,
	unsigned int LobeANeuronCount,
	unsigned int LobeBNeuronCount,
	std::vector<float> ConnectionWeights
)
{

}/**/

//the singleton handler instance 
__HebianConnector* HebianConnector::Connector = NULL;

//creates the HebianConnector
HebianConnector::HebianConnector() 
{
	//if we don't have a handler interface, make one
	if (Connector == NULL)
		Connector = new __HebianConnector();
}

//returns what connections should be made
/*std::vector<float> HebianConnector::DoHebianConnection(
	std::vector<bool> LobeASpikeHistory,
	std::vector<bool> LobeBSpikeHistory,
	unsigned int LobeANeuronCount,
	unsigned int LobeBNeuronCount,
	std::vector<float> ConnectionWeights
) 
{

}/**/