#ifndef NETWORK_H
#define NETWORK_H

#include <map>
#include <vector>
#include <math.h>

#include "Node.h"

class Network 
{
	//all of the nodes in this network
	std::map<unsigned long long int, Node> Nodes;
	//all of the input and output nodes in this network
	std::vector<Node> InputNodes;
	std::vector<Node> OutputNodes;

	//the index for all nodes ever added to this network
	unsigned long long int UniqueNodeIndex;
public:

	//creates a network based on the genome at GenomePath
	Network(std::string GenomePath, int inputs, int outputs, float(*ActivationFunction)(float), bool Verbose = false);
	Network(std::string GenomePath, int inputs, int outputs, int ActivationFunctionIndex, bool Verbose = false);

	//destroys the network
	~Network();

	//the activation function for this network
	float(*ActivationFunction)(float);

	//returns the values of all of the output nodes
	std::vector<float> GetResults();
	//sets the values of all of the input nodes
	void SetInputs(std::vector<float> &Inputs);

	//saves the network to a file on disk
	void SaveNetwork(std::string GenomePath, bool verbose = false);

	//TODO(aria): make like half of these functions give error codes
	//the number of inputs, outputs and nodes in the network
	int InputCount();
	int OutputCount();
	int NodeCount();

	//adds a node between a connection to the network
	bool AddNodeBetweenConnection(int TargetNodeIndex, int ConnectionIndex, float bias);
	//adds a connection between nodes to the network
	bool AddConnectionBetweenNodes(int SourceNodeIndex, int TargetNodeIndex, float weight);

	//removes a node from the network
	bool RemoveNode(int NodeIndex);
	//removes a connection from the network
	bool RemoveConnection(int NodeIndex, int ConnectionIndex);

	//gets the bias of a node
	float GetNodeBias(int NodeIndex);
	//sets the bias of a node
	void SetNodeBias(int NodeIndex, float bias);
	//get the total number of connections going into a node
	int GetTotalNodeConnections(int TargetNodeIndex);
	//gets the weight of a connection
	float GetConnectionWeight(int TargetNodeIndex, int ConnectionIndex);
	//sets the weight of a connection
	void SetConnectionWeight(int TargetNodeIndex, int ConnectionIndex, float weight);
};

#endif