import java.awt.*;
import java.awt.print.*;
import java.awt.geom.*;

public class WaveformContainer_2 extends WaveformContainer implements Printable 
{        
    
    public int print(Graphics g, PageFormat pf, int pageIndex) throws PrinterException 
    {
        int st_x = 0, st_y = 0;
        double height = pf.getImageableHeight();
        double width = pf.getImageableWidth();
        Graphics2D g2 = (Graphics2D)g;
        String ver = System.getProperty("java.version");


        if(pageIndex == 0)
        {

    //fix page margin error on jdk 1.2.X
            if(ver.indexOf("1.2") != -1)
            {
                if(pf.getOrientation() == PageFormat.LANDSCAPE)
                {
                    st_y = -13;
                    st_x =  15;
                    width -= 5;
                }
                else
                {
                    //st_x = 10;
                    st_y = -5;
                    width -= 25;
                    height -= 25;
                }
            }
            g2.translate(pf.getImageableX(), pf.getImageableY());
            PrintAll(g2, st_x, 
                         st_y,
                         (int)height, 
                         (int)width
                     ); 
            
            return Printable.PAGE_EXISTS;
        } else
            return Printable.NO_SUCH_PAGE;
    }
    
}