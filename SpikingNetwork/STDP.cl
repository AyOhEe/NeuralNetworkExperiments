float STDPFunction(ulong SourceTime, ulong TargetTime, float OriginalWeight)
{
	//if they're both non-zero, return the original weight
	if(SourceTime != 0 && TargetTime != 0)
	{
		return OriginalWeight;
	}
	
	//get the difference between the source and target times
	ulong TimeDifference = abs(SourceTime - TargetTime);

	//determine the weight change
	float WeightChange = min(exp(-0.05 * TimeDifference), 0.05 * TimeDifference);
	//branchlessly determine if the change should be positive or negative
	WeightChange -= (SourceTime < TargetTime) * 2 * WeightChange;

	//alter the weight change to keep weights from spiraling out of control
	WeightChange *= exp(0.5 * -abs(OriginalWeight)) * (4 - abs(OriginalWeight)) * 0.25f;

	//return the new weight
	return Originalweight + WeightChange;
}

//kernel entrypoint
__kernel void Main(
	__global unsigned long long SourceTimes*, 
	__global unsigned long long TargetTimes*,
	__global float Weights*,
	__global float NewWeights*)
{
	//get the ID of this kernel
	int ID = get_global_id(0);
	
	//set this kernel's weight to the new weight calculated by STDP
	NewWeights[ID] = STDPFunction(SourceTimes[ID], TargetTimes[ID], Weights[ID]);
}
