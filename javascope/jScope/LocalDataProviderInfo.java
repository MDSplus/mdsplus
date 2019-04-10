package jScope;

class LocalDataProviderInfo
{
    int dtype;
    int pixelSize;
    int [] dims;

    public LocalDataProviderInfo(int dtype, int pixelSize, int dims[])
    {
	this.dtype = dtype;
	this.pixelSize = pixelSize;
	this.dims = dims;
    }
}

