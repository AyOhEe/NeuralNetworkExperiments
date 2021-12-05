#ifndef NODE_H
#define NODE_H
/* Network.H
 * Contains the `Node` Class and a Connection class
 */
#include <vector>
#include "Network.h"

 //a connection between two nodes in a network
struct Connection
{
	//the weight of this connection
	float weight;

	//the source node of this connection
	Node* Source;

	//constructs a connection from a gene
	Connection(ConnectionGene Gene, std::vector<Node> &Nodes);
};

//a node in a network
class Node
{
	//the connections pointing to this node
	std::vector<Connection> Connections;

	//the bias of this node
	float bias;

	//the value of this node once it's been calculated
	float Value;

	//is this node currently in the process of calculating it's value?
	bool CalculatingValue;
	//does this node need to recalculate it's value?
	bool NeedsToRecalculate;
public:

	//returns the value of this node(by calculation, if required)
	float CalculateValue(Network &Network);

	//constructs a node from a node gene
	Node(NodeGene Gene);
};

#endif