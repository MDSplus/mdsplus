import java.io.*;
import java.awt.*;
/**
 **Classe relativa alle singole wave con i relativi
 **parametri di configurazione. Utilizata dalla classe
 **SetupData
*/ 
class WaveSetupData {
   public static final int UNDEF_SHOT = -99999;
   public static final int MAX_NUM_SHOT = 30;   
// public static final int MAX_COLOR  = 12;
//  public static final Color[]  COLOR_SET = {Color.black, Color.blue, Color.cyan, Color.darkGray,
	//				     Color.gray, Color.green, Color.lightGray, 
	//				     Color.magenta, Color.orange, Color.pink, 
	//				     Color.red, Color.yellow};
  
   String  x_expr;
   String  y_expr;
   int     shot;
// Color   color; 
   int     color_idx;
   boolean interpolate;
   int     marker;
   String  up_err;
   String  low_err;

/* 
   public int colorToIdx111()
   {   
	int c_idx;
	
	if(color == null)
	    return 0;
	for(c_idx = 0;  c_idx < MAX_COLOR && !color.equals(COLOR_SET[c_idx]); c_idx++);
	c_idx = c_idx % MAX_COLOR;
	return c_idx;
   }
  
   public int colorToIdx111(Color c)
   {   
	int c_idx;
	for(c_idx = 0;  c_idx < MAX_COLOR && c != COLOR_SET[c_idx]; c_idx++);
	c_idx = c_idx % MAX_COLOR;
	return c_idx;
   }
*/

   public boolean equals(WaveSetupData ws)
   {
     if(x_expr != null && y_expr != null){
	if(!y_expr.equals(ws.y_expr) || !x_expr.equals(ws.x_expr)) 
	    return false;
	else
	    return true;
     } else {
	if(x_expr != null)
	    if(!x_expr.equals(ws.x_expr)) 
		return false;
	    else
		return true;
	 else
	    if(!y_expr.equals(ws.y_expr)) 
		return false;
	    else
		return true;
     }	      
   }
    
   public void copy(WaveSetupData ws)
   {
	if(ws.x_expr != null)
	   x_expr = new String(ws.x_expr);
	if(ws.y_expr != null)
	   y_expr = new String(ws.y_expr);
    	if(ws.up_err != null)
	   up_err = new String(ws.up_err);
	if(ws.low_err != null)
	   low_err = new String(ws.low_err);
	shot          = ws.shot;
//	color         = ws.color;
	color_idx     = ws.color_idx;
    	interpolate   = ws.interpolate;
	marker        = ws.marker;
   } 
}
