#ifndef ERRORCODES_H
#define ERRORCODES_H

enum ErrorCodes
{
	//general codes
	SUCCESS,							//the operation succeeded
	UNKNOWN_FAILURE,					//the operation failed for an unknown reason,
										//the error was logged in console

	//internally returned codes
	//connection error codes
	CONNECTION_MISSING_SOURCE,			//a connection attempted to calculate it's value, but found it's source invalid

	//node error codes
	NODE_INVALID_CONNECTION_DISCARDED,	//a connection was found to be invalid, and was removed

	//network error codes
	NETWORK_INVALID_GENOME,				//the network was given an invalid genome
	NETWORK_INVALID_INPUTS,				//the network was passed invalid inputs
	NETWORK_INVALID_NODE_INDEX,			//the network was given an invalid node index
	NETWORK_INVALID_CONN_INDEX,			//the network was given an invalid connection index
	NETWORK_INVALID_ARGUMENT,			//the network was given an invalid argument
	NETWORK_INVALID_SAVE_PATH,			//the network was given an invalid save path
	NETWORK_INVALID_BREED_GENOME_A,		//the network was given an invalid genome A path to breed
	NETWORK_INVALID_BREED_GENOME_B,		//the network was given an invalid genome B path to breed

	//API Error codes
	INVALID_HANDLE,						//the api was passed an invalid network handle
};

#endif