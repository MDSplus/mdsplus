package mdsdata;

public class ConditionData extends CompoundData
{
    public static Data getData(byte data)
    {
	return new ConditionData();
    }
    public ConditionData()
    {
        dtype = Data.DTYPE_CONDITION;
    }
    public ConditionData(byte opcode, Data arg)
    {
        dtype = Data.DTYPE_CONDITION;
        this.opcode = opcode;
        descs = new Data[1];
        descs[0] = arg;
    }
    public int getOpcode() { return opcode;}
    public Data getArg() {return descs[0];}
    public void setOpcode(byte opcode){this.opcode = opcode;}
    public void setArg(Data arg) {descs[0] = arg;}
}
