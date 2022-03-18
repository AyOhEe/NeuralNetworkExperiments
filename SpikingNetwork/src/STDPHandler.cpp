#include "../Include/STDPHandler.h"

#include <OpenCL/CL/cl.hpp>

//creates the __STDPHandler
__STDPHandler::__STDPHandler() 
{

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