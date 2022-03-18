#include "STDPHandler.h"

#include <iostream>

int main()
{
	//output get_i
	std::cout << STDPHandler::Get_i() << std::endl;

	//create a new handler instance
	STDPHandler Handler = STDPHandler();

	//output get_i
	std::cout << STDPHandler::Get_i() << std::endl;
}