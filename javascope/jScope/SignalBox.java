package jScope;

/* $Id$ */
import java.util.Hashtable;

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
