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
 * Class description of DTYPE_B
 * 
 * Constructors:Uint8(byte)
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.52
 */
public class Uint8 extends Scalar {
    byte datum;

    public Uint8(byte datum)        
    {
        this(datum, null, null, null, null);
    }
    public Uint8(byte datum, Data help, Data units, Data error, Data validation)
    {
        super(help, units, error, validation);
        clazz = CLASS_S;
        dtype = DTYPE_BU;
        this.datum = datum;
    }
    public boolean equals(Object data)
    {
        if(!(data instanceof Uint8))
            return false;
        return((Uint8)data).datum == datum;
    }
    public static Data getData(byte datum, Data help, Data units, Data error, Data validation) 
    { 
        return new Uint8(datum, help, units, error, validation);
    }

    public  byte getByte() throws MdsException
    {
        return datum;
    }

    /**
     * Convert this data into a short. Implemented at this class level by returning TDI
     * data(WORD(this)).If data() fails or the returned class is not scalar, generated
     * an exception.
     */
    public  short getShort() throws MdsException
    {
        return (short)datum;
    }

    /**
     * Convert this data into a int. Implemented at this class level by returning TDI
     * data(LONG(this)).If data() fails or the returned class is not scalar, generated
     * an exception.
     */
    public  int getInt() throws MdsException
    {
        return (int)datum;
    }


    /**
     * Convert this data into a long. Implemented at this class level by returning TDI
     * data(QUADWORD(this)).If data() fails or the returned class is not scalar,
     * generated an exception.
     */
    public  long getLong() throws MdsException
    {
        return (long)datum;
    }

    /**
     * Convert this data into a float. Implemented at this class level by returning TDI
     * data(F_FLOAT(this)).If data() fails or the returned class is not scalar,
     * generated an exception.
     */
    public  float getFloat()throws MdsException
    {
        return (float)datum;
    }

    /**
     * Convert this data into a double. Implemented at this class level by returning TDI
     * data(FT_FLOAT(this)). If data() fails or the returned class is not scalar,
     * generated an exception.
     */
    public  double getDouble()throws MdsException
    {
        return (double)datum;
    }


    /**
     * Convert this data into a byte array. Implemented at this class level by
     * returning TDI data(BYTE(this)). If data() fails or the returned class is not
     * array, generates an exception. In Java and C++ will return a 1 dimensional
     * array using row-first ordering if a multidimensional array. 
     */
    public  byte[] getByteArray() throws MdsException
    {
        return new byte[]{datum};
    }


    /**
     * Convert this data into a short array. Implemented at this class level by
     * returning TDI data(WORD(this)). If data() fails or the returned class is not
     * array, generates an exception. In Java and C++ will return a 1 dimensional
     * array using row-first ordering if a multidimensional array. 
     */
    public  short[] getShortArray()throws MdsException
    {
        return new short[]{(short)datum};
    }


    /**
     * Convert this data into a int array. Implemented at this class level by
     * returning TDI data (LONG(this)). If data() fails or the returned class is not
     * array, generates an exception. In Java and C++ will return a 1 dimensional
     * array using row-first ordering if a multidimensional array. 
     */
    public  int[] getIntArray()throws MdsException
    {
        return new int[]{(int)datum};
    }

    /**
     * Convert this data into a long array. Implemented at this class level by
     * returning TDI data(QUADWORD(this)). If data() fails or the returned class is
     * not array, generates an exception. In Java and C++ will return a 1 dimensional
     * array using row-first ordering if a multidimensional array. 
     */
    public  long[] getLongArray()throws MdsException
    {
        return new long[]{(long)datum};
    }

    /**
     * Convert this data into a float array. Implemented at this class level by
     * returning TDI data(QUADWORD(this)). If data() fails or the returned class is
     * not array, generates an exception. In Java and C++ will return a 1 dimensional
     * array using row-first ordering if a multidimensional array. 
     */
    public  float[] getFloatArray()throws MdsException
    {
        return new float[]{(float)datum};
    }

    /**
     * Convert this data into a long array. Implemented at this class level by
     * returning TDI data(QUADWORD(this)). If data() fails or the returned class is
     * not array, generates an exception. In Java and C++ will return a 1 dimensional
     * array using row-first ordering if a multidimensional array. 
     */
    public  double[] getDoubleArray()throws MdsException
    {
        return new double[]{(double)datum};
    }
    public int getSizeInBytes() {return 1;}
}