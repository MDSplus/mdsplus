//package jTraverser;

public class ConditionData extends CompoundData
{
    public static Data getData() {return new ConditionData(); }
    public ConditionData() {dtype = DTYPE_CONDITION; }
    public ConditionData(int modifier, Data argument)
    {
	dtype = DTYPE_CONDITION;
	opcode = modifier;
	descs = new Data[1];
	descs[0] = argument;
    }
    public ConditionData(Data argument)
    {
	dtype = DTYPE_CONDITION;
	opcode = 0;
	descs = new Data[1];
	descs[0] = argument;
    }

    public final int getModifier() {return opcode; }
    public final Data getArgument() {return descs[0]; }

    public static final int NEGATE_CONDITION = 7;
    public static final int IGNORE_UNDEFINED = 8;
    public static final int IGNORE_STATUS = 9;
}