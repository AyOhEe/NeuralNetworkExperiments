// dllmain.cpp : Defines the entry point for the DLL application.
#include "../Include/framework.h"

#include <map>
#include <algorithm>

#include "../../NeuralNetwork/Include/Network.h"
#include "../../NeuralNetwork/Include/Node.h"
#include "../../NeuralNetwork/Include/ErrorCodes.h"

#if defined(__NT__) | defined(_WIN32)
    #ifdef NEURALNETDLL_EXPORTS
		#define NEURALNET_API __declspec(dllexport)
    #else
		#define NEURALNET_API __declspec(dllimport)
    #endif
#elif __unix__
    #define NEURALNET_API 
#endif

#define VALIDATE_HANDLE_BOOL \
/*ensure the handle is valid*/ \
std::map<unsigned long long int, Network*>::iterator handleLocation = __NETWORKS.find(handle); \
if (handleLocation == __NETWORKS.end()) \
{ \
	/*invalid handle, indicate failure*/ \
	*ErrCode = INVALID_HANDLE; \
	return false; \
} \

#define VALIDATE_HANDLE_VOID \
/*ensure the handle is valid*/ \
std::map<unsigned long long int, Network*>::iterator handleLocation = __NETWORKS.find(handle); \
if (handleLocation == __NETWORKS.end()) \
{ \
	/*invalid handle, indicate failure*/ \
	*ErrCode = INVALID_HANDLE; \
	return; \
} \

#define VALIDATE_HANDLE_INT \
/*ensure the handle is valid*/ \
std::map<unsigned long long int, Network*>::iterator handleLocation = __NETWORKS.find(handle); \
if (handleLocation == __NETWORKS.end()) \
{ \
	/*invalid handle, indicate failure*/ \
	*ErrCode = INVALID_HANDLE; \
	return -1; \
} \

//all networks alive
std::map<unsigned long long int, Network*> __NETWORKS = std::map<unsigned long long int, Network*>();

