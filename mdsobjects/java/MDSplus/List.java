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

public class List extends Apd
{
    public List()
    {
        this(new Data[0]);
    }
    public List(Data [] descs)
    {
        super(descs, null, null, null, null);
    }
    public List(Data [] descs, Data help, Data units, Data error, Data validation)
    {
        super(descs, help, units, error, validation);
        dtype = DTYPE_LIST;
    }
    
    public static Data getData(Data [] descs, Data help, Data units, Data error, Data validation)
    {
        return new List(descs, help, units, error, validation);
    }

    public int len()
    {
        return nDescs;
    }    
    public void append(Data data)
    {
        setDescAt(len(), data);
    }
    public void remove(Data data)
    {
        for(int i = 0; i < nDescs; i++)
        {
            if(descs[i].equals(data))
            {
                for(int j = i; j < nDescs-1; j++)
                    descs[j] = descs[j+1];
                nDescs--;
            }
        }
    }
    public void remove(int idx)
    {
        if(idx < 0 || idx >= nDescs) return;
        for(int j = idx; j < nDescs-1; j++)
            descs[j] = descs[j+1];
        nDescs--;
    }
    public void insert(int idx, Data data) throws MdsException
    {
        if(idx < 0 || idx > nDescs)
            throw new MdsException("Index out of range in List.insert()");
        resizeDescs(nDescs+1);
        for(int i = nDescs; i > idx; i--)
            descs[i] = descs[i-1];
        descs[idx] = data;
        nDescs++;
    }
    
    public Data getElementAt(int idx)
    {
        if(idx < 0 || idx >= nDescs)
            return null;
        return descs[idx];
    }    
}
