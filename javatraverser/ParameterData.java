//package jTraverser;

public class ParameterData extends CompoundData
{
    public static Data getData() {return new ParameterData(); }
    public ParameterData() {dtype = DTYPE_PARAM; }
    public ParameterData(Data data, Data help, Data validation)
    {
	dtype = DTYPE_PARAM;
	descs = new Data[3];
	descs[0] = data;
	descs[1] = help;
	descs[2] = validation;
    }
    public final Data getDatum() {return descs[0]; }
    public final Data getHelp() { return descs[1]; }
    public final Data getValidation() {return descs[2]; }
}


