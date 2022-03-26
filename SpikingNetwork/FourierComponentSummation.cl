float f2Mag(float2 v)
{
	return sqrt((float)((v.x * v.x) + (v.y * v.y)));
}

__kernel void Main(__global float2* WorkingBuffer, __constant float* NDatapoints, __global float* CondensedBuffer) 
{
	/*-----FOURIER COMPONENT SUMMATION-----*/
	//get the row we're summing up
	int ColumnID = get_global_size(0);

	//start summing up the data
	int SumLocation = ColumnID * *NDatapoints;
	for(int i = 1; i < *NDatapoints; i++)
	{
		WorkingBuffer[SumLocation] += WorkingBuffer[SumLocation + i];
	}

	//store the magnitude of the sum in the condensed buffer
	CondensedBuffer[ColumnID] = f2Mag(WorkingBuffer[SumLocation]);
}