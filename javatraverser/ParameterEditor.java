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

public class ParameterEditor extends JPanel implements Editor
{
    ExprEditor expr, help, validity;
    public ParameterEditor()
    {
        this(new ExprEditor(null, false), new ExprEditor(null, true), new ExprEditor(null, false));
    }
    public ParameterEditor(ExprEditor expr, ExprEditor help, ExprEditor validity)
    {
        this.expr = expr;
        this.help = help;
        this.validity = validity;
        setLayout(new BorderLayout());
        add(new LabeledExprEditor("Data", expr), "North");
        add(new LabeledExprEditor("Help", help), "Center");
        add(new LabeledExprEditor("Validity", validity), "South");
    }

    public void reset()
    {
      expr.reset();
      help.reset();
      validity.reset();
    }
    public void setEditable(boolean editable)
    {
      expr.setEditable(editable);
      help.setEditable(editable);
      validity.setEditable(editable);
    }

    public Data getData() {return new ParameterData(expr.getData(), help.getData(), validity.getData());}
    public void setData(Data data)
    {
      if (data instanceof ParameterData) {
        expr.setData( ( (ParameterData) data).getDatum());
        help.setData( ( (ParameterData) data).getHelp());
        validity.setData( ( (ParameterData) data).getValidation());
      }
    }
}
