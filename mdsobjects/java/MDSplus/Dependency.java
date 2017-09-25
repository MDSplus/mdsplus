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
 * Class for DTYPE_DEPENDENCY
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.35
 */
public class Dependency extends Compound 
{

    public Dependency(int opcode, Data arg1, Data arg2, 
            Data help, Data units, Data error, Data validation)
    {
        super(help, units, error, validation);
        this.opcode = opcode;
        clazz = CLASS_R;
        dtype = DTYPE_DEPENDENCY;
        descs = new Data[2];
        descs[0] = arg1;
        descs[1] = arg2;
    }
    public Dependency(int opcode, Data arg1, Data arg2)
    {
        this(opcode, arg1, arg2, null, null, null, null);
    }

    public Dependency(Data help, Data units, Data error, Data validation)
    {
        super(help, units, error, validation);
        clazz = CLASS_R;
        dtype = DTYPE_DEPENDENCY;
        descs = new Data[2];
        opcode = 0;
    }
    public static Dependency getData(Data help, Data units, Data error, Data validation)
    {
        return new Dependency(help, units, error, validation);
    }



    public Data getArg1()
    {
        return descs[0];
    }

    public Data getArg2()
    {
        return descs[1];
    }

    /**
      * @param data
     */
    public void setArg1(Data data)
    {
        descs[0] = data;
    }

    /**
     * 
     * @param data
     */
    public void setArg2(Data data)
    {
        descs[1] = data;
    }
}