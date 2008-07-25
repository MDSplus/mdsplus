package mdsdata;

public class CompoundData extends Data
{
    int opcode;
    Data descs[];
    public CompoundData() 
    {
        clazz = Data.CLASS_R;
    }
    public CompoundData(Data descs[])
    {
        clazz = Data.CLASS_R;
        this.descs = descs;
    }
}
