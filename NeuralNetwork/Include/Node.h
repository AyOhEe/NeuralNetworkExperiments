#ifndef NODE_H
#define NODE_H

#include "BinaryReader.h"
#include "Network.h"
#include "Connection.h"

#include <vector>

//forward definitions
class Network;
class Node;
class Connection;

class Node 
{
	//networks require special access to nodes
	friend Network;
	//connections require special access to nodes
	friend Connection;

	//all of the connections going into this node
	std::vector<Connection> Connections;
	//the bias of this node
	float Bias;
	//the value of this node the last time it was calculated
	float value;

	//does this node need to recalculate it's value?
	bool NeedsToRecalc;

public:

	//creates a node from a gene
	Node(BR_RETURN_INT_TYPE* Gene);
	//creates a node from a bias
	Node(float bias);

	//gets the value of this node
	float CalculateValue(Network* Net);

	//appends this node to a node chromosome
	void AppendNodeToChromosome(std::ofstream &FileStream);

	//returns Gene as a string
	static std::string GeneAsString(BR_RETURN_INT_TYPE* Gene);
};

#endif