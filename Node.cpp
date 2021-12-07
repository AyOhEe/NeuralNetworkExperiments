#include "Include/Node.h"

//constructs a node from a node gene
Node::Node(NodeGene Gene) 
{
	Bias = Gene.Bias;
}

//returns the value of this node(by calculation, if required)
float Node::CalculateValue(Network &Network) 
{
	//are we already in the process of calculating our value?
	if (CalculatingValue) 
	{
		//yep, return our last value
		return Value;
	}

	//do we need to recalculate this node's value?
	if (NeedsToRecalculate) 
	{
		//yes, iterate through our connections and sum the return values
		CalculatingValue = true;
		float NewValue = 0;
		for (std::vector<Connection>::iterator iter = Connections.begin(); iter != Connections.end(); iter++)
		{
			NewValue += iter->Weight * iter->Source->CalculateValue(Network);
		}
		//and put the sum through the activation function, along side this node's bias
		NewValue = Bias + Network.ActivationFunction(Value);

		//we nolonger need to recalcuate, as we've already recalculated
		NeedsToRecalculate = false;
		//we're also no longer calculating
		CalculatingValue = false;
		
		//store the new value as the actual value
		Value = NewValue;
	}
	else 
	{
		//no, just return the value
		return Value;
	}
}

//constructs a connection from a gene
Connection::Connection(ConnectionGene Gene, Network &Network)
{
	//store the weight
	Weight = Gene.Weight;
	
	//determine how we should get the source node
	if (Gene.SourceType) 
	{
		//nodes, we can use the network's node array
		//we modulo the gene's source id to ensure it always gets a node, no matter what the value is
		Source = &(Network.Nodes[Gene.SourceID % Network.Nodes.size()]);
	}
	else 
	{
		//inputs, we can use the network's input node array
		//we modulo the gene's source id to ensure it always gets a node, no matter what the value is
		Source = &(Network.InputNodes[Gene.SourceID % Network.Nodes.size()]);
	}
}
