using System;

[Serializable]
public struct ConnectionGene
{
    //gene values
    public bool SourceType;
    public bool TargetType;
    public uint SourceIndex;
    public uint TargetIndex;
    public float Weight;

    ConnectionGene(byte[] bytes) 
    {
        //get our values
        SourceType = (bytes[0] & 0b10000000) == 0b10000000;
        TargetType = (bytes[0] & 0b01000000) == 0b01000000;
        SourceIndex = BitConverter.ToUInt32(bytes, 1);
        TargetIndex = BitConverter.ToUInt32(bytes, 5);
        Weight = BitConverter.ToSingle(bytes, 9);
    }
}
