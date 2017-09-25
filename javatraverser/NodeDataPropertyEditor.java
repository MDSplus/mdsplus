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
//package jTraverser;
import java.beans.*;
import java.awt.*;

public abstract class NodeDataPropertyEditor implements PropertyEditor {
    protected Data data;
    
    public void setValue(Object o){data = (Data)o;}
    public Object getValue() {return data;}
    public void setAsText(String s) 
    {
	try {
	    data = Tree.dataFromExpr(s);
	    } catch (Exception e) {data = null; }
    }
    public String getAsText() {
	try {
	    return Tree.dataToString(data);
	    } catch (Exception e) {return null; }
    }
    public String []getTags() {return null;}
    public boolean  supportsCustomEditor() {return true;}
    public abstract Component getCustomEditor(); //to be subclassed
    public boolean isPaintable() {return false; }
    public void paintValue(Graphics g, Rectangle r){}
    public String getJavaInitializationString() {return null; }
    // event notification not used here
    public void addPropertyChangeListener(PropertyChangeListener l){}
    public void removePropertyChangeListener(PropertyChangeListener l){}
}