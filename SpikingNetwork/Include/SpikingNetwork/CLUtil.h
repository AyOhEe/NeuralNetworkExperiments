#ifndef CLUTIL_H
#define CLUTIL_H

#include <CL/cl.hpp>

namespace CLUtil
{
	void errCheck(cl_int errCode, int l)
	{
		int line = l;
		_ASSERT(errCode == 0);
	}

	//creates a program in opencl from the source file at file
	cl::Program CreateProgram(std::string file);
};

#endif