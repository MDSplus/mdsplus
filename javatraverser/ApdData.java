package jTraverser;

class ApdData extends Data
{
    Data[] descs;
    
    public static Data getData(Data descs[]) {return new ApdData(descs); }
    public boolean isAtomic() {return false; }
    public ApdData()
    {
	dclass = CLASS_APD;
    }
    public ApdData(Data descs[])
    {
	this.descs = descs;
    }
}