#include <Network.h>

int main()
{
	int ErrCode = 0;
	SpikingNetwork net = SpikingNetwork("Genomes/test_genome", 2, 2, &ErrCode, true);
}