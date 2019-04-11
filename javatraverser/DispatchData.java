//package jTraverser;

public class DispatchData extends CompoundData
{
    public static Data getData() {return new DispatchData(); }
    public DispatchData() {dtype = DTYPE_DISPATCH; }
    public DispatchData(int type, Data ident, Data phase, Data when, Data completion)
    {
	dtype = DTYPE_DISPATCH;
	opcode = type;
	descs = new Data[4];
	descs[0] = ident;
	descs[1] = phase;
	descs[2] = when;
	descs[3] = completion;
    }
    public final int getType() {return  opcode; }
    public final Data getIdent() {return descs[0]; }
    public final Data getPhase() {return descs[1]; }
    public final Data getWhen()	 {return descs[2]; }
    public final Data getCompletion() {return descs[3]; }

    public static final int SCHED_NONE = 0x0;
    public static final int SCHED_ASYNCH = 1;//0x1000000;
    public static final int SCHED_SEQ = 2;//0x2000000;
    public static final int SCHED_COND = 3;//0x3000000;

}