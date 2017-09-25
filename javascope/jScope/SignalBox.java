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
package jScope;

/* $Id$ */
import java.util.*;
import javax.swing.*;

public class SignalBox
{
     
     class Sign
     {
        String x_expr;
        String y_expr;
        
        Sign(String x_expr, String y_expr)
        {
            this.x_expr = new String(x_expr == null ? "" : x_expr);
            this.y_expr = new String(y_expr == null ? "" : y_expr);
        }
        
        public String toString()
        {
            return y_expr+" "+x_expr;
        }
     }
    
     Hashtable<String, Sign> signals_name = new Hashtable<>();
     Object obj[];
     
     public void AddSignal(String x_expr, String y_expr)
     {
        if(x_expr == null && y_expr == null) return;
        
        String s = x_expr+y_expr;
        if(!signals_name.containsKey(s))
        {
            signals_name.put(s, new Sign(x_expr,y_expr));
            obj = (Object[])(signals_name.values()).toArray();
        }
     }
     
     public String getXexpr(int i)
     {
        return ((Sign)obj[i]).x_expr;
     }

     public String getYexpr(int i)
     {
        return ((Sign)obj[i]).y_expr;
     }

     public void removeExpr(int i)
     {
        RemoveSignal(((Sign)obj[i]).x_expr, ((Sign)obj[i]).y_expr);
     }


     public void RemoveSignal(String x_expr, String y_expr)
     {
        String s = x_expr+y_expr;
        if(signals_name.containsKey(s))
        {
            signals_name.remove(s);
            obj = (Object[])(signals_name.values()).toArray();
        }
     }
     
     public String toString()
     {
        if(obj == null) return "";
        String out = new String();
        for(int i = 0; i < obj.length;i++)
            out = out +"\n"+obj[i];
        return out;
     }
    
}
