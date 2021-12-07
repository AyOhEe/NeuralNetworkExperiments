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
		float NetValue = 0;
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

	//get and store the source node
	Source = &(Network.Nodes[0]);
}
