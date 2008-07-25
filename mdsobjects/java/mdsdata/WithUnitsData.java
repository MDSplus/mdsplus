package mdsdata;

public class WithUnitsData extends CompoundData
{
    public static Data getData()
    {
	return new WithUnitsData();
    }
    public WithUnitsData()
    {
        dtype = Data.DTYPE_WITH_UNITS;
    }
    public WithUnitsData(Data data, Data units)
    {
        dtype = Data.DTYPE_WITH_UNITS;
        descs = new Data[2];
        descs[0] = data;
        descs[1] = units;
    }
    public Data getDatum() {return descs[0];}
    public Data getUnits() { return descs[1];}
    public void setData(Data data) {descs[0] = data;}
    public void setUnits(Data units) {descs[1] = units;}
}
