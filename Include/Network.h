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

//a gene representing a connection between two nodes in a network
struct ConnectionGene
{
	bool SourceType;
	bool TargetType;
	int SourceID;
	int TargetID;
	float Weight;
};

//a gene representing a node in a network
struct NodeGene 
{
	int(*ActivationFunction)(float);
	float Bias;
};

//a network composed of nodes and connections
class Network 
{
	enum GeneType 
	{
		Connection,
		Node
	};
public:
    //creates a network based on the .genome file at GenomePath
	Network(std::string GenomePath);

	//creates a network from a pre-existing vector of Node and Connection Genes
	Network(std::vector<ConnectionGene> &ConnectionGenes, std::vector<NodeGene> &NodeGenes);

	//creates an empty network
	Network();
};

#endif