package mdsdata;

public class ConglomData extends CompoundData
{    
    public static Data getData()
    {
	return new ConglomData();
    }
    public ConglomData()
    {
        dtype = Data.DTYPE_CONGLOM;
    }
    public ConglomData(Data image, Data model, Data name, Data qualifiers)
    {
        dtype = Data.DTYPE_CONGLOM;
        descs = new Data[4];
        descs[0] = image;
        descs[1] = model;
        descs[2] = name;
        descs[3] = qualifiers;
    }
    public Data getImage() {return descs[0];}
    public Data getModel() { return descs[1];}
    public Data getName() { return descs[2];}
    public Data getQualifiers() { return descs[3];}
    public void setImage(Data image) {descs[0] = image;}
    public void setHelp(Data model) {descs[1] = model;}
    public void setName(Data name) {descs[2] = name;}
    public void setQualifiers(Data qualifiers) {descs[3] = qualifiers;}
}
