package mdsdata;

public class FunctionData extends CompoundData
{
    public static Data getData()
    {
	return new FunctionData();
    }
    public FunctionData()
    {
        dtype = Data.DTYPE_FUNCTION;
    }
    public FunctionData(byte opcode, Data[] args)
    {
        this.opcode = opcode;
        dtype = Data.DTYPE_FUNCTION;
        descs = new Data[args.length];
        for(int i = 0; i < args.length; i++)
            descs[i] = args[i];
    }
    public Data [] getArgs() {return descs;}
    public void setArgs(Data[] args)
    {
        descs = new Data[args.length];
        for(int i = 0; i < args.length; i++)
            descs[i] = args[i];
    }
    public int getOpcode() {return opcode;}
    public void setOpcode(byte opcode) { this.opcode = opcode;}
}
