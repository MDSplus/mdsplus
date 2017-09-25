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

public class Dictionary extends Apd
{
    public Dictionary()
    {
        this(new Data[0]);
    }
    public Dictionary(Data [] descs)
    {
        super(descs, null, null, null, null);
    }
    public Dictionary(Data [] descs, Data help, Data units, Data error, Data validation)
    {
        super(descs, help, units, error, validation);
        dtype = DTYPE_DICTIONARY;
    }
    
    public static Data getData(Data [] descs, Data help, Data units, Data error, Data validation)
    {
        return new Dictionary(descs, help, units, error, validation);
    }
    public int len()
    {
        return nDescs/2;
    }    
    public Data getItem(String key)
    {
        for(int i = 0; i < len(); i++)
        {
            if(key.equals(descs[2*i]))
                return descs[2*i+1];
        }
        return null;
    }
    
    public void setItem(String key, Data data)
    {
        for(int i = 0; i < len(); i++)
        {
            if(key.equals(descs[2*i]))
            {
                descs[2*i+1] = data;
                return;
            }
        }
        resizeDescs(nDescs+2);
        descs[nDescs] = key;
        descs[nDescs+1] = data;
        nDescs+=2;
    }
}