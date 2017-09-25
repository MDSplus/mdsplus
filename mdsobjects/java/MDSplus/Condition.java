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
 * Class or DTYPE_CONDITION
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.34
 */
public class Condition extends Compound 
{
    public Condition(int opcode, Data arg,
            Data help, Data units, Data error, Data validation)
    {
        super(help, units, error, validation);
        this.opcode = opcode;
        clazz = CLASS_R;
        dtype = DTYPE_CONDITION;
        descs = new Data[1];
        descs[0] = arg;
    }
    public Condition(int opcode, Data arg)
    {
        this(opcode, arg, null, null, null, null);
    }

    public Condition(Data help, Data units, Data error, Data validation)
    {
        super(help, units, error, validation);
        clazz = CLASS_R;
        dtype = DTYPE_CONDITION;
        descs = new Data[0];
        opcode = 0;
    }
    public static Condition getData(Data help, Data units, Data error, Data validation)
    {
        return new Condition(help, units, error, validation);
    }


    public int getOpcode()
    {
            return opcode;
    }

    public Data getArg()
    {
            return descs[0];
    }

    /**
     * 
     * @param opcode
     */
    public void setOcode(int opcode)
    {
        this.opcode = opcode;
    }

    /**
     * 
     * @param data
     */
    public void setArg(Data data)
    {
        descs = new Data[]{data};
    }

}