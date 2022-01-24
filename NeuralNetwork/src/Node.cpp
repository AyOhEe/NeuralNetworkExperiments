#include "../Include/Node.h"

//creates a node from a gene
Node::Node(BR_RETURN_INT_TYPE* Gene) 
{
	//calculate and store the bias
	Bias = Gene[1] == 1 ? -1 : 1;
	Bias *= Gene[2];
	Bias /= NODE_GENE_BIAS_DIVISOR;
}
//creates a node from a bias
Node::Node(float bias) 
{
	//store the bias
	Bias = bias;
}

//gets the value of this node
float Node::CalculateValue(Network* Net) 
{
	//only recalculate if we are required to
	if (NeedsToRecalc) 
	{
		//iterate through the connections of the node
		float NewVal = 0;
		for (auto ConnIter : Connections) 
		{
			//is the source an input or internal node?
			if(ConnIter.SourceNode < 0)
			{
				//input node
				//add the weighted value of the connection
				NewVal += ConnIter.Weight * Net->InputNodes[-ConnIter.SourceNode - 1].CalculateValue(Net);
			}
			else 
			{
				//internal node
				//add the weighted value of the connection
				NewVal += ConnIter.Weight * Net->Nodes[ConnIter.SourceNode].CalculateValue(Net);
			}
		}
		//put the new value through the activation function
		value = Net->ActivationFunction(NewVal);
	}

	//return our value
	return value;
}

//appends this node to a node chromosome
void Node::AppendNodeToChromosome(std::ofstream &FileStream) 
{
	//calculate the bias as an integer
	int IntBias = abs(Bias * NODE_GENE_BIAS_DIVISOR);

	//remake the old gene
	char* Gene = new char[2];
	Gene[0] = 0b10000000 + (Bias < 0 ? 0b01000000 : 0) + (IntBias >> 8);
	Gene[1] = IntBias & 0b11111111;

	//write the gene to the file
	FileStream << Gene[0] << Gene[1];
}

//returns Gene as a string
std::string Node::GeneAsString(BR_RETURN_INT_TYPE* Gene)
{
	std::stringstream StringStream;
	StringStream << "Node Gene: " << (Gene[1] == 1 ? -1 : 1) * Gene[2] / NODE_GENE_BIAS_DIVISOR;
	return StringStream.str();
}