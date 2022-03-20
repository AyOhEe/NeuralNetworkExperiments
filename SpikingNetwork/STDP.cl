float STDPFunction(ulong SourceTime, ulong TargetTime, float OriginalWeight)
{
	//if they're both non-zero, return the original weight
	if(SourceTime != 0 && TargetTime != 0)
	{
		return OriginalWeight;
	}
	
	//get the difference between the source and target times
	float TimeDifference = 0.05 * abs_diff((ulong)SourceTime, (ulong)TargetTime);

	//determine the weight change
	float WeightChange = min(exp((float)-TimeDifference), (float)TimeDifference);
	//branchlessly determine if the change should be positive or negative
	WeightChange *= 1 + ((SourceTime < TargetTime) * -2);

	//alter the weight change to keep weights from spiraling out of control
	WeightChange *= exp(-fabs((float)(0.5f * (ulong)OriginalWeight))) * (4 - abs((ulong)OriginalWeight)) * 0.0625f;

	//return the new weight with a minimum value of zero and a maximum of four
	return fmin((float)4, fmax((float)0, OriginalWeight + WeightChange));
}

//kernel entrypoint
__kernel void Main(
	__global ulong* SourceTimes, 
	__global ulong* TargetTimes, 
	__global float* Weights, 
	__global float* NewWeights) 
{
	//get the ID of this kernel
	int ID = get_global_id(0);
	
	//set this kernel's weight to the new weight calculated by STDP
	NewWeights[ID] = STDPFunction(SourceTimes[ID], TargetTimes[ID], Weights[ID]);
}
