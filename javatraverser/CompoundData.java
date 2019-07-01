//package jTraverser;

public class CompoundData extends Data
{
    Data[] descs;
    int opcode = 0;

    public boolean isAtomic() {return false; }
    public CompoundData()
    {
	dclass = CLASS_R;
    }
    public CompoundData(Data descs[])
    {
	this.descs = descs;
    }
    public Data [] getDescs() {return descs; }
}