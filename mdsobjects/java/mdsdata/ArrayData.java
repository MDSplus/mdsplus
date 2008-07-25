package mdsdata;

public class ArrayData extends Data 
{
    int dims[];
    public ArrayData(int dims[])
    {
        clazz = Data.CLASS_A;
        this.dims = new int[dims.length];
        for(int i = 0; i < dims.length; i++)
            this.dims[i] = dims[i];
    }
    public ArrayData(int dim)
    {
        clazz = Data.CLASS_A;
        dims = new int[1];
        dims[0] = dim;
    }
}
