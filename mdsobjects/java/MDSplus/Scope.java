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

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author manduchi
 */
import jScope.*;

public class Scope
{
    CompositeWaveDisplay window;
    public Scope(java.lang.String name, int x, int y, int width, int height)
    {
           window = CompositeWaveDisplay.createWindow(name);
           window.showWindow(x,y,width, height);
    }
    public void plot(Data x, Data y, int row, int col, java.lang.String color) throws Exception
    {
        window.removeAllSignals(row, col);
        window.addSignal(x.getFloatArray(), y.getFloatArray(), row, col, color,"", true ,0);
    }
    public void oplot(Data x, Data y, int row, int col, java.lang.String color) throws Exception
    {
        window.addSignal(x.getFloatArray(), y.getFloatArray(), row, col, color, "", true, 0);
    }


    //Test
    public static void main(java.lang.String args[])
    {
        Scope scope = new Scope("CACCA", 100, 100, 100, 100);
        float x[] = new float[1000];
        float y1[] = new float[1000];
        float y2[] = new float[1000];
        for(int i = 0; i < 1000; i++)
        {
            x[i] = i;
            y1[i] = x[i]*x[i];
            y2[i] = 2*y1[i];
        }
        try {
            scope.plot(new Float32Array(x), new Float32Array(y1) , 1, 1, "black");
            scope.oplot(new Float32Array(x), new Float32Array(y2) , 1, 1, "red");
        }catch(Exception exc)
        {System.err.println(exc);}
    }

}


