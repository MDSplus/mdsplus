//package jTraverser;

public class WithUnitsData extends CompoundData
{
    public static Data getData() {return new WithUnitsData(); }
    public WithUnitsData() {dtype = DTYPE_WITH_UNITS; }
    public WithUnitsData(Data data, Data units)
    {
	dtype = DTYPE_WITH_UNITS;
	descs = new Data[2];
	descs[0] = data;
	descs[1] = units;
    }

    public final Data getDatum() {return descs[0]; }
    public final Data getUnits() {return descs[1]; }
}