extern "C"
{
	//creates a network and returns a handle
	unsigned long long int NEURALNET_API CREATE_NETWORK(
		const char* GenomePath, 
		unsigned int Inputs, 
		unsigned int Outputs, 
		int ActivationFunctionIndex, 
		unsigned int *ErrCode,
		bool Verbose)
	{
		//default the error code to success
		*ErrCode = SUCCESS;
		unsigned int Err = SUCCESS;
		try 
		{
			//create the new network
			Network* Net = new Network(std::string(GenomePath), Inputs, Outputs, ActivationFunctionIndex, &Err, Verbose);
			if(Err != SUCCESS)
			{
				//oops, something didn't pan out
				*ErrCode = Err;
				return 0;
			}

			//insert it into the vector
			__NETWORKS[(unsigned long long int)Net] = Net;

			//return the "handle" which is really just the memory address of the network(guaranteed to be unique)
			return (unsigned long long int)Net;
		}
		catch (std::exception &ex)
		{
			*ErrCode = UNKNOWN_FAILURE;
			std::stringstream ErrorMessage;
			ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
			std::cout << ex.what() << std::endl << ErrorMessage.str() << std::endl;
			return false;
		}
	}

	//removes a network if it exists
    bool NEURALNET_API DESTROY_NETWORK(unsigned long long int handle, unsigned int *ErrCode)
	{
		//default the error code to success
		*ErrCode = SUCCESS;
		try 
		{
			VALIDATE_HANDLE_BOOL

			//valid handle, delete the network and remove it from the vector
			delete handleLocation->second;
			__NETWORKS.erase(handleLocation);

			//indicate success
			return true;
		}
		catch (std::exception &ex)
		{
			*ErrCode = UNKNOWN_FAILURE;
			std::stringstream ErrorMessage;
			ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
			std::cout << ex.what() << std::endl << ErrorMessage.str() << std::endl;
			return false;
		}
	}

	//creates a copy of the network given
	bool NEURALNET_API COPY_NETWORK(unsigned long long int handle, unsigned int *ErrCode)
	{
		//default the error code to success
		*ErrCode = SUCCESS;
		try
		{
			VALIDATE_HANDLE_BOOL

			//valid handle, create copy of the network and store it
			Network* Net = new Network(*__NETWORKS[handle]);
			__NETWORKS[(unsigned long long int)Net] = Net;
		}
		catch (std::exception &ex)
		{
			*ErrCode = UNKNOWN_FAILURE;
			std::stringstream ErrorMessage;
			ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
			std::cout << ex.what() << std::endl << ErrorMessage.str() << std::endl;
			return false;
		}

		//indicate success
		return true;
	}

	//sets the inputs of a network
	bool NEURALNET_API SET_NETWORK_INPUTS(unsigned long long int handle, float* inputs, int n_inputs, unsigned int *ErrCode)
	{
		//default the error code to success
		*ErrCode = SUCCESS;
		try 
		{
			VALIDATE_HANDLE_BOOL

				//validate n_inputs
				if (n_inputs != handleLocation->second->InputCount())
				{
					return false;
				}

			//convert the inputs into a vector
			std::vector<float> Inputs;
			std::copy(inputs, inputs + n_inputs, std::back_inserter(Inputs));

			//set the inputs
			unsigned int Err = SUCCESS;
			handleLocation->second->SetInputs(Inputs, &Err);
			if (Err != SUCCESS) 
			{
				*ErrCode = Err;
				return false;
			}

			return true;
		}
		catch (std::exception &ex)
		{
			*ErrCode = UNKNOWN_FAILURE;
			std::stringstream ErrorMessage;
			ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
			std::cout << ex.what() << std::endl << ErrorMessage.str() << std::endl;
			return false;
		}
	}

	//calculates the values of a network
	bool NEURALNET_API GET_NETWORK_OUTPUTS(unsigned long long int handle, float* _outputs, int n_outputs, unsigned int *ErrCode)
	{
		//default the error code to success
		*ErrCode = SUCCESS;
		try
		{
			VALIDATE_HANDLE_BOOL

			//validate n_outputs
			if (n_outputs != handleLocation->second->OutputCount())
			{
				return false;
			}

			//copy over the network outputs to the output array
			unsigned int Err = SUCCESS;
			std::vector<float> Outputs = handleLocation->second->GetResults(&Err);
			if (Err != SUCCESS)
			{
				*ErrCode = Err;
				return false;
			}
			for (int i = 0; (i < n_outputs) & (i < Outputs.size()); i++)
			{
				_outputs[i] = Outputs[i];
			}

			return true;
		}
		catch (std::exception &ex)
		{
			*ErrCode = UNKNOWN_FAILURE;
			std::stringstream ErrorMessage;
			ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
			std::cout << ex.what() << std::endl << ErrorMessage.str() << std::endl;
			return false;
		}
	}

	//returns the number of inputs, outputs or nodes in a network
	int NEURALNET_API GET_NETWORK_INPUT_COUNT(unsigned long long int handle, unsigned int *ErrCode)
	{
		//default the error code to success
		*ErrCode = SUCCESS;
		try 
		{
			VALIDATE_HANDLE_BOOL

			return handleLocation->second->InputCount();
		}
		catch (std::exception &ex)
		{
			*ErrCode = UNKNOWN_FAILURE;
			std::stringstream ErrorMessage;
			ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
			std::cout << ex.what() << std::endl << ErrorMessage.str() << std::endl;
			return false;
		}
	}
	int NEURALNET_API GET_NETWORK_NODE_COUNT(unsigned long long int handle, unsigned int *ErrCode)
	{
		//default the error code to success
		*ErrCode = SUCCESS;
		try 
		{
			VALIDATE_HANDLE_BOOL

			return handleLocation->second->NodeCount();
		}
		catch (std::exception &ex)
		{
			*ErrCode = UNKNOWN_FAILURE;
			std::stringstream ErrorMessage;
			ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
			std::cout << ex.what() << std::endl << ErrorMessage.str() << std::endl;
			return false;
		}
	}
	int NEURALNET_API GET_NETWORK_OUTPUT_COUNT(unsigned long long int handle, unsigned int *ErrCode)
	{
		//default the error code to success
		*ErrCode = SUCCESS;
		try 
		{
			VALIDATE_HANDLE_BOOL

			return handleLocation->second->OutputCount();
		}
		catch (std::exception &ex)
		{
			*ErrCode = UNKNOWN_FAILURE;
			std::stringstream ErrorMessage;
			ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
			std::cout << ex.what() << std::endl << ErrorMessage.str() << std::endl;
			return false;
		}
	}

	//adds a node between a connection to a network
	bool NEURALNET_API ADD_NODE_BETWEEN_CONNECTION(unsigned long long int handle, int TargetNodeIndex, int ConnectionIndex, float bias, unsigned int *ErrCode)
	{
		//default the error code to success
		*ErrCode = SUCCESS;
		try 
		{
			VALIDATE_HANDLE_BOOL

			unsigned int Err = SUCCESS;
			handleLocation->second->AddNodeBetweenConnection(TargetNodeIndex, ConnectionIndex, bias, &Err);
			if (Err != SUCCESS)
			{
				*ErrCode = Err;
				return false;
			}

			return true;
		}
		catch (std::exception &ex)
		{
			*ErrCode = UNKNOWN_FAILURE;
			std::stringstream ErrorMessage;
			ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
			std::cout << ex.what() << std::endl << ErrorMessage.str() << std::endl;
			return false;
		}
	}
	//adds a connection between nodes to a network
	bool NEURALNET_API ADD_CONNECTION_BETWEEN_NODES(unsigned int handle, int SourceNodeIndex, int TargetNodeIndex, float weight, unsigned int *ErrCode)
	{
		//default the error code to success
		*ErrCode = SUCCESS;
		try 
		{
			VALIDATE_HANDLE_BOOL

			unsigned int Err = SUCCESS;
			handleLocation->second->AddConnectionBetweenNodes(SourceNodeIndex, TargetNodeIndex, weight, &Err);
			if (Err != SUCCESS)
			{
				*ErrCode = Err;
				return false;
			}

			return true;
		}
		catch (std::exception &ex)
		{
			*ErrCode = UNKNOWN_FAILURE;
			std::stringstream ErrorMessage;
			ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
			std::cout << ex.what() << std::endl << ErrorMessage.str() << std::endl;
			return false;
		}
	}

	//removes a node from the network
	bool NEURALNET_API REMOVE_NODE(unsigned int long long handle, int NodeIndex, unsigned int *ErrCode)
	{
		//default the error code to success
		*ErrCode = SUCCESS;
		try 
		{
			VALIDATE_HANDLE_BOOL

			unsigned int Err = SUCCESS;
			handleLocation->second->RemoveNode(NodeIndex, &Err);
			if (Err != SUCCESS)
			{
				*ErrCode = Err;
				return false;
			}

			return true;
		}
		catch (std::exception &ex)
		{
			*ErrCode = UNKNOWN_FAILURE;
			std::stringstream ErrorMessage;
			ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
			std::cout << ex.what() << std::endl << ErrorMessage.str() << std::endl;
			return false;
		}
	}
	//removes a connection from the network
	bool NEURALNET_API REMOVE_CONNECTION(unsigned long long int handle, int NodeIndex, int ConnectionIndex, unsigned int *ErrCode)
	{
		//default the error code to success
		*ErrCode = SUCCESS;
		try 
		{
			VALIDATE_HANDLE_BOOL

			unsigned int Err = SUCCESS;
			handleLocation->second->RemoveConnection(NodeIndex, ConnectionIndex, &Err);
			if (Err != SUCCESS)
			{
				*ErrCode = Err;
				return false;
			}

			return true;
		}
		catch (std::exception &ex)
		{
			*ErrCode = UNKNOWN_FAILURE;
			std::stringstream ErrorMessage;
			ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
			std::cout << ex.what() << std::endl << ErrorMessage.str() << std::endl;
			return false;
		}
	}

	//gets the bias of a node
	float NEURALNET_API GET_NODE_BIAS(unsigned long long int handle, int NodeIndex, unsigned int *ErrCode)
	{
		//default the error code to success
		*ErrCode = SUCCESS;
		try 
		{
			VALIDATE_HANDLE_BOOL

			unsigned int Err = SUCCESS;
			float result = handleLocation->second->GetNodeBias(NodeIndex, &Err);
			if (Err != SUCCESS)
			{
				*ErrCode = Err;
				return false;
			}

			return result;
		}
		catch (std::exception &ex)
		{
			*ErrCode = UNKNOWN_FAILURE;
			std::stringstream ErrorMessage;
			ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
			std::cout << ex.what() << std::endl << ErrorMessage.str() << std::endl;
			return false;
		}
	}
	//sets the bias of a node
	bool NEURALNET_API SET_NODE_BIAS(unsigned long long int handle, int NodeIndex, float bias, unsigned int *ErrCode)
	{
		//default the error code to success
		*ErrCode = SUCCESS;
		try 
		{
			VALIDATE_HANDLE_BOOL

			unsigned int Err = SUCCESS;
			handleLocation->second->SetNodeBias(NodeIndex, bias, &Err);
			if (Err != SUCCESS)
			{
				*ErrCode = Err;
				return false;
			}

			return true;
		}
		catch (std::exception &ex)
		{
			*ErrCode = UNKNOWN_FAILURE;
			std::stringstream ErrorMessage;
			ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
			std::cout << ex.what() << std::endl << ErrorMessage.str() << std::endl;
			return false;
		}
	}

	//gets the total number of connections going into a connection
	int NEURALNET_API GET_NODE_CONNECTION_COUNT(unsigned long long int handle, int TargetNodeIndex, unsigned int *ErrCode)
	{
		//default the error code to success
		*ErrCode = SUCCESS;
		try 
		{
			VALIDATE_HANDLE_INT

			unsigned int Err = SUCCESS;
			int result = handleLocation->second->GetTotalNodeConnections(TargetNodeIndex, &Err);
			if (Err != SUCCESS)
			{
				*ErrCode = Err;
				return false;
			}

			return result;
		}
		catch (std::exception &ex)
		{
			*ErrCode = UNKNOWN_FAILURE;
			std::stringstream ErrorMessage;
			ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
			std::cout << ex.what() << std::endl << ErrorMessage.str() << std::endl;
			return false;
		}
	}

	//gets the weight of a connection
	float NEURALNET_API GET_CONNECTION_WEIGHT(unsigned long long int handle, int TargetNodeIndex, int ConnectionIndex, unsigned int *ErrCode)
	{
		//default the error code to success
		*ErrCode = SUCCESS;
		try 
		{
			VALIDATE_HANDLE_BOOL

			unsigned int Err = SUCCESS;
			float result = handleLocation->second->GetConnectionWeight(TargetNodeIndex, ConnectionIndex, &Err);
			if (Err != SUCCESS)
			{
				*ErrCode = Err;
				return false;
			}

			return result;
		}
		catch (std::exception &ex)
		{
			*ErrCode = UNKNOWN_FAILURE;
			std::stringstream ErrorMessage;
			ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
			std::cout << ex.what() << std::endl << ErrorMessage.str() << std::endl;
			return false;
		}
	}
	//sets the weight of a connection
	bool NEURALNET_API SET_CONNECTION_WEIGHT(unsigned long long int handle, int TargetNodeIndex, int ConnectionIndex, float weight, unsigned int *ErrCode)
	{
		//default the error code to success
		*ErrCode = SUCCESS;
		try 
		{
			VALIDATE_HANDLE_BOOL

			unsigned int Err = SUCCESS;
			handleLocation->second->SetConnectionWeight(TargetNodeIndex, ConnectionIndex, weight, &Err);
			if (Err != SUCCESS)
			{
				*ErrCode = Err;
				return false;
			}

			return true;
		}
		catch (std::exception &ex)
		{
			*ErrCode = UNKNOWN_FAILURE;
			std::stringstream ErrorMessage;
			ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
			std::cout << ex.what() << std::endl << ErrorMessage.str() << std::endl;
			return false;
		}
	}

	//saves a network
	bool NEURALNET_API SAVE_NETWORK(unsigned long long int handle, const char* path, unsigned int *ErrCode)
	{
		//default the error code to success
		*ErrCode = SUCCESS;
		try 
		{
			VALIDATE_HANDLE_BOOL

			unsigned int Err = SUCCESS;
			handleLocation->second->SaveNetwork(std::string(path), &Err);
			if (Err != SUCCESS)
			{
				*ErrCode = Err;
				return false;
			}

			return true;
		}
		catch (std::exception &ex)
		{
			*ErrCode = UNKNOWN_FAILURE;
			std::stringstream ErrorMessage;
			ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
			std::cout << ex.what() << std::endl << ErrorMessage.str() << std::endl;
			return false;
		}
	}

    //breeds two networks together
    unsigned long long int NEURALNET_API BREED_NETWORKS(
		unsigned long long int handleA, unsigned long long int handleB, 
		int inputs, int outputs, int ActivationFunctionIndex, 
		float* _CrossoverPoints, int nCrossoverPoints, float MutationChance, 
		unsigned int *ErrCode, bool verbose)
    {
		//default the error code to success
		*ErrCode = SUCCESS;
		try 
		{
			//validate the network handles
			std::map<unsigned long long int, Network*>::iterator handleALocation = __NETWORKS.find(handleA);
			if (handleALocation == __NETWORKS.end())
			{
				/*invalid handle, indicate failure*/
				*ErrCode = NETWORK_INVALID_BREED_GENOME_A;
				return 0;
			}
			std::map<unsigned long long int, Network*>::iterator handleBLocation = __NETWORKS.find(handleB);
			if (handleBLocation == __NETWORKS.end())
			{
				/*invalid handle, indicate failure*/
				*ErrCode = NETWORK_INVALID_BREED_GENOME_A;
				return 0;
			}
			//save the networks to be bred
			unsigned int Err = SUCCESS;
			handleALocation->second->SaveNetwork("Networks/BreedA", &Err);
			if (Err != SUCCESS)
			{
				*ErrCode = Err;
				return false;
			}
			handleBLocation->second->SaveNetwork("Networks/BreedB", &Err);
			if (Err != SUCCESS)
			{
				*ErrCode = Err;
				return false;
			}

			//copy over the crossover points into a vector
			std::vector<float> CrossoverPoints;
			std::copy(_CrossoverPoints, _CrossoverPoints + nCrossoverPoints, std::back_inserter(CrossoverPoints));
			//create the breed settings struct
			Network::BreedSettings Settings{ CrossoverPoints, MutationChance };

			//breed the networks
			Network* ChildNet = new Network("Networks/BreedA", "Networks/BreedB", Settings, inputs, outputs, ActivationFunctionIndex, &Err, verbose);
			if (Err != SUCCESS)
			{
				*ErrCode = Err;
				return false;
			}

			//insert it into the map
			__NETWORKS[(unsigned long long int)ChildNet] = ChildNet;

			//return the "handle" which is really just the memory address of the network(guaranteed to be unique)
			return (unsigned long long int)ChildNet;
		}
		catch (std::exception &ex)
		{
			*ErrCode = UNKNOWN_FAILURE;
			std::stringstream ErrorMessage;
			ErrorMessage << "Unknwon Failure at" << __FILE__ << ":" << __LINE__;
			std::cout << ex.what() << std::endl << ErrorMessage.str() << std::endl;
			return false;
		}
    }
}

//TODO: throw some definitions for these functions in a header and split this file into multiple files(maybe?)