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
import java.awt.*;
import java.util.*;
public  class  DeviceWaveParameters extends DeviceParameters
{
     public DeviceWaveParameters(){}
     protected String getParameterName() {return "WAVE";}
     protected void addParameter(JPanel jp, NidData nidData)
     {
        try {
            NidData prevDefNid = subtree.getDefault(0);
            subtree.setDefault(nidData, 0);
            NidData currNid;
            currNid = subtree.resolve(new PathData(":DESCRIPTION"), 0);
            String description = subtree.evaluateData(currNid, 0).getString();
            NidData currXNid = subtree.resolve(new PathData(":X"), 0);
            DeviceField currXField = new DeviceField();
            currXField.setSubtree(subtree);
            currXField.setBaseNid(currXNid.getInt());
            currXField.setOffsetNid(0);
            currXField.setLabelString("X:");
            currXField.setNumCols(30);
            parameters.add(currXField);
            NidData currYNid = subtree.resolve(new PathData(":Y"), 0);
            DeviceField currYField = new DeviceField();
            currYField.setSubtree(subtree);
            currYField.setBaseNid(currYNid.getInt());
            currYField.setOffsetNid(0);
            currYField.setLabelString("Y:");
            currYField.setNumCols(30);
            parameters.add(currYField);
            JPanel jp1 = new JPanel();
            jp1.add(new JLabel(description));
            jp1.add(currXField);
            jp1.add(currYField);
            jp.add(jp1);
            currXField.configure(currXNid.getInt());
            currYField.configure(currYNid.getInt());
         }catch(Exception exc)
        {
            JOptionPane.showMessageDialog(null, "Error in DeviceWaveParameters.addParam: " + exc);
        }
    }
}





