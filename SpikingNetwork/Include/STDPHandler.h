#ifndef STDP_HANDLER_H
#define STDP_HANDLER_H

#include <iostream>

//the actual manager for OpenCL and STDP
class __STDPHandler
{
	//dummy variable
	static int i;

public:
	//creates the __STDPHandler
	__STDPHandler();

	//returns i
	static int Get_i();
};

//the interface class for the STDP handler
class STDPHandler
{
	//the singleton handler instance 
	static __STDPHandler* Handler;

public:
	//creates the STDPHandler
	STDPHandler();

	//returns __STDPHandler::i
	static int Get_i();
};

#endif