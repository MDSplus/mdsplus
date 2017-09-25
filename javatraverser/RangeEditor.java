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
import javax.swing.*;
import java.awt.*;

public class RangeEditor extends JPanel implements Editor
{
    RangeData range;
    LabeledExprEditor begin_edit, end_edit, delta_edit;
    
    public RangeEditor() {this(null);}
    public RangeEditor(RangeData range)
    {
	this.range = range;
	if(this.range == null)
	{
	    this.range = new RangeData(null, null, null);
	}
	GridLayout gl = new GridLayout(3, 1);
	gl.setVgap(0);
	setLayout(gl);
	begin_edit = new LabeledExprEditor( "Start", new ExprEditor(
	    this.range.getBegin(), false));
	add(begin_edit);
	end_edit = new LabeledExprEditor( "End", new ExprEditor(
	    this.range.getEnd(), false));
	add(end_edit);
	delta_edit = new LabeledExprEditor( "Increment", new ExprEditor(
	    this.range.getDelta(), false));
	add(delta_edit);
    }
    
    public void reset()
    {
	begin_edit.reset();
	end_edit.reset();
	delta_edit.reset();
    }
    
    public Data getData()
    {
	return new RangeData(begin_edit.getData(), end_edit.getData(), delta_edit.getData());
    }
	
    public void setData(Data data)
    {
	this.range = (RangeData)data;
	if(this.range == null)
	{
	    this.range = new RangeData(null, null, null);
	}
	begin_edit.setData(range.getBegin());
	end_edit.setData(range.getEnd());
	delta_edit.setData(range.getDelta());
	reset();
    }
    
    public void setEditable(boolean editable)
    {
	if(begin_edit != null) begin_edit.setEditable(editable);
	if(end_edit != null) end_edit.setEditable(editable);
	if(delta_edit != null) delta_edit.setEditable(editable);
    }

}