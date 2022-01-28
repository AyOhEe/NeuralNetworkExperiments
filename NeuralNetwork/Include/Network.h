#ifndef NETWORK_H
#define NETWORK_H

#include <map>
#include <vector>
#include <math.h>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

#ifndef __unix__
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

#include "Node.h"
#include "Chromosome.h"

#define NODE_GENE_BIAS_DIVISOR 8192.0f
#define CONNECTION_GENE_WEIGHT_DIVISOR 128.0f

//forward definitions
class Network;
class Node;
class Connection;

class Network 
{
	//nodes need access to other nodes
	friend Node;
	//connections need access to nodes
	friend Connection;

	//all of the nodes in this network
	std::map<long long int, Node> Nodes;
	//all of the input and output nodes in this network
	std::vector<Node> InputNodes;
	std::vector<Node> OutputNodes;

	//the index for all nodes ever added to this network
	long long int UniqueNodeIndex;
public:

	//creates a network based on the genome at GenomePath
	Network(std::string GenomePath, int inputs, int outputs, float(*ActivationFunction)(float), unsigned int* ErrCode, bool Verbose = false);
	Network(std::string GenomePath, int inputs, int outputs, int ActivationFunctionIndex, unsigned int* ErrCode, bool Verbose = false);

	//settings determining how networks should be bred
	struct BreedSettings
	{
		std::vector<float> CrossoverPoints; //points where the genome that is read from is swapped. measured in percents
		float MutationChance; //mutation chance. measured in mutations/1000 bits
	};

	//creates a network based on two genomes
	Network(
		std::string GenomePathA, std::string GenomePathB, 
		BreedSettings Settings, 
		int inputs, int outputs, 
		int ActivationFunctionIndex, 
		unsigned int* ErrCode, bool Verbose = false
	);

	//the activation function for this network
	float(*ActivationFunction)(float);

	//returns the values of all of the output nodes
	std::vector<float> GetResults(unsigned int* ErrCode);
	//sets the values of all of the input nodes
	void SetInputs(std::vector<float> &Inputs, unsigned int* ErrCode);

	//saves the network to a file on disk
	void SaveNetwork(std::string GenomePath, unsigned int* ErrCode, bool verbose = false);

	//the number of inputs, outputs and nodes in the network
	int InputCount();
	int OutputCount();
	int NodeCount();

	//adds a node between a connection to the network
	bool AddNodeBetweenConnection(int TargetNodeIndex, int ConnectionIndex, float bias, unsigned int* ErrCode);
	//adds a connection between nodes to the network
	bool AddConnectionBetweenNodes(int SourceNodeIndex, int TargetNodeIndex, float weight, unsigned int* ErrCode);

	//removes a node from the network
	bool RemoveNode(int NodeIndex, unsigned int* ErrCode);
	//removes a connection from the network
	bool RemoveConnection(int NodeIndex, int ConnectionIndex, unsigned int* ErrCode);

	//gets the bias of a node
	float GetNodeBias(int NodeIndex, unsigned int* ErrCode);
	//sets the bias of a node
	void SetNodeBias(int NodeIndex, float bias, unsigned int* ErrCode);
	//get the total number of connections going into a node
	int GetTotalNodeConnections(int TargetNodeIndex, unsigned int* ErrCode);
	//gets the weight of a connection
	float GetConnectionWeight(int TargetNodeIndex, int ConnectionIndex, unsigned int* ErrCode);
	//sets the weight of a connection
	bool SetConnectionWeight(int TargetNodeIndex, int ConnectionIndex, float weight, unsigned int* ErrCode);
};

#endif