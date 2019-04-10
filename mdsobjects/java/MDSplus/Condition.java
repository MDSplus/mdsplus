package MDSplus;

/**
 * Class or DTYPE_CONDITION
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.34
 */
public class Condition extends Compound
{
    public Condition(int opcode, Data arg,
            Data help, Data units, Data error, Data validation)
    {
        super(help, units, error, validation);
        this.opcode = opcode;
        clazz = CLASS_R;
        dtype = DTYPE_CONDITION;
        descs = new Data[1];
        descs[0] = arg;
    }
    public Condition(int opcode, Data arg)
    {
        this(opcode, arg, null, null, null, null);
    }

    public Condition(Data help, Data units, Data error, Data validation)
    {
        super(help, units, error, validation);
        clazz = CLASS_R;
        dtype = DTYPE_CONDITION;
        descs = new Data[0];
        opcode = 0;
    }
    public static Condition getData(Data help, Data units, Data error, Data validation)
    {
        return new Condition(help, units, error, validation);
    }


    public int getOpcode()
    {
            return opcode;
    }

    public Data getArg()
    {
            return descs[0];
    }

    /**
     *
     * @param opcode
     */
    public void setOcode(int opcode)
    {
        this.opcode = opcode;
    }

    /**
     *
     * @param data
     */
    public void setArg(Data data)
    {
        descs = new Data[]{data};
    }

}