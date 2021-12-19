#ifndef GENOME_H
#define GENOME_H
/* Network.H
 * Contains the `Network` Class and two Gene classes
 */

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <math.h>

#include "Node.h"
#include "BinaryReader.h"

#define NODE_GENE_BIAS_DIVISOR 8192.0f
#define CONNECTION_GENE_WEIGHT_DIVISOR 128.0f

//forward declaration of node and connection so we're clear to use it
class Node;
class Connection;

//a gene representing a connection between two nodes in a network
struct ConnectionGene
{
	bool SourceType;
	bool TargetType;
	int SourceID;
	int TargetID;
	float Weight;

	//returns a representation of this gene as a string
	std::string ToString();

	//appends this gene to a filestream
	void AppendGene(std::ofstream &stream, bool verbose = false);
};

//a gene representing a node in a network
struct NodeGene 
{
	float Bias;

	//returns a representation of this gene as a string
	std::string ToString();

	//appends this gene to a filestream
	void AppendGene(std::ofstream &stream, bool verbose = false);
};

//a network composed of nodes and connections
class Network 
{
	//connections need access to the node array
	friend Connection;

	//all of the nodes in this network
	std::vector<Node*> Nodes;
	//all of the input nodes for this network
	std::vector<Node*> InputNodes;
	std::vector<Node*> OutputNodes;

public:
	//the activation function for this network
	float(*ActivationFunction)(float);

    //creates a network based on the .genome file at GenomePath
	Network(std::string GenomePath, int inputs, int outputs, float(*ActivationFunction)(float), bool Verbose = false);

	//creates a network from a pre-existing vector of Node and Connection Genes
	Network(std::vector<ConnectionGene> &ConnectionGenes, std::vector<NodeGene> &NodeGenes, int inputs, int outputs, float(*ActivationFunction)(float));

	//creates a blank network
	Network();

	//destroys the network
	~Network();

	//returns the values of all of the output nodes
	std::vector<float> GetResults();
	//sets the values of all of the input nodes
	void SetInputs(std::vector<float> &Inputs);

	//saves the network to a file on disk
	void SaveNetwork(std::string GenomePath, bool verbose = false);
};

#endif