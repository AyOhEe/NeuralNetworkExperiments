#include "../Include/Connection.h"

//constructs a connection from a gene
Connection::Connection(BR_RETURN_INT_TYPE* Gene, Network *Net) 
{

}
//constructs a gene from a node identifier and weight
Connection::Connection(long long int NodeIdentifier, float weight) 
{

}

//creates a connection in network Net according to Gene
void Connection::CreateConnection(BR_RETURN_INT_TYPE* Gene, Network *Net) 
{

}

//appends this connection to a node chromosome
void Connection::AppendConnectionToChromosome(std::ofstream &FileSteam, long long int TargetIndex, Network* ParentNet) 
{

}

//returns Gene as a string
std::string Connection::GeneAsString(BR_RETURN_INT_TYPE* Gene) 
{

}