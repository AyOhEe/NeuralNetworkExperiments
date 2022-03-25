#include "CLUtil.h"

//creates a program in opencl from the source file at file
cl::Program CLUtil::CreateProgram(std::string file)
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