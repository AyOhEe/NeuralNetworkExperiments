#include "Include/Network.h"
#include <math.h>

//returns sigmoid(x)
float Sigmoid(float x) 
{
	return 1 / (1 + pow(2.718, -x));
}

int main() 
{
	Network network = Network("test_genome_smallMOD.genome", 2, 1, Sigmoid);
    
    return 0;
} 