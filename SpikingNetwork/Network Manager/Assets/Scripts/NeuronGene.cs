using System;

[Serializable]
public struct NeuronGene
{
    //gene values
    public bool Type;
    public bool OutputType;
    public float MembraneResistance;
    public float ThresholdOffset;

    //creates a Neuron gene from a byte list
    NeuronGene(byte[] bytes)
    {
        //get our values
        Type = (bytes[0] & 0b10000000) == 0b10000000;
        OutputType = (bytes[0] & 0b01000000) == 0b01000000;
        MembraneResistance = BitConverter.ToSingle(bytes, 1);
        ThresholdOffset = BitConverter.ToSingle(bytes, 5);
    }
}