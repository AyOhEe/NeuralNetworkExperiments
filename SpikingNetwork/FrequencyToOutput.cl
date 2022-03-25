__kernel void Main(__global float* FrequencyBuffer, __global float* OutputBuffer, __constant ulong UpdateStep)
{
	//get the kernel id
	int ID = get_global_id(0);

	//calculate the current value
	OutputBuffer[ID] = sin((float)(2 * M_PI_F * FrequencyBuffer[ID] * UpdateStep * 0.01f));
}