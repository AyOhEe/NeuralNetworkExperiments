#include "../Include/STDPHandler.h"

//dummy variable
int __STDPHandler::i = 0;

//creates the __STDPHandler
__STDPHandler::__STDPHandler() 
{
	i = 5;
	std::cout << "__STDPHandler::__STDPHandler() Called!" << std::endl;
}

//returns i
int __STDPHandler::Get_i() 
{
	return i;
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

//returns __STDPHandler::i
int STDPHandler::Get_i() 
{
	return __STDPHandler::Get_i();
}