import java.awt.Graphics;
import java.awt.Component;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.TextField;
import java.awt.Label;
import java.awt.Frame;
import java.awt.Point;
import java.awt.Cursor;
import java.awt.Color;
import java.awt.Container;
import java.awt.Component;
import java.awt.FileDialog;
import java.io.PrintWriter;
import java.io.IOException;
import java.util.*;
import java.io.*;
import javax.swing.*;
//import java.awt.print.*;


class jScopeWaveContainer extends WaveformContainer 
{
    private   static final int MAX_COLUMN = 4;       

              DataProvider dp;
    private   jScopeDefaultValues def_vals;
    private   boolean   fast_network_access = false;
    private   boolean   supports_fast_network = false;
    private   boolean   supports_local = true;
    private   String    title = null;
    private   DataServerItem  server_item = null;
    private   String    server_infor = null;
    private   String    event = null;
    private   String    print_event = null;
    private   Color     colors[] = Waveform.COLOR_SET;
    private   String    colors_name[] = Waveform.COLOR_NAME;
    private   int	    grid_mode = 0, x_lines_grid = 3, y_lines_grid = 3;
    private   boolean   brief_error = true;
    private   int       columns;
    private   UpdW      updateThread;
    private   boolean   abort;
    private   boolean   add_sig = false;
    private   ProfileDialog profile_dialog;
    private   int       main_shots[] = null;
    private   String    main_shot_str = null;
    private   boolean   main_shot_changed = false;
    private   String    main_shot_error = null;
    private   jScopeMultiWave profile_source = null;
              MdsWaveInterface mds_wi[];
    private   String    save_as_txt_directory = null;
    private   jScopeBrowseSignals browse_sig = null;
    private   boolean modified; 

    class UpdW extends Thread  
    {
        boolean pending = false;
  
        synchronized public void run()
        {
                      
            Date date = new Date();
            long start, end;
            WaveContainerEvent wce = null;

              
            setName("Update Thread");

            while(true)
            {
                    
                try {
                            
                    if(!pending)
                        wait();
                    pending = false;


                    date = new Date();
                    start = date.getTime();
                            
                    wce = new WaveContainerEvent(this, WaveContainerEvent.START_UPDATE, "Start Update");
                    jScopeWaveContainer.this.dispatchWaveContainerEvent(wce);
                    	    
                    try
                    {
                               
	                    UpdateAllWave();
                         	
                        date = new Date();
                        end = date.getTime();
                         	     	    
	                    if(!abort)
                            wce = new WaveContainerEvent(this, WaveContainerEvent.END_UPDATE, "All waveforms are up to date < "+(end-start)+" ms >");
	                    else
                            wce = new WaveContainerEvent(this, WaveContainerEvent.KILL_UPDATE, " Aborted ");

                        jScopeWaveContainer.this.dispatchWaveContainerEvent(wce);
                    	    	    
	                } 
	                catch(Throwable e) 
	                {
                        wce = new WaveContainerEvent(this, WaveContainerEvent.KILL_UPDATE, "Error during apply " + e);
                        jScopeWaveContainer.this.dispatchWaveContainerEvent(wce);
	                }
                } catch (InterruptedException e){}
            }
        }     
         
        synchronized public void StartUpdate()
        {
            pending = true;
            notify();
        }
        
    }

    public jScopeWaveContainer(int rows[], jScopeDefaultValues def_vals)
    {
        this(rows, null, def_vals);
        
    }

       
    public jScopeWaveContainer(int rows[], DataProvider dp, jScopeDefaultValues def_vals)
    {
        super(rows, false);
        this.def_vals = def_vals;
        this.dp = dp; 
        Component c[] = CreateWaveComponents(getComponentNumber());
        AddComponents(c);
        updateThread = new UpdW();
        updateThread.start();
        setBackground(Color.white);
        save_as_txt_directory = System.getProperty("jScope.curr_directory");
    }
    
   protected Component[] CreateWaveComponents(int num)
   {
        Component c[] = new Component[num];
        jScopeMultiWave      wave = null;
        for(int i = 0; i < c.length;i++)
        {
	       wave = buildjScopeMultiWave(dp, def_vals);
	       wave.wi.full_flag = !GetFastNetworkState();
	       wave.addWaveformListener(this);
	       SetWaveParams(wave);
           c[i] = wave;
        }
        return c;
    }
    
    protected jScopeMultiWave buildjScopeMultiWave(DataProvider dp, jScopeDefaultValues def_vals)
    {
        return new jScopeMultiWave(dp, def_vals);
    }
    
    public void initMdsWaveInterface()
    {
        jScopeMultiWave w;
        
        mds_wi = new MdsWaveInterface[getGridComponentCount()];
	    for(int i = 0, k = 0; i < 4; i++)
	    {
	        for(int j = 0; j < rows[i]; j++, k++) 
		    {
		        w = (jScopeMultiWave)getGridComponent(k);
		        w.setCursor(new Cursor(Cursor.WAIT_CURSOR));
		        if(w != null)
		            mds_wi[k] = (MdsWaveInterface)w.wi;
		    }
	    }
    }
    
    
    public void ChangeDataProvider(DataProvider dp)
    {
        jScopeMultiWave w;
                 
        main_shot_changed = true;
	    for(int i = 0; i < getGridComponentCount(); i++)
        {
	        w = (jScopeMultiWave)GetWavePanel(i);
	        if(w != null)
	        {
	   	        if(w.wi != null)
		            w.wi.SetDataProvider(dp);
	            w.Erase();
	            w.SetTitle(null);
		    }
		}
    }
    
    public void Reset()
    {
        int reset_rows[] = {1, 0, 0, 0};
        ph = null;
        pw = null;
        SetTitle(null);
        event = null;
        print_event = null;
        ResetDrawPanel(reset_rows);
        update();
        jScopeMultiWave w = (jScopeMultiWave)GetWavePanel(0);
        w.jScopeErase();
        def_vals.Reset();
    }

