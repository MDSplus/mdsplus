import java.awt.event.*;
import java.io.*;
import java.lang.*;
import java.util.*;
import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;
import com.apple.mrj.*;


public class jScopeMac extends jScope_1 implements MRJQuitHandler, MRJOpenDocumentHandler 
{
    static String     MACfile;

    public jScopeMac()
    {
        this(100,100);
    }
    
    public jScopeMac(int spos_x, int spos_y)
    {
        super(spos_x, spos_y);
        MRJApplicationUtils.registerOpenDocumentHandler(this);
        MRJApplicationUtils.registerQuitHandler(this);    
    }
  
    public void SaveConfiguration(String conf_file)    
    {
	    MRJFileUtils.setDefaultFileType(new MRJOSType("TEXT"));
	    MRJFileUtils.setDefaultFileCreator(new MRJOSType("JSCP"));
	    super.SaveConfiguration(conf_file);
    }
    
    public void handleOpenFile(File file)
    {
        MACfile = new String(file.toString());
        if(num_scope != 0)
        {
	        num_scope++;
	        Rectangle r = getBounds();
	        jScopeMac new_scope = new jScopeMac(r.x + 5, r.y + 40);
	        new_scope.validate();
	        new_scope.pack();
	        new_scope.setSize(750, 550);
	        new_scope.show();
	        new_scope.wave_panel.SetCopySource(wave_panel.GetCopySource());
	        new_scope.startScope(MACfile);
        }   
     }

    public void startScope(String file)
    {
            MACfile = null;
            super.startScope(file);
    }

    protected jScope_1 buildNewScope(int x, int y)
    {
        return new jScopeMac(x, y);
    }
    
    public static void main(String[] args)
    {
            String file = null;    
            
            jScopeMac win = new jScopeMac(100, 100);
            
            win.pack();                 
	        win.setSize(750, 550);
            if(args.length == 1) 
                file = new String(args[0]); 
        
            if(MACfile != null)
	            file = new String(MACfile); 
        
            num_scope++;
            win.startScope(file);
    }

}