//package jTraverser;

public class DependencyData extends CompoundData
{
    int opcode;
    public static Data getData() {return new DependencyData(); }
    public DependencyData() {dtype = DTYPE_DEPENDENCY; }
    public DependencyData(int opcode, Data arg1, Data arg2)
    {
	dtype = DTYPE_DEPENDENCY;
	this.opcode = opcode;
	descs = new Data[2];
	descs[0] = arg1;
	descs[1] = arg2;
    }

    public final int getOpcode() {return opcode; }
    public final Data[] getArguments() {return descs; }

    public static final int DEPENDENCY_AND = 10;
    public static final int DEPENDENCY_OR = 11;
}