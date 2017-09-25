/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
package MDSplus;

/**
 * Array description for  DTYPE_B
 * 
 * Constructor: Int8Array(byte[])
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.39
 */
public class Int8Array extends Array 
{
    byte [] datum;
    public Int8Array(byte[] inDatum)
    {
        help = null;
        units = null;
        error = null;
        validation = null;
        clazz = CLASS_A;
        dtype = DTYPE_B;
        dims = new int[1];
        dims[0] = inDatum.length;
        datum = new byte[inDatum.length];
        System.arraycopy(inDatum, 0, datum, 0, inDatum.length);
    }
    public Int8Array(byte[] inDatum, int []dims) throws MdsException
    {
        this(inDatum, dims, null, null, null, null);
    }
         //Row first copy of tridimensionalal array
    private static int getTotSize(byte arr[][])
    {
        int totSize = 0;
        for(int i = 0; i < arr.length; i++)
        {
            totSize += arr[i].length;
        }
        return totSize;
    }
    private static byte[] copyToLinearized(byte arr[][], byte linArr[])
    {
        int currSize = 0;
        for(int i = 0; i < arr.length; i++)
        {
            System.arraycopy(arr[i], 0, linArr, currSize, arr[i].length);
            currSize+= arr[i].length;
         }
        return linArr;
    }
        //NOTE same diemnsion assumed along sub arrays
    private static int[] getDims(byte arr[][])
    {
        int d[] = new int[2];
        d[0] = arr.length;
        d[1] = arr[0].length;
        return d;
    }


    public Int8Array(byte [][] inDatum) throws MdsException
    {
        this(copyToLinearized(inDatum, new byte[(getTotSize(inDatum))]), getDims(inDatum));
    }
    //NOTE same dimension assumed along sub arrays
         //Get dimension of tridimensional array
    private static int getTotSize(byte arr[][][])
    {
        int totSize = 0;
        for(int i = 0; i < arr.length; i++)
        {
            for(int j = 0; j < arr[i].length; j++)
                totSize += arr[i][j].length;
        }
        return totSize;
    }
        //Row first copy of tridimensionalal array
    private static byte[] copyToLinearized(byte arr[][][], byte linArr[])
    {
        int currSize = 0;
        for(int i = 0; i < arr.length; i++)
        {
            for(int j = 0; j < arr[i].length; j++)
            {
                System.arraycopy(linArr, currSize, arr[i][j], 0, arr[i][j].length);
                currSize+= arr[i][j].length;
            }
        }
        return linArr;
    }
        //NOTE same diemnsion assumed along sub arrays
    private static int[] getDims(byte arr[][][])
    {
        int d[] = new int[3];
        d[0] = arr.length;
        d[1] = arr[0].length;
        d[2] = arr[0][0].length;
        return d;
    }


    public Int8Array(byte [][][] inDatum) throws MdsException
    {
        this(copyToLinearized(inDatum, new byte[(getTotSize(inDatum))]), getDims(inDatum));
    }
    public Int8Array(byte[] inDatum, int []dims, Data help, Data units, Data error, Data validation) throws MdsException
    {
        super(dims, help, units, error, validation);
        clazz = CLASS_A;
        dtype = DTYPE_B;
        datum = new byte[inDatum.length];
        System.arraycopy(inDatum, 0, datum, 0, inDatum.length);
        setShape(dims);
    }

    public boolean equals(Object obj)
    {
        if(!(obj instanceof Int8Array))
            return false;
        Int8Array data = (Int8Array)obj;
        if(data.dims.length != dims.length)
            return false;
        for(int i = 0; i < dims.length; i++)
            if(data.dims[i] != dims[i])
                return false;
        for(int i = 0; i < datum.length; i++)
            if(data.datum[i] != datum[i])
                return false;
        return true;
    }
    
    public static Data getData(byte []datum, int []dims, Data help, Data units, Data error, Data validation)
    {
        try {
            return new Int8Array(datum, dims, help, units, error, validation);
        }catch(Exception exc){return null;}
    }
    public Data getElementAt(int idx)
    {
        return new Int8(datum[idx]);
    }
    
    public int getSize(){return datum.length;}
    protected Array getPortionAt(int startIdx, int []newDims, int newSize) throws MdsException
    {
        byte newDatum[] = new byte[newSize];
        System.arraycopy(datum, startIdx, newDatum, 0, newSize);
        return new Int8Array(newDatum, newDims);
    }
    public void setElementAt(int idx, Data data) throws MdsException
    {
        datum[idx] = data.getByte();
    }
    protected  void setPortionAt(Array data, int startIdx, int size) throws MdsException
    {
        byte [] newDatum = data.getByteArray();
        System.arraycopy(datum, startIdx, newDatum, 0, size);
    }
    public byte[] getByteArray()
    {
        byte[] retDatum = new byte[datum.length];
        for(int i = 0; i < datum.length; i++)
            retDatum[i] = (byte)datum[i];
        return retDatum;
    }
    public short[] getShortArray()
    {
        short[] retDatum = new short[datum.length];
        for(int i = 0; i < datum.length; i++)
            retDatum[i] = (short)datum[i];
        return retDatum;
    }
    public int[] getIntArray() 
    {
        int[] retDatum = new int[datum.length];
        for(int i = 0; i < datum.length; i++)
            retDatum[i] = (int)datum[i];
        return retDatum;
    }
    public long[] getLongArray()
    {
        long[] retDatum = new long[datum.length];
        for(int i = 0; i < datum.length; i++)
            retDatum[i] = (long)datum[i];
        return retDatum;
    }
    public float[] getFloatArray()
    {
        float[] retDatum = new float[datum.length];
        for(int i = 0; i < datum.length; i++)
            retDatum[i] = (float)datum[i];
        return retDatum;
    }
    public double[] getDoubleArray() 
    {
        double[] retDatum = new double[datum.length];
        for(int i = 0; i < datum.length; i++)
            retDatum[i] = (double)datum[i];
        return retDatum;
    }
    public java.lang.String[] getStringArray() throws MdsException
    {
        throw new MdsException("Cannot convert byte array to string array");
    }
    public int getSizeInBytes() {return getSize() * 1;}

}