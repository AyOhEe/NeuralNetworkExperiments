#include <HebianConnector.h>

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