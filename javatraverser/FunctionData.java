//package jTraverser;

public class FunctionData extends CompoundData
{
    public static Data getData() {return new FunctionData(); }
    public FunctionData() {dtype = DTYPE_FUNCTION; }

    public FunctionData(int opcode, Data [] args)
    {
	dtype = DTYPE_FUNCTION;
	this.opcode = opcode;
	if(args == null)
	{
	    return;
	}
	descs = new Data[args.length];
	for(int i = 0; i < args.length; i++)
	    descs[i] = args[i];
    }
    public final int getOpcode()  {return opcode; }
    public final Data[] getArgs() {return descs; }
}