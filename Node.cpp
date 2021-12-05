#include "Include/Node.h"

//constructs a node from a node gene
Node::Node(NodeGene Gene) 
{
	bias = Gene.Bias;
}

//returns the value of this node(by calculation, if required)
float Node::CalculateValue(Network &Network) 
{
	//do we need to recalculate this node's value?
	if (NeedsToRecalculate) 
	{
		//yes, iterate through our connections and sum the return values
		CalculatingValue = true;
		for (std::vector<Connection>::iterator iter = Connections.begin(); iter != Connections.end(); iter++)
		{
			Value += iter->weight * iter->Source->CalculateValue(Network);
		}
		//and put the sum through the activation function
		Value = Network.ActivationFunction(Value);

		//we nolonger need to recalcuate, as we've already recalculated
		NeedsToRecalculate = false;
		//we're also nolonger calculating
		CalculatingValue = false;
	}
	else 
	{
		//no, just return the value
		return Value;
	}
}