    public void NotifyChange(Waveform dest, Waveform source)    
    {
     
       jScopeMultiWave w =  ((jScopeMultiWave)source);
        
       MdsWaveInterface mwi = new MdsWaveInterface(((MdsWaveInterface)w.wi));
       mwi.setDefaultsValues(def_vals);
	  ((jScopeMultiWave)dest).wi = mwi;
	  ((jScopeMultiWave)dest).wi.SetDataProvider(dp);
	  ((jScopeMultiWave)dest).wi.wave = dest;
	  Refresh((jScopeMultiWave)dest, "Copy in");
	  
    }

    public String GetTitle(){return title;}
    public String GetEvent(){return event;}
    public String GetPrintEvent(){return print_event;}
    public String GetServerName(){return server_item.data_server;}
    public String getBrowseClass(){return server_item.browse_class;}
    public String getBrowseUrl(){return server_item.browse_url;}
    public DataServerItem getServerItem(){return server_item;}
    
    
    public String GetServerInfo()
    {
        if(dp == null && server_item != null && server_item.data_server != null)
            return "Can't connect to " + server_item.data_server;
        if(dp == null && server_item == null)
            return "Not connected";
            
        return server_item.data_server;
    }
   
    public void    SetTitle(String title){this.title = title;}
    
    public void    SetServerItem(DataServerItem server_item)
    {
        this.server_item = server_item;
    }

    public boolean supportsFastNetwork(){return supports_fast_network;}
    public boolean supportsLocalProvider(){return supports_local;}
    
    public boolean supportsCompression()
    {
        if(dp != null)
            return dp.supportsCompression();
        else
            return false;
    }
    
    
    public boolean compressionEnable()
    {
        if(dp != null)
            return dp.useCompression();
        else
            return false;
    }

    public void setCompression(boolean state, NetworkListener l) throws IOException
    {
        if(dp != null && dp.supportsCompression() && dp.useCompression() != state) 
        {
            RemoveAllEvents(l);
            dp.disconnect();
            dp.setCompression(state);
            AddAllEvents(l);
        }
    }
    
    public void freeCache()
    {
        dp.freeCache();
    }

    
    public boolean supportsCache()
    {
        if(dp != null)
            return dp.supportsCache();
        else
            return false;
    }

    public void enableCache(boolean state)
    {
        if(dp != null)
            dp.enableCache(state);
    }

    public boolean isCacheEnabled()
    {
        if(dp != null)
            return dp.isCacheEnabled();
        else
            return false;
    }


    
  public void SetBriefError(boolean brief_error){this.brief_error = brief_error;}
  public boolean getBriefError(){return brief_error;}
 
    /*
  public void PrintAllWaves(PrinterJob prnJob, PageFormat pf) throws PrinterException
  {
       prnJob.setPrintable(this, pf);
       prnJob.print();
  }
*/

    public void processWaveformEvent(WaveformEvent e) 
    {
        super.processWaveformEvent(e);
        jScopeMultiWave w = (jScopeMultiWave)e.getSource();
        switch(e.getID())
        {
            case WaveformEvent.PROFILE_UPDATE:
                if(profile_dialog != null && profile_dialog.isShowing())
                {
                    profile_dialog.updateProfiles(e.name,
                                                  e.pixels_x, e.start_pixel_x, 
                                                  e.pixels_y, e.start_pixel_y,
                                                  e.pixels_signal, e.frames_time);
                    if(e.pixels_line != null)
                        profile_dialog.updateProfileLine(e.pixels_line);
                    else
                        profile_dialog.removeProfileLine();
                    
                }
             break;
             case WaveformEvent.END_UPDATE :
                Point p = getComponentPosition(w);
	            if(w.wi.isAddSignal())
	            {
	                String er;
	                if(w.wi.error != null)
	                    er = w.wi.error;
	                else
	                     er = ((MdsWaveInterface)w.wi).getErrorString(this.brief_error);
	                if(er != null)
//		                JOptionPane.showMessageDialog(this, er, 
//		                                            "alert", 
//		                                            JOptionPane.ERROR_MESSAGE);
		                jScope_1.ShowMessage(this, er, 
		                                            "alert", 
		                                            JOptionPane.ERROR_MESSAGE);
	                w.wi.setAddSignal(false);
	            }
                
                WaveContainerEvent wce = new WaveContainerEvent(this, WaveContainerEvent.END_UPDATE, "Wave row " + p.x + " column "+ p.y + " is updated");
                jScopeWaveContainer.this.dispatchWaveContainerEvent(wce);
             break;
        }
    }

    public void showBrowseSignals()
    {
        if(browse_sig != null)
            browse_sig.show();
        else
        {
          String msg;
          if(this.getBrowseUrl() == null)
            msg = "Signals browser not yet implemented on this data server";
          else
          {
            msg = "jScope is unable to locate the signal server page at " + this.getBrowseUrl();
            msg = msg + "\nModify browse_url property for this data server in jScope.properties file.";
		  }
		  JOptionPane.showMessageDialog(null, msg , "alert", JOptionPane.ERROR_MESSAGE); 
        }    
    }

    public void ShowProfileDialog(jScopeMultiWave wave)
    {
        if(profile_dialog != null && profile_dialog.isVisible())
            profile_dialog.dispose();
        //if(profile_dialog == null)
        {
            profile_dialog = new ProfileDialog(wave);
            profile_dialog.pack();
            profile_dialog.setSize(200, 300);
            if(profile_source != null)
                profile_source.setSendProfile(false);
            wave.setSendProfile(true);
            profile_source = wave;
            profile_dialog.show();
            wave.sendProfileEvent();
        }
    }
   
   
  public void PrintAll(Graphics g, int st_x, int st_y, int height, int width)
  {
	  
	if(title != null && title.length() != 0)
	{
	     FontMetrics fm;
	     int s_width;

	     if(font == null)
	     {
	        font = g.getFont();
	        font = new Font(font.getName(), font.getStyle(), 18);
	        g.setFont(font);
	     }
	     else
	     {
	        font = new Font(font.getName(), font.getStyle(), 18);
	        g.setFont(font);
	     }
	     
	     fm = g.getFontMetrics();
	     
	     st_y += fm.getHeight() + 6;
	     height -= st_y + 30;
         
         s_width = fm.stringWidth(title);

         g.drawString(title, st_x + (width - s_width)/2, st_y - 2);
	}
	
	super.PrintAll(g, st_x, st_y, height, width);  
 }         
  
  public void StartUpdate()
  {
      if(!updateThread.isAlive())
      {
        updateThread = new UpdW();
        updateThread.start();
      }
      initMdsWaveInterface();
      updateThread.StartUpdate();
  }

/*
  public void StartPrint(PrinterJob prnJob, PageFormat pf)
  {
    try
    {
      initMdsWaveInterface();
      UpdateAllWave();
      PrintAllWaves(prnJob, pf);
    } catch (InterruptedException e){}
      catch (PrinterException e){}
      catch (Exception e){}
  }
*/

  public int[] getMainShots()
  {
     try
     {
        if(main_shot_changed)
            evaluateMainShot(main_shot_str);
     } catch (IOException e){}
     return main_shots;
  }
  
  public String getMainShotStr()
  {
    try
    {
        if(main_shot_changed)
            evaluateMainShot(main_shot_str);
    } catch (IOException e){}
    return main_shot_str;
  }

  public void setMainShotStr(String main_shot_str)
  {
    if(main_shot_str == null || !main_shot_str.equals(this.main_shot_str))
       main_shot_changed = true;
    this.main_shot_str = main_shot_str;
  }

  public String getMainShotError(boolean brief)
  {
    //if(brief)
    //    return main_shot_error.substring(0, main_shot_error.indexOf("\n"));
    //else
        return main_shot_error;
  }

  public void evaluateMainShot(String in_shots) throws IOException
  {
        String error = null;
	    int int_data[] = null;

        main_shot_error = null;
        
	    if(in_shots == null || in_shots.trim().length() == 0)
	    {
	        main_shot_error = "Main shot value Undefine";
            main_shots = null;
            main_shot_str = null;
	        return;
        }
        
//        if(main_shot_str != null && main_shot_str.equals(in_shots) && main_shots != null)
//            return null;

        main_shots = null;
        main_shot_str = null;
	
//	    dp.Update(null, 0);
	    int_data = dp.GetIntArray(in_shots);
	    if( int_data == null || int_data.length == 0 || int_data.length > MdsWaveInterface.MAX_NUM_SHOT)
	    {
	        if(int_data != null && int_data.length > MdsWaveInterface.MAX_NUM_SHOT)
                error = "Too many shots. Max shot list elements " + MdsWaveInterface.MAX_NUM_SHOT +"\n";
	        else {
		        if(dp.ErrorString() != null)	    
		            main_shot_error = dp.ErrorString();
		        else
		            main_shot_error = "Shot syntax error\n";
	        }
	    }
	    
	    if(main_shot_error == null)
	    {
	        main_shot_changed = false;
                main_shots = int_data;
	    }
        main_shot_str = in_shots.trim();
  }

  
  public void AbortUpdate()
  {
        abort = true;
  }


  public void RemoveAllEvents(NetworkListener l)  throws IOException
  {
     jScopeMultiWave w;
     MdsWaveInterface wi; 
     
     if(dp == null)
        return;
    
     if(event != null && event.length() != 0)
		 dp.removeNetworkListener(l, event);
     
     if(print_event != null && print_event.length() != 0)
		 dp.removeNetworkListener(l, print_event);      
        
	 for(int i = 0, k = 0; i < 4; i++)
	 {
		for(int j = 0; j < getComponentsInColumn(i); j++, k++) 
	    {
	        w = (jScopeMultiWave)getGridComponent(k);
	        w.RemoveEvent();
        }
     }
  }
   
  public synchronized void AddAllEvents(NetworkListener l)  throws IOException
  {
     jScopeMultiWave w;
     MdsWaveInterface wi; 

     if(dp == null)
        return;

     if(event != null && event.length() != 0)
		 dp.addNetworkListener(l, event);
     
     if(print_event != null && print_event.length() != 0)
		 dp.addNetworkListener(l, print_event);      
    
	 for(int i = 0, k = 0; i < 4; i++)
	 {
		for(int j = 0; j < getComponentsInColumn(i); j++, k++) 
		{
	        w = (jScopeMultiWave)getGridComponent(k);
	        w.AddEvent();
        }
     }
  }


    public void InvalidateDefaults()
    {           
        jScopeMultiWave w;
	    for(int i = 0, k = 0; i < rows.length; i++)
	    {
		    for(int j = 0, n_error; j < rows[i]; j++, k++) 
		    {
	            w = (jScopeMultiWave)getGridComponent(k);
	            if(w != null)
		            ((MdsWaveInterface)w.wi).default_is_update = false;
		    }
	    }
    }
   
    public boolean GetFastNetworkState()
    {
        return fast_network_access;
    }
    
    public void SetFastNetworkState(boolean state)
    {
        jScopeMultiWave w;
    	fast_network_access = state;
	    for(int i = 0; i < getComponentNumber(); i++)
        {
	        w = (jScopeMultiWave)getGridComponent(i);
	        if(w != null && w.wi != null ) {
		       w.wi.full_flag = !state;
		       w.wi.setModified(true);
		       //Refresh(w, "Refresh");
		    }
		}
    }    

    public void EraseAllWave()
    {
        jScopeMultiWave w;
	    for(int i = 0; i < getComponentNumber(); i++)
        {
	        w = (jScopeMultiWave)getGridComponent(i);
	        if(w != null)
		       w.Erase();
		}
    }
        
    public synchronized void UpdateAllWave() throws Exception
    {
        jScopeMultiWave w;
        WaveContainerEvent wce;
        
        int curr_mode = GetMode();
	    //SetMode(Waveform.MODE_WAIT);
    	setCursor(new Cursor(Cursor.WAIT_CURSOR));
    	
        try
        {

            if(mds_wi == null)
                abort = true;
            else
	            abort = false;
	    	
	    	if(def_vals != null && def_vals.public_variables != null) 
	    	    dp.SetEnvironment(def_vals.public_variables);
	    	 

	        for(int i = 0, k = 0; i < 4 && !abort; i++)
	        {
	            for(int j = 0; j < rows[i]; j++, k++) 
		        {
		            if(mds_wi[k] != null)
		                mds_wi[k].Update();
		            //w = (jScopeMultiWave)getGridComponent(k);
		            //((MdsWaveInterface)w.wi).Update();
		        }
	        }

	        //    Initialize wave evaluation
	        for(int i = 0, k = 0; i < 4 && !abort; i++)
	        {
		        for(int j = 0; j < rows[i] && !abort; j++, k++) 
		        {
		        // w = (jScopeMultiWave)getGridComponent(k);
		        // if( w.wi!= null && w.wi.error == null)
		            if(mds_wi[k] != null && mds_wi[k].error == null)
		            {
			            wce = new WaveContainerEvent(this, WaveContainerEvent.START_UPDATE, 
							        "Start Evaluate column " + (i + 1) + " row " + (j + 1));
                        dispatchWaveContainerEvent(wce);
			            //w.wi.StartEvaluate();
			            mds_wi[k].StartEvaluate();
		            }
		        }
	        }

        
	        if(main_shots != null)
	        {
		        for(int l = 0; l < main_shots.length && !abort; l++)
		        {
		            for(int i = 0, k = 0; i < 4 && !abort; i++)
		            {
			            for(int j = 0; j < rows[i] && !abort; j++, k++)
			            {
    //		                w = (jScopeMultiWave)getGridComponent(k);
    //			            MdsWaveInterface wi = (MdsWaveInterface)w.wi;
    //			            if(wi != null && wi.error == null && wi.num_waves != 0 && wi.UseDefaultShot())
			                if(mds_wi[k] != null && mds_wi[k].error == null && mds_wi[k].num_waves != 0 && mds_wi[k].UseDefaultShot())
			                {
					            wce = new WaveContainerEvent(this, WaveContainerEvent.START_UPDATE, 
								                "Update signal column " + (i + 1) + " row " + (j + 1) + 
								                " main shot " + main_shots[l]);
					            dispatchWaveContainerEvent(wce);
    //				            wi.EvaluateShot(main_shots[l]);			
				                mds_wi[k].EvaluateShot(main_shots[l]);			
				            }
			            }
			        } 
		        }
	        }
    	    
	        // Evaluate evaluate other shot	
        
	        for(int i = 0, k = 0; i < 4 && !abort; i++)
	        {
		        for(int j = 0; j < rows[i] && !abort; j++, k++) 
		        {
		    //      w = (jScopeMultiWave)getGridComponent(k);
            //      if(w.wi != null && w.wi.error == null && w.wi.num_waves != 0)
			        if(mds_wi[k] != null && mds_wi[k].error == null && mds_wi[k].num_waves != 0)
                    {
                        wce = new WaveContainerEvent(this, WaveContainerEvent.START_UPDATE, 
                                "Evaluate wave column " + (i + 1) + " row " + (j + 1));
                        dispatchWaveContainerEvent(wce);
			        //    w.wi.EvaluateOthers();
			            mds_wi[k].EvaluateOthers();
    			        
		            }		    
		        }
	        }	    

            SwingUtilities.invokeLater(new Runnable() 
            {
                public void run()
                {
                    jScopeMultiWave wx;
	                for(int i = 0, k = 0; i < 4 && !abort; i++)
	                {
		                for(int j = 0; j < rows[i] && !abort; j++, k++) 
		                {
	    	                wx = (jScopeMultiWave)getGridComponent(k);
	    	                if(wx.wi != null)
			                    wx.Update(wx.wi);
		                }
                    }
                }
             });
            
             mds_wi = null;
            
	 //       System.gc();
	//        SetMode(curr_mode);

	        setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
        
        } catch (Exception e) 
        {
   //       System.gc();
  //        SetMode(curr_mode);
  	        setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
            throw(new Exception(""+e));
        }
    }

    private String AddRemoveEvent(NetworkListener l, String curr_event, String event)  throws IOException
    {
        if(curr_event != null && curr_event.length() != 0)
        {
            if(event == null || event.length() == 0)
            {
                dp.removeNetworkListener(l, curr_event);
                return null;
            } 
            else 
            {
                if(!curr_event.equals(event))
                {
                    dp.removeNetworkListener(l, curr_event);
                    dp.addNetworkListener(l, event);
                    return event;
                }
            }
         } 
         else 
         {
            if(event != null && event.length() != 0) 
                dp.addNetworkListener(l, event);
                return event;
         }
         return null;
    }

    public void SetEvent(NetworkListener l, String event) throws IOException
    {
        this.event = AddRemoveEvent(l, this.event, event);
    }
    

    public void SetPrintEvent(NetworkListener l, String print_event) throws IOException
    {
        this.print_event = AddRemoveEvent(l, this.print_event, print_event);
    }


    public void FromFile(Properties pr, String prompt) throws IOException
    {        
	    String prop;
	    int read_rows[] = {0,0,0,0};

	    prop = pr.getProperty(prompt+".columns");
	    if(prop == null)
	        throw(new IOException("missing columns keyword"));
	    else
	    {
		    columns = new Integer(prop).intValue();
		    pw = new float[MAX_COLUMN];
	    }
	     
	    title = pr.getProperty(prompt+".title");
	    
	    event = pr.getProperty(prompt+".update_event");
	    
	    print_event = pr.getProperty(prompt+".print_event");
	    
	    prop = pr.getProperty(prompt+".fast_network_access");
	    if(prop != null)
	    {
		    fast_network_access =  new Boolean(prop).booleanValue();
	    }
	    
	    prop = pr.getProperty(prompt+".data_server_address");
	    if(prop != null)
	    {
	        server_item = new DataServerItem();
	        String server_name;
		    server_name = prop;
            int idx = server_name.indexOf("|");
            if(idx != -1)
            {
                String user = server_name.substring(0, idx);
                if(!user.equals(System.getProperty("user.name")))
                    server_name = System.getProperty("user.name") + server_name.substring(idx, server_name.length());
            }
		    server_item.data_server = server_name;
        }
        
        if(server_item != null)
        {
		    server_item.browse_class = pr.getProperty(prompt+".data_server_browse_class");
        }
        
        if(server_item != null)
        {
		    server_item.browse_url = pr.getProperty(prompt+".data_server_browse_url");
        }

        for(int c = 1; c <= MAX_COLUMN; c++)
        {
            prop = pr.getProperty(prompt+".rows_in_column_"+c);
            if(prop == null)
            {
                if(c == 1)
	                throw(new IOException("missing rows_in_column_1 keyword"));
                break;
            }
		    int r = new Integer(prop).intValue();
		    read_rows[c - 1] = r;
        }
		
		if(columns > 1)
		{
		    for(int c = 1; c < columns; c++)
		    {
                prop = pr.getProperty(prompt+".vpane_"+c);
                if(prop == null)
                {
	                throw(new IOException("missing vpan_"+ c +" keyword"));
                }
		        int w = new Integer(prop).intValue();
		        pw[c-1] = (float)w;
		    }
		}
						    
	    prop = pr.getProperty(prompt+".reversed");
	    if(prop != null)
	    {
		    reversed =  new Boolean(prop).booleanValue();
	    }
	    
        def_vals.xmax = pr.getProperty(prompt+".global_1_1.xmax");
        
        def_vals.xmin = pr.getProperty(prompt+".global_1_1.xmin");
        
        def_vals.xlabel = pr.getProperty(prompt+".global_1_1.x_label");
        
        def_vals.ymax = pr.getProperty(prompt+".global_1_1.ymax");
        
        def_vals.ymin = pr.getProperty(prompt+".global_1_1.ymin");
        
        def_vals.ylabel = pr.getProperty(prompt+".global_1_1.y_label");
        
        def_vals.experiment_str = pr.getProperty(prompt+".global_1_1.experiment");
        
        def_vals.title_str = pr.getProperty(prompt+".global_1_1.title");
        
	    def_vals.upd_event_str = pr.getProperty(prompt+".global_1_1.event");
	    
	    def_vals.def_node_str = pr.getProperty(prompt+".global_1_1.default_node");
        
        prop = pr.getProperty(prompt+".global_1_1.shot");
        if(prop != null)
        {
		    if(prop.indexOf("_shots") != -1) 
			    def_vals.shot_str  =  prop.substring(prop.indexOf("[")+1, prop.indexOf("]")); 
            else
		        def_vals.shot_str = prop;
            
            def_vals.is_evaluated = false;
        }
	    	    
	    ResetDrawPanel(read_rows);
	    jScopeMultiWave w;
	    
        for(int c = 0, k = 0; c < 4 ; c++)
		{
		    for(int r = 0; r < read_rows[c]; r++)
		    {
		        w = (jScopeMultiWave)getGridComponent(k);
			    ((MdsWaveInterface)w.wi).FromFile(pr, "Scope.plot_"+(r+1)+"_"+(c+1));
			    k++;
		    }
		}

	
	    //Evaluate real number of columns
	    int r_columns =  0;
	    for(int i = 0; i < 4; i++)
	        if(read_rows[i] != 0)
		        r_columns = i + 1;
        
	    //Silent file configuration correction
	    //possible define same warning information	    	    
	    if(columns != r_columns)
	    {			
		    columns = r_columns;
		    pw = new float[MAX_COLUMN];
		    for(int i = 0; i < columns; i++)
		        pw[i] = (float) 1./columns;
	    } else {			    
		        if(columns == 4)
		            pw[3] = Math.abs((float)((1000 - pw[2])/1000.));
		        if(columns >= 3)
		            pw[2] = Math.abs((float)(((pw[2] != 0) ? (pw[2] - pw[1]): 1000 - pw[1])/1000.));
		        if(columns >= 2)
		            pw[1] = Math.abs((float)(((pw[1] != 0) ? (pw[1] - pw[0]): 1000  - pw[0])/1000.));
		        if(columns >= 1)
		            pw[0] = Math.abs((float)(((pw[0] == 0) ? 1000 : pw[0])/1000.));
	    }
	
	    UpdateHeight();
	    
    }

    public void UpdateHeight()
    {
	    float height = 0;
//        Dimension d = waves[0].getMinimumSize();
        jScopeMultiWave w;
	
	    ph = new float[getComponentNumber()];
    
	    for(int j = 0, k = 0; j < columns; j++)
	    {
	        height = 0;
    	    for(int i=0; i < rows[j]; i++) {
		        w = (jScopeMultiWave)getGridComponent(k+i);
	            height += w.wi.height;
            }
	        for(int i = 0; i < rows[j]; i++, k++)
	        {
		        w = (jScopeMultiWave)getGridComponent(k);
		        //if( w.wi.height < d.height)//RowColumnLayout.MIN_SIZE_H)
		        if(height == 0)
		        {
		            k -= i;
		            for(i = 0; i < rows[j]; i++)
			            ph[k++] = (float)1./rows[j];			
		            break;
		        }
	            ph[k] = (float)(w.wi.height/height);
	        }
	    }
	    invalidate();
  }

  private boolean IsIpAddress(String addr)
  {
    if(addr.trim().indexOf(".") != -1 && addr.trim().indexOf(" ") == -1)
        return true;
    else
        return false;
  }

    public String SetDataServer(DataServerItem server_item, NetworkListener l)//String new_data_server, NetworkListener l)
    {
	    boolean change = false;  
	    //DataProvider old_dp = dp;
	    DataProvider new_dp = null;
	    String error = null;
	    
	    if(server_item == null || server_item.data_server.trim().length() == 0)
	        return "Defined null or empty data server name";
	        
	    String new_data_server = server_item.data_server;
	    
        if(new_data_server.equals("Local") && supports_local)
        {
	        try {
		        new_dp = new LocalProvider();
		        fast_network_access = false;
		        supports_fast_network = false;
		        supports_local = true;
		        change = true;	
	        } catch (Throwable ex) {
		        supports_local = false;
		        error = "Local data access is not supported on this platform";
		        change = false;
	        }
	    } 

	    if(new_data_server.equals("Jet MDSPlus data"))
	    {
		    new_dp = new JetMdsProvider();
		    fast_network_access = false;
		    supports_fast_network = true;
		    change = true;	
	    }
	    
	    
//Gabriele 26/10/99
        if(new_data_server.equals("Jet data"))
        {
            new_dp = new JetDataProvider();
            Container c = this;
            while(c != null && !(c instanceof JFrame))
                c = c.getParent();
            ((JetDataProvider)new_dp).InquireCredentials((JFrame)c);//(JFrame)this.getParent().getParent());
            int option = ((JetDataProvider)new_dp).GetLoginStatus();
            switch(option)
            {
                case JetDataProvider.LOGIN_OK :
		            fast_network_access = false;
		            supports_fast_network = false;
                    change = true;
                break;
                case JetDataProvider.LOGIN_ERROR :
                    return "Invallid Login";
                case JetDataProvider.LOGIN_CANCEL :
                    return null;         
            }
        } 
    
        if(new_data_server.indexOf("Ftu data") != -1)
        {
            try
            {
	            String ip = new_data_server.substring(new_data_server.indexOf(":")+1, new_data_server.length());
                new_dp = new FtuProvider(ip);
                fast_network_access = false;
		        supports_fast_network = true;
                change = true;
            } 
            catch (IOException ex) 
            {
		        error = "Ftu data server error :" + ex.getMessage();
		        change = false;
            }
        } 
        
        if(new_data_server.indexOf("TEXTOR data") != -1)
        {
            new_dp = new TWUDataProvider();
            fast_network_access = false;
		    supports_fast_network = true;
            change = true;
        } 

        if(new_data_server.equals("Demo server"))
        {
            new_dp = new DemoProvider();
 		    fast_network_access = false;
		    supports_fast_network = false;
            change = true;
        }

        if(!change && IsIpAddress(new_data_server))
        {
	        new_dp = new NetworkProvider(new_data_server);
            fast_network_access = false;
		    supports_fast_network = true;
            change = true;
        }

	    if(change) 
	    {
	        try
	        {
	            RemoveAllEvents(l);
	        
	            if(dp != null) {
                    dp.removeConnectionListener((ConnectionListener)l);
                    dp.disconnect();
                }
                
	            dp = new_dp;
                if(dp != null)
                    dp.addConnectionListener((ConnectionListener)l);
                    
                SetServerItem(server_item);
                ChangeDataProvider(dp);
                AddAllEvents(l);
                
                //create browse panel if defined
                if(server_item.browse_class != null &&
                   server_item.browse_url != null)
                {
                    try
                    {
                        Class cl = Class.forName(server_item.browse_class);
                        browse_sig = (jScopeBrowseSignals)cl.newInstance();
                        browse_sig.setWaveContainer(this);
                        browse_sig.connectToBrowser(server_item.browse_url);
                        browse_sig.setTitle("URL : "+server_item.browse_url);
                    } 
                    catch (Exception e)
                    {
		   //             JOptionPane.showMessageDialog(this, "Can't create browse signal : "+e, 
		   //                                         "alert", 
		   //                                         JOptionPane.ERROR_MESSAGE);
                        browse_sig = null;
                    }
                } else {
                    browse_sig = null;
                }
                this.server_item = server_item;
            } 
            catch (IOException e) 
            {
                SetServerItem(server_item);
                dp = null;
                error = e.getMessage();
            }
	    } else
            error = "Data server "+ new_data_server +" not defined\n";        
	    
	    	
	    return error;
    }
    
    // with_error == true => Signals is added also if an error occurs 
    // during its evaluation
    public void AddSignal(String tree, String shot, String expr, boolean with_error)
    {
        MdsWaveInterface new_wi = null;
        jScopeMultiWave sel_wave = (jScopeMultiWave)GetSelectPanel();
 
        if(sel_wave.wi == null)
        {
            sel_wave.wi = new MdsWaveInterface(sel_wave, dp, def_vals);
            if(!with_error)
                ((MdsWaveInterface)sel_wave.wi).prev_wi = new MdsWaveInterface(sel_wave, dp, def_vals);
        } else {
            new_wi = new MdsWaveInterface((MdsWaveInterface)sel_wave.wi);
            new_wi.wave = sel_wave;
            if(!with_error)
                new_wi.prev_wi = (MdsWaveInterface)sel_wave.wi;
            sel_wave.wi = new_wi;
        }
        if(tree != null &&
           (((MdsWaveInterface)sel_wave.wi).cexperiment == null || 
            ((MdsWaveInterface)sel_wave.wi).cexperiment.trim().length() == 0))
        {
            ((MdsWaveInterface)sel_wave.wi).cexperiment = new String(tree);
            ((MdsWaveInterface)sel_wave.wi).defaults &= ~(1 << MdsWaveInterface.B_exp);
        }
        
        if(shot != null && 
           (((MdsWaveInterface)sel_wave.wi).cin_shot == null || 
            ((MdsWaveInterface)sel_wave.wi).cin_shot.trim().length() == 0))
        {
            ((MdsWaveInterface)sel_wave.wi).cin_shot = new String(shot);
            ((MdsWaveInterface)sel_wave.wi).defaults &= ~(1 << MdsWaveInterface.B_shot);
        }
        
        if(sel_wave.wi.AddSignal(expr)) 
        {
            add_sig = true;
            modified = true;
            Refresh(sel_wave, "Add signal to");
            update();
            add_sig = false;
	    }
    }
    
    public boolean isChange()
    {
        return modified;
    }

    public void AddSignal(String expr)
    {
        AddSignal(null, null, expr, false);
    }

    public void ToFile(PrintWriter out, String prompt) throws IOException
    {
	    jScopeMultiWave w;
        MdsWaveInterface wi;

	    WaveInterface.WriteLine(out, prompt + "title: ", title);
	    if(server_item != null)
	    {
	        WaveInterface.WriteLine(out, prompt + "data_server_address: ",      server_item.data_server);
	        if(server_item.browse_class != null)
	            WaveInterface.WriteLine(out, prompt + "data_server_browse_class: ", server_item.browse_class);
	        if(server_item.browse_url != null)
	            WaveInterface.WriteLine(out, prompt + "data_server_browse_url: ",   server_item.browse_url);
	    }
	    WaveInterface.WriteLine(out, prompt + "fast_network_access: ", ""+fast_network_access);		    
	    WaveInterface.WriteLine(out, prompt + "update_event: ", event);		    
	    WaveInterface.WriteLine(out, prompt + "print_event: ", print_event);
        WaveInterface.WriteLine(out, prompt + "reversed: "     , ""+reversed);

	    out.println();
	     
	    WaveInterface.WriteLine(out,prompt + "global_1_1.experiment: "   , def_vals.experiment_str);
	    WaveInterface.WriteLine(out,prompt + "global_1_1.event: "        , def_vals.upd_event_str);
	    WaveInterface.WriteLine(out,prompt + "global_1_1.default_node: " , def_vals.def_node_str);
	    WaveInterface.WriteLine(out,prompt + "global_1_1.shot: "         , def_vals.shot_str);
	    WaveInterface.WriteLine(out,prompt + "global_1_1.title: "        , def_vals.title_str);
	    WaveInterface.WriteLine(out,prompt + "global_1_1.xmax: "         , def_vals.xmax);
	    WaveInterface.WriteLine(out,prompt + "global_1_1.xmin: "         , def_vals.xmin);
	    WaveInterface.WriteLine(out,prompt + "global_1_1.x_label: "       , def_vals.xlabel);
	    WaveInterface.WriteLine(out,prompt + "global_1_1.ymax: "          , def_vals.ymax);
	    WaveInterface.WriteLine(out,prompt + "global_1_1.ymin: "          , def_vals.ymin);
	    WaveInterface.WriteLine(out,prompt + "global_1_1.y_label: "       , def_vals.ylabel);
	    
	    out.println();
	    	    
	    out.println("Scope.columns: " + getColumns());
	
	    for(int i = 0, c = 1, k = 0; i < getColumns(); i++,c++)
	    {
		    WaveInterface.WriteLine(out, prompt + "rows_in_column_" + c + ": " , ""+getComponentsInColumn(i));
    		for(int j = 0, r = 1; j < getComponentsInColumn(i); j++, r++)
		    {	
		        w = (jScopeMultiWave)getGridComponent(k);
			    wi = (MdsWaveInterface)w.wi;
		        
		        out.println("\n");

		        WaveInterface.WriteLine(out, prompt + "plot_" + r + "_" + c + ".height: "          , ""+w.getSize().height );
		        WaveInterface.WriteLine(out, prompt + "plot_" + r + "_" + c + ".grid_mode: "       , ""+w.grid_mode);
                if(wi != null)
		            wi.ToFile(out, prompt + "plot_" + r + "_" + c + ".");
		        k++;
		    }
	     }

	    out.println();
	    
	     for(int i = 1, k = 0, pos = 0; i < getColumns(); i++)
	     {
		    w = (jScopeMultiWave)getGridComponent(k);
			wi = (MdsWaveInterface)w.wi;
		    pos += (int)(((float)w.getSize().width/ getSize().width) * 1000.);
		    WaveInterface.WriteLine(out, prompt + "vpane_" + i + ": " , ""+pos);
		    k += getComponentsInColumn(i);
	     } 	    
    }


    public synchronized void Refresh(jScopeMultiWave w, String label)
    {        
        Point p = null;

        if(add_sig)
            p = getSplitPosition();
        if(p == null)
            p = getComponentPosition(w);
	    
        WaveContainerEvent wce = new WaveContainerEvent(this, WaveContainerEvent.START_UPDATE, label+" wave row " + p.x + " column " + p.y);
        jScopeWaveContainer.this.dispatchWaveContainerEvent(wce);
	    
	    //If is added a signal to waveform only signal added
	    //is evaluated, in the other cases, refresh from popup menu
	    //or event update, all signals in the waveform must be
	    //evaluated
	    if(!add_sig)
	        ((jScopeMultiWave)w).wi.setModified(true);

	    w.Refresh();
	    
        if(add_sig)
            resetSplitPosition();

    }    
        
    public void SaveAsText(jScopeMultiWave w, boolean all)
    {

        Vector panel = new Vector();
        MdsWaveInterface wi;
        jScopeMultiWave wave;

        if(!all && (w == null || w.wi == null || w.wi.signals == null || w.wi.signals.length == 0)) return;
        
        String title = "Save";
        if(all)
            title = "Save all signals in text format";
        else 
        {
            Point p = this.getWavePosition(w);
            if(p != null)
                title = "Save signals on panel (" + p.x +", " + p.y + ") in text format";
        }        
        JFileChooser file_diag = new JFileChooser();
	    if(save_as_txt_directory != null && save_as_txt_directory.trim().length() != 0)
	        file_diag.setCurrentDirectory(new File(save_as_txt_directory));
        
        file_diag.setDialogTitle(title);
        
        int returnVal = file_diag.showSaveDialog(this);
        
        if (returnVal == JFileChooser.APPROVE_OPTION) 
        {    	  
	        File file = file_diag.getSelectedFile();
	        String txtsig_file = file.getAbsolutePath();
	        if(txtsig_file != null)
	        {
	            save_as_txt_directory = new String(txtsig_file);
	            if(all)
	            {
	                for(int i = 0; i < GetWaveformCount(); i++)
	                   panel.addElement(GetWavePanel(i));
	                
	            } else
                    panel.addElement(w);
                
                String s = "", s1="", s2="";
                boolean g_more_point, new_line;
                StringBuffer space = new StringBuffer();
                
                try {
	                BufferedWriter out = new BufferedWriter(new FileWriter(txtsig_file));
	                for(int l = 0 ; l < 3; l++)
	                {
	                    out.write("%");
                        for(int k = 0; k < panel.size(); k++)
                        {
                            wave = (jScopeMultiWave)panel.elementAt(k);
                            wi = (MdsWaveInterface)wave.wi;
                            
                            if(wi == null || wi.signals == null)
                                continue;
                            
	                        for(int i = 0; i < wi.signals.length; i++)
                            {
                                switch(l)
                                {
                                case 0 : s = "x : "    + ((wi.in_x != null && wi.in_x.length > 0) ? wi.in_x[i]: "None"); break;
                                case 1 : s = "y : "    + ((wi.in_y != null && wi.in_y.length > 0) ? wi.in_y[i]: "None"); break;
                                case 2 : s = "Shot : " + ((wi.shots != null && wi.shots.length > 0) ? ""+wi.shots[i] : "None"); break;
                                }
                                out.write(s, 0, (s.length() < 34) ? s.length() : 34);
                                space.setLength(0);
                                for(int u = 0; u < 35 - s.length(); u++)
                                    space.append(' ');
                                out.write(space.toString());
                            }
                        }
	                    out.newLine();
                    }
    	  
    	            g_more_point = true;
    	            
                    int n_max_sig = 0;
                    boolean more_point[] = new boolean[panel.size()];
                    for(int k = 0; k < panel.size(); k++)
                    {
                        more_point[k] = true;
                        wave = (jScopeMultiWave)panel.elementAt(k);
                        wi = (MdsWaveInterface)wave.wi;
                        if(wi == null || wi.signals == null)
                            continue;
                        if(wi.signals.length > n_max_sig)
                            n_max_sig = wi.signals.length;
    	            }
    	            
    	            int start_idx[][] = new int[panel.size()][n_max_sig];
                    while(g_more_point)
                    {
                        g_more_point = false;
                        for(int k = 0; k < panel.size(); k++)
                        {
                            wave = (jScopeMultiWave)panel.elementAt(k);
                            wi = (MdsWaveInterface)wave.wi;
                            
                            if(wi == null || wi.signals == null)
                                continue;
                            
                            if(!more_point[k])
                            {
	                            for(int i = 0; i < wi.signals.length; i++)
	                                out.write("                                   ");
                                continue;
                            }
                            g_more_point = true;
                            
                            int j = 0;
                            double xmax = wave.GetWaveformMetrics().XMax();
                            double xmin = wave.GetWaveformMetrics().XMin();
                            //int start_idx[] = new int[wi.signals.length];
                            
	                        //for(int i = 0; i < wi.signals.length; i++)
	                        //    start_idx[i] = 0;
        	                
                            more_point[k] = false;
	                        for(int i = 0; i < wi.signals.length; i++)
                            {
                                s1 = "";
                                s2 = "";
                                if(wi.signals[i] != null && wi.signals[i].x != null)
                                {
                                    for(j = start_idx[k][i]; j < wi.signals[i].x.length; j++) 
                                    {
                                        if(wi.signals[i].x[j] > xmin &&
                                           wi.signals[i].x[j] < xmax)
                                        {   
                                            more_point[k] = true;
                                            s1 = ""+wi.signals[i].x[j];
                                            s2 = ""+wi.signals[i].y[j];
                                            start_idx[k][i] = j+1;
                                            break;
                                        } 
                                    }
                                }
                                out.write(s1);
                                space.setLength(0);
                                for(int u = 0; u < 15 - s1.length(); u++)
                                    space.append(' ');
                                space.append(' ');
                                out.write(space.toString());
                                out.write(" ");
                                out.write(s2);
                                space.setLength(0);
                                for(int u = 0; u < 18 - s2.length(); u++)
                                    space.append(' ');
                                out.write(space.toString());                            
                            }
                        }
	                    out.newLine();
                    }                
	                out.close();

	            } catch (IOException e) {
	                System.out.println(e);
	            }
	        }
	    }
	    file_diag = null;
    }

}
