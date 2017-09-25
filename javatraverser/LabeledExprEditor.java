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

public class LabeledExprEditor extends JPanel implements Editor
{
    ExprEditor expr;
    public LabeledExprEditor(Data data)
    {
    this("Expression", new ExprEditor(data, (data != null && data.dtype == Data.DTYPE_T), 4, 20));
    }
    public LabeledExprEditor(String label_str)
    {
	this(label_str, new ExprEditor(null, false));
    }
    public LabeledExprEditor(String label_str, ExprEditor expr)
    {
	this.expr = expr;
	setLayout(new BorderLayout());
    setBorder(BorderFactory.createTitledBorder(label_str));
	add(expr, "Center");
    }

    public void reset()
    {
	expr.reset();
    }
    public void setEditable(boolean editable)
    {
	expr.setEditable(editable);
    }
    
    public Data getData() {return expr.getData();}
    public void setData(Data data) {expr.setData(data);}
    
}