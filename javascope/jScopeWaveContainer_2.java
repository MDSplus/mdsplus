import java.awt.*;
import java.awt.print.*;

class jScopeWaveContainer_2 extends jScopeWaveContainer implements Printable
{

    public jScopeWaveContainer_2(int rows[], jScopeDefaultValues def_vals)
    {
        this(rows, null, def_vals);
    }

       
    public jScopeWaveContainer_2(int rows[], DataProvider dp, jScopeDefaultValues def_vals)
    {
        super(rows, dp, def_vals);
    }
 
    protected jScopeMultiWave BuildjScopeMultiWave(DataProvider dp, jScopeDefaultValues def_vals)
    {
        return new jScopeMultiWave_2(dp, def_vals);
    }
    
    public void StartPrint(PrinterJob prnJob, PageFormat pf)
    {
        try
        {
            InitMdsWaveInterface();
            UpdateAllWave();
            PrintAllWaves(prnJob, pf);
        } catch (InterruptedException e){}
        catch (PrinterException e){}
        catch (Exception e){}
    }
    
    public void PrintAllWaves(PrinterJob prnJob, PageFormat pf) throws PrinterException
    {
        prnJob.setPrintable(this, pf);
        prnJob.print();
    }
    
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