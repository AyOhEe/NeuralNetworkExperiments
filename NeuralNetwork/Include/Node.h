#ifndef NODE_H
#define NODE_H
/* Network.H
 * Contains the `Node` Class and a Connection and Node class
 */
#include <vector>

#include "Network.h"

//forward declaration of Node class so that std::vector<Node> isn't seen as std::vector<error-type>
class Node;
//forward declaration of Genes so that we're clear to use them
struct ConnectionGene;
struct NodeGene;
//forward declaration of network so that we're clear to use it
class Network;

//a connection between two nodes in a network
class Connection
{
public:
	//the weight of this connection
	float Weight;

	//the source node of this connection
	Node* Source;

	//constructs a connection from a gene
	Connection(ConnectionGene Gene, Network &Network);
	//constructs a connection from a weight and a source node
	Connection(float weight, Node* source);
	
	//returns this connection as a gene
	ConnectionGene AsGene(Network *Network, bool TargetType, int TargetID);
};

//a node in a network
class Node
{
	//connections need access to private members in their constructors
	friend Connection::Connection(ConnectionGene Gene, Network &Network);

	//networks also need access to private members
	friend Network;

	//the connections pointing to this node
	std::vector<Connection> Connections;

	//the bias of this node
	float Bias;

	//the value of this node once it's been calculated
	float Value;

	//does this node need to recalculate it's value?
	bool NeedsToRecalculate;
public:

	//returns the value of this node(by calculation, if required)
	float CalculateValue(Network *Network);

	//constructs a node from a node gene
	Node(NodeGene Gene);
	//constructs the node from a bias
	Node(float Bias);

	//destroys the node and it's connections
	~Node();

	//returns this node as a gene
	NodeGene AsGene();
};

#endif