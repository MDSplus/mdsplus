package mdsdata;

public class SignalData extends CompoundData
{
    public static Data getData()
    {
	return new SignalData();
    }
    public SignalData()
    {
        dtype = Data.DTYPE_SIGNAL;
    }
    public SignalData(Data value, Data raw, Data dimension)
    {
        dtype = Data.DTYPE_SIGNAL;
        descs = new Data[3];
        descs[0] = value;
        descs[1] = raw;
        descs[2] = dimension;
    }
    public SignalData(Data value, Data raw, Data dimensions[])
    {
        dtype = Data.DTYPE_SIGNAL;
        descs = new Data[2+dimensions.length];
        descs[0] = value;
        descs[1] = raw;
        for(int i = 0; i < dimensions.length; i++)
            descs[2+i] = dimensions[i];
    }
    public Data getValue() {return descs[0];}
    public Data getRaw() { return descs[1];}
    public Data getDimension() {return descs[2];}
    public Data getDimensionAt(int idx) {return descs[2+idx];}
    public int getNumDimensions() {return descs.length - 1;}
    
    public void setValue(Data value) {descs[0] = value;}
    public void setRaw(Data raw) {descs[1] = raw;}
    public void setDimension(Data dimension) {descs[2] = dimension;}
    public void setDimensionAt(Data dimension, int idx) {descs[2+idx] = dimension;}
 }
