package mdsdata;

public class DependencyData extends CompoundData
{
    public static Data getData(byte data)
    {
	return new DependencyData();
    }
    public DependencyData()
    {
        dtype = Data.DTYPE_DEPENDENCY;
    }
    public DependencyData(byte opcode, Data arg1, Data arg2)
    {
        dtype = Data.DTYPE_DEPENDENCY;
        this.opcode = opcode;
        descs = new Data[2];
        descs[0] = arg1;
        descs[1] = arg2;
    }
    public int getOpcode() { return opcode;}
    public Data getArg1() {return descs[0];}
    public Data getArg2() { return descs[1];}
    public void setOpcode(byte opcode){this.opcode = opcode;}
    public void setArg1(Data arg1) {descs[0] = arg1;}
    public void setArg2(Data arg2) {descs[1] = arg2;}
}
