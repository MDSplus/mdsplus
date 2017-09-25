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
import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.table.*;
import java.util.*;
import java.text.*;
import jScope.*;

public class DeviceWaveDisplay extends DeviceComponent
{
    protected Waveform wave;
    protected Data oldData;
    float x[] = null,y[] = null;
    protected int prefHeight = 200;

    public void setPrefHeight(int prefHeight)
    {
        this.prefHeight = prefHeight;
    }

    public int getPrefHeight(){return prefHeight;}
    public DeviceWaveDisplay()
    {}


    protected void initializeData(Data data, boolean is_on)
    {
        oldData = data;
        setLayout(new BorderLayout());
        wave = new Waveform();
        wave.setPreferredSize(new Dimension(300, 200));
        add(wave, "Center");
        displayData(data, is_on);
    }


    protected void displayData(Data data, boolean is_on)
    {
        try {
            Data xData = subtree.evaluateData(Data.fromExpr("DIM_OF(" + data +
                ")"), 0);
            Data yData = subtree.evaluateData(data, 0);
            x = xData.getFloatArray();
            y = yData.getFloatArray();
            wave.Update(x, y);
        }catch(Exception exc){}
    }

     protected Data getData()
    {
        return oldData;
    }

    protected boolean getState(){return true;}
    public void setEnabled(boolean state) {}
    public boolean isDataChanged() {return false;}

    void postApply()
    {
        displayData(oldData, true);
    }

    public void reset()
    {
    }

    public void print(Graphics g)
    {
        wave.paintComponent(g);
    }
}

