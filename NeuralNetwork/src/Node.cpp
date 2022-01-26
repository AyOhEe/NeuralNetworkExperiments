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

//default constructor
Node::Node() {}

//gets the value of this node
float Node::CalculateValue(Network* Net, unsigned int* ErrCode)
{
	*ErrCode = SUCCESS; //default to success if not told otherwise

	//only recalculate if we are required to
	if (NeedsToRecalc) 
	{
		//iterate through the connections of the node
		float NewVal = 0;
		unsigned int ConnErr = 0;
		for (int i = 0; i < Connections.size(); i++) 
		{
			//attempt to add the value of the connection. if we fail, remove the connection as it's invalid
			if (!Connections[i].TryAddValue(&NewVal, Net, &ConnErr)) 
			{
				Connections.erase(Connections.begin() + i--); //we decrement i here as to not skip an element

				//detect and pass on errors
				if (ConnErr == CONNECTION_MISSING_SOURCE)
					*ErrCode = NODE_INVALID_CONNECTION_DISCARDED;
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