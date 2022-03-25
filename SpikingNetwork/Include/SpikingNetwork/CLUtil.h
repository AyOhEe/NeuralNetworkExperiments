#ifndef CLUTIL_H
#define CLUTIL_H

#include <CL/cl.hpp>
#include <fstream>

namespace CLUtil
{
	void errCheck(cl_int errCode, int l);

	//creates a program in opencl from the source file at file
	cl::Program CreateProgram(std::string file);
	//creates a program in opencl from the source file at file with context
	cl::Program CreateProgram(std::string file, cl::Context context);
};

#endif