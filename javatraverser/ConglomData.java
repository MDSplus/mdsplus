//package jTraverser;

public class ConglomData extends CompoundData
{
    public static Data getData() {return new ConglomData(); }
    public ConglomData() {dtype = DTYPE_CONGLOM; }
    public ConglomData(Data image, Data model, Data name, Data qualifiers)
    {
	dtype = DTYPE_CONGLOM;
	descs = new Data[4];
	descs[0] = image;
	descs[1] = model;
	descs[2] = name;
	descs[3] = qualifiers;
    }
    public final Data getImage() {return descs[0]; }
    public final Data getModel() {return descs[1]; }
    public final Data getName() {return descs[2]; }
    public final Data getQualifiers() {return descs[3]; }
}