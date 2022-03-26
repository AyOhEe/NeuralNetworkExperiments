__kernel void Main(__global float* HistoryBuffer, __global float2* WorkingBuffer, __constant float FrequencyBinSize) 
{
	/*-----FOURIER CALCULATIONS-----*/
	//get the ids of this kernel
	int HistoryValueID = get_global_id(0);
	int FrequencyBandID = get_global_id(1);
	//get the working id for this kernel
	int WorkingID = HistoryValueID + (get_global_size(0) * FrequencyBandID);

	//get the total number of history values we have
	int N = get_global_size(0);

	//get the frequency that this kernel is looking for
	float LookingFrequency = *FrequencyBinSize * FrequencyBandID;

	//do the fourier transform for this datapoint and frequency band
	float FourierStep = LookingFrequency * HistoryValueID * (M_PI_F * 2)/N;
	float a = HistoryBuffer[HistoryValueID] * cos((float)FourierStep);
	float b = HistoryBuffer[HistoryValueID] * -sin((float)FourierStep);
	WorkingBuffer[WorkingID] = (float2)(a, b);
}