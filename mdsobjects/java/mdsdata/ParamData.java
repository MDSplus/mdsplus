package mdsdata;

public class ParamData extends CompoundData
{
    public static Data getData()
    {
	return new ParamData();
    }
    public ParamData()
    {
        dtype = Data.DTYPE_PARAM;
    }
    public ParamData(Data value, Data help, Data validation)
    {
        dtype = Data.DTYPE_PARAM;
        descs = new Data[3];
        descs[0] = value;
        descs[1] = help;
        descs[2] = validation;
    }
    public Data getValue() {return descs[0];}
    public Data getHelp() { return descs[1];}
    public Data getValidation() { return descs[2];}
    public void setValue(Data value) {descs[0] = value;}
    public void setHelp(Data help) {descs[1] = help;}
    public void setValidation(Data validation) {descs[2] = validation;}
}
