#include "../Include/Node.h"

//constructs a node from a node gene
Node::Node(NodeGene Gene) 
{
	Bias = Gene.Bias;
}

//constructs the node from a bias
Node::Node(float Bias) 
{
	Bias = Bias;
}

//returns the value of this node(by calculation, if required)
float Node::CalculateValue(Network *Network) 
{
	//do we need to recalculate this node's value?
	if (NeedsToRecalculate) 
	{
		//we nolonger need to recalcuate, as we're now in the process of recalcuating
		NeedsToRecalculate = false;

		float NewValue = 0;
		for (std::vector<Connection>::iterator iter = Connections.begin(); iter != Connections.end(); iter++)
		{
			NewValue += iter->Weight * iter->Source->CalculateValue(Network);
		}
		//and put the sum through the activation function, along side this node's bias
		NewValue = Network->ActivationFunction(Bias + NewValue);
		
		//store the new value as the actual value
		Value = NewValue;
	}

	//return our value now that it's guaranteed to be correct
	return Value;
}

//destroys the node and it's connections
Node::~Node() 
{
	//clear the connections vector
	if (!Connections.empty()) 
	{
		Connections.clear();
	}
}

//constructs a connection from a gene
Connection::Connection(ConnectionGene Gene, Network* Network)
{
	//store the weight
	Weight = Gene.Weight;
	
	//determine how we should get the source node
	if (Gene.SourceType) 
	{
		//nodes, we can use the network's node array
		//we modulo the gene's source id to ensure it always gets a node, no matter what the value is
		Source = Network->Nodes[Gene.SourceID % Network->Nodes.size()];
	}
	else 
	{
		//inputs, we can use the network's input node array
		//we modulo the gene's source id to ensure it always gets a node, no matter what the value is
		Source = Network->InputNodes[Gene.SourceID % Network->InputNodes.size()];
	}
}

//constructs a connection from a weight and a source node
Connection::Connection(float weight, Node* source) 
{
	//store the weight and source
	Weight = weight;
	Source = source;
}

//returns this node as a gene
NodeGene Node::AsGene()
{
	//the gene to return
	NodeGene Gene;

	//store this node's properties in the gene
	Gene.Bias = Bias;

	//return the gene
	return Gene;
}

//returns this connection as a gene
ConnectionGene Connection::AsGene(Network *Network, bool TargetType, int TargetID)
{
	//the gene to return
	ConnectionGene Gene;

	//find and store this connection's properties in the gene

	//try to find the source node and index in input nodes
	int index = -1;
	std::vector<Node*>::iterator SourceIter = std::find(Network->InputNodes.begin(), Network->InputNodes.end(), Source);
	if (SourceIter != Network->InputNodes.end()) 
	{
		//the source was an input node, store it's index and set the source type
		Gene.SourceType = false;

		Gene.SourceID = SourceIter - Network->InputNodes.begin();
	}
	else 
	{
		//the source was an internal node, get it's index and set the source type
		Gene.SourceType = true;

		SourceIter = std::find(Network->Nodes.begin(), Network->Nodes.end(), Source); 
		Gene.SourceID = SourceIter - Network->Nodes.begin();
	}

	//store the target type and target id
	Gene.TargetType = TargetType;
	Gene.TargetID = TargetID;

	//store the weight of the connection
	Gene.Weight = Weight;

	//return the gene
	return Gene;
}