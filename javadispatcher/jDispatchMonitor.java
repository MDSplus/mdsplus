import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;

class jDispatchMonitor extends JFrame implements MdsServerListener,
                                                 ConnectionListener
{
    static Font disp_font = new Font("Platino Linotype", Font.ITALIC, 12);
    static Font doing_font = new Font("Platino Linotype", Font.ITALIC | Font.BOLD, 12);
    static Font done_font = new Font("Platino Linotype", Font.PLAIN, 12);
    static Font done_failed_font = new Font("Platino Linotype", Font.ITALIC, 12);
    

    MdsServer mds_server = null;
    MdsServer dispatcher = null;
    
    JDesktopPane desktop = null;
    JLabel total_actions_l, dispatched_l, doing_l, done_l, failed_l,
           exp_l, shot_l, phase_l;
    int disp_count = 0, doing_count = 0, done_count = 0, failed_count = 0, total_count;
    
    ButtonGroup   phase_group = new ButtonGroup();
    JMenu         phase_m = new JMenu("Phase");
    JPopupMenu    do_command_menu = new JPopupMenu();
    JMenuItem     do_redispatch;
    JMenuItem     do_abort;
    
    
    boolean auto_scroll = true;
    int curr_phase = -1;
    int show_phase = -1;
    
    String tree;
    int    shot;
    
 
    JInternalFrame build, executing, failed;
    
    JList buildList,executingList,failedList;
    DefaultListModel build_list = new DefaultListModel();
    DefaultListModel executing_list = new DefaultListModel();
    DefaultListModel failed_list = new DefaultListModel();
    private int num_window = 0;
    
    transient Hashtable phase_hash = new Hashtable();
    transient Hashtable phase_name = new Hashtable();
    
    class ToolTipJList extends JList
    {
        
        public ToolTipJList(ListModel lm)
        {
            super(lm);
        }
        
        public String getToolTipText(MouseEvent event) 
        {
            String out = null;
            int st = this.getFirstVisibleIndex();
            int end = this.getLastVisibleIndex();
            
            if(st == -1 || end == -1) return null;
            
            Rectangle r;
            for(int i = st; i <= end; i++)
            {
                r = this.getCellBounds(i, i);
                if(r.contains(event.getPoint()))
                {
                    MdsMonitorEvent me = (MdsMonitorEvent)getModel().getElementAt(i);
                    out = me.error_message;
                    break;
                }
            }
            return out;
        }        
    }
  
    class BuildCellRenderer extends JLabel implements ListCellRenderer 
    {
        public BuildCellRenderer()
        {
            setOpaque(true);
        }
        
        public Component getListCellRendererComponent(
                                                        JList list,
                                                        Object value,
                                                        int index,
                                                        boolean isSelected,
                                                        boolean cellHasFocus)
        {
            // Setting Font  
            // for a JList item in a renderer does not work.
            setFont(done_font);
            
            
            MdsMonitorEvent me = (MdsMonitorEvent)value;
            setText(me.getMonitorString());
            if(me.on == 0)
                setForeground(Color.lightGray);
            else
                setForeground(Color.black);
            
            if (isSelected) 
            {
                setBackground(list.getSelectionBackground());
            }
            else {
                setBackground(list.getBackground());
            }                
            return this;
        }
    }    
  
        
    class FailedCellRenderer extends JLabel implements ListCellRenderer 
    {
        public FailedCellRenderer()
        {
            setOpaque(true);
        }
        
        public Component getListCellRendererComponent(
                                                        JList list,
                                                        Object value,
                                                        int index,
                                                        boolean isSelected,
                                                        boolean cellHasFocus)
        {
            // Setting Font  
            // for a JList item in a renderer does not work.
            setFont(done_font);
            
            MdsMonitorEvent me = (MdsMonitorEvent)value;
            setText(me.getMonitorString());
                        
            if (isSelected) 
            {
                setBackground(list.getSelectionBackground());
            }
            else {
                setBackground(list.getBackground());
            }                
            return this;
        }
    }    
    class ExecutingCellRenderer extends JLabel implements ListCellRenderer 
    {
        public ExecutingCellRenderer()
        {
            setOpaque(true);
        }
        
        public Component getListCellRendererComponent(
                                                        JList list,
                                                        Object value,
                                                        int index,
                                                        boolean isSelected,
                                                        boolean cellHasFocus)
        {
            // Setting Font  
            // for a JList item in a renderer does not work.
            setFont(done_font);
            
            MdsMonitorEvent me = (MdsMonitorEvent)value;
            setText(me.getMonitorString());
                        
            if (isSelected) 
            {
                setBackground(list.getSelectionBackground());
            }
            else {
                setBackground(list.getBackground());
            }
            
            
            switch(me.mode)
            {
                case MdsMonitorEvent.MonitorDispatched :
                    setForeground(Color.lightGray);
                    //this.setFont(disp_font);
                    break;
                case MdsMonitorEvent.MonitorDoing      : 
                    setForeground(Color.black);
                    //setFont(doing_font);
                    break;
                case MdsMonitorEvent.MonitorDone       : 
                    if((me.ret_status & 1) == 1)
                    {
                        setForeground(Color.blue);
                        //setFont(done_font);
                    } else {
                        setForeground(Color.red);
                        //setFont(done_failed_font);
                    }
                    break;
            }
            return this;
        }
    }    
    
    
    
    class ShowMessage implements Runnable
    {
        String msg;
        
        public ShowMessage(String msg)
        {
            this.msg = msg;
        }
        
	    public void run() {
	        JOptionPane.showMessageDialog(null, msg, "alert", JOptionPane.ERROR_MESSAGE);
	    }
        
    }
    
    public jDispatchMonitor()
    {
        this(null);
    }
    
    public jDispatchMonitor(String monitor_server)
    {
        JMenuBar mb = new JMenuBar();
        this.setJMenuBar(mb);
        setWindowTitle();
                


        try
        {
            if(monitor_server != null && monitor_server.length() != 0)
                openConnection(monitor_server);
        } 
        catch (IOException e)
        {
            System.out.println(e.getMessage());
        }
        JMenu edit = new JMenu("Edit");
        JMenuItem open = new JMenuItem("Open Connection ...");
        open.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                    openConnection();
                }
            }       
        );
        edit.add(open);
 
        JMenuItem exit = new JMenuItem("Exit");
        exit.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                   exitApplication();
                }
            }       
        );
        edit.add(exit);
        
        JMenu view = new JMenu("View");
        JCheckBoxMenuItem build_cb = new JCheckBoxMenuItem("Build", true);
        build_cb.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                    num_window += build.isShowing() ? -1 : 1;
                    build.setVisible(!build.isShowing()); 
                }
            }       
        );
        view.add(build_cb);

        JCheckBoxMenuItem executing_cb = new JCheckBoxMenuItem("Executing", true);
        executing_cb.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                    num_window += executing.isShowing() ? -1 : 1;
                    executing.setVisible(!executing.isShowing()); 
                }
            }       
        );
        view.add(executing_cb);

        JCheckBoxMenuItem failed_cb = new JCheckBoxMenuItem("Failed", true);
        failed_cb.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                    num_window += failed.isShowing() ? -1 : 1;
                    failed.setVisible(!failed.isShowing()); 
                }
            }       
        );
        view.add(failed_cb);
        
        view.add(new JSeparator());

        JCheckBoxMenuItem auto_scroll_cb = new JCheckBoxMenuItem("Auto Scroll", true);
        auto_scroll_cb.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                     auto_scroll = !auto_scroll;
                }
            }       
        );
        view.add(auto_scroll_cb);

                
        mb.add(edit);
        mb.add(view);
        mb.add(phase_m);
        
        JPanel p0 = new JPanel();
        p0.setBorder(BorderFactory.createRaisedBevelBorder());
        
        Box box = new Box(BoxLayout.Y_AXIS);
        JPanel p = new JPanel();
        p.add(new JLabel("Active actions"));
        p.add((total_actions_l = new JLabel("    0")));        
        p.add(new JLabel("Dispatched"));
        p.add((dispatched_l = new JLabel("    0")));
        p.add(new JLabel("Doing"));
        p.add((doing_l = new JLabel("    0")));
        p.add(new JLabel("Done"));
        p.add((done_l = new JLabel("    0")));
        p.add(new JLabel("Failed"));
        p.add((failed_l = new JLabel("    0")));
        
        JPanel p1 = new JPanel();
        p1.add((exp_l = new JLabel("Experiment:          ")));
        p1.add((shot_l = new JLabel("Shot:        ")));
        p1.add((phase_l = new JLabel("Phase:        ")));
        
        box.add(p1);
        box.add(p);
        p0.add(box);
        
   	    this.getContentPane().add(p0, BorderLayout.NORTH);
        
        
	    desktop = new JDesktopPane()
	    {        
            public void setBounds(int x, int y, int width, int height)
            {
                super.setBounds(x, y, width, height);
                positionWindow();
            }            	        

            private void positionWindow()
            {
                if(num_window == 0) 
                    return;
                Dimension dim = desktop.getSize();
                dim.height = dim.height/num_window;
                int y = 0;
                if(build.isVisible())
                {
                    build.setBounds(0,y,dim.width,dim.height);
                    y += dim.height;
                }
                if(executing.isVisible())
                {
                    executing.setBounds(0,y,dim.width,dim.height);
                    y += dim.height;
                }
                if(failed.isVisible())
                {
                    failed.setBounds(0,y,dim.width,dim.height);
                }  
            } 
	    }; 
   	          
        buildList = new JList(build_list);
        buildList.setCellRenderer(new BuildCellRenderer());
        JScrollPane sp_build = new JScrollPane();
        sp_build.getViewport().setView(buildList);
               
        executingList = new ToolTipJList(executing_list);
        executingList.setToolTipText("");
        executingList.setCellRenderer(new ExecutingCellRenderer());
        JScrollPane sp_executing = new JScrollPane();
        sp_executing.getViewport().setView(executingList);
        
        do_redispatch = new JMenuItem("Redispatch");
	    do_redispatch.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                try
	                {
	                    Object ob[] = executingList.getSelectedValues();
	                    if(ob != null && ob.length != 0)
	                    {
	                        for(int i=0; i < ob.length; i++)
	                            redispatch((MdsMonitorEvent)ob[i]);
	                    }
	                } 
	                catch (Exception exc) 
	                {
	                    JOptionPane.showMessageDialog(null, 
	                                                "Error dispatching action(s)", 
	                                                "Alert", 
	                                                JOptionPane.ERROR_MESSAGE);
	                    
	                }
	            }
	        }
	    );
        do_command_menu.add(do_redispatch);
        
        
        do_abort = new JMenuItem("Abort");
	    do_abort.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                try
	                {
	                    Object ob[] = executingList.getSelectedValues();
	                    if(ob != null && ob.length == 1)
	                    {
	                          abortAction((MdsMonitorEvent)ob[0]);
	                    }
	                } 
	                catch (Exception exc) 
	                {
	                    JOptionPane.showMessageDialog(null, 
	                                                "Error during action(s) re-dispatch", 
	                                                "Alert", 
	                                                JOptionPane.ERROR_MESSAGE);
	                    
	                }
	            }
	        }
	    );
        do_command_menu.add(do_abort);
        
        executingList.addMouseListener( new MouseAdapter()
	    {
	          public void mousePressed(MouseEvent e)
	          {
	            if(MdsHelper.getDispatcher() != null &&
	              (e.getModifiers() & Event.META_MASK) != 0)
	            {
	                Object ob[] = executingList.getSelectedValues();
	                if(ob == null || ob.length == 0)
	                {
	                    if(executingList.getModel().getSize() != 0)
	                        JOptionPane.showMessageDialog(null, 
	                                                  "Select action(s) to re-dispatch", 
	                                                  "Warning", 
	                                                  JOptionPane.INFORMATION_MESSAGE);
	                    return;
	                }
	                if(ob.length == 1 && ((MdsMonitorEvent)ob[0]).mode == MdsMonitorEvent.MonitorDoing)
	                {
	                    do_redispatch.setEnabled(false);
	                    do_abort.setEnabled(true);
	                } else {
	                    do_redispatch.setEnabled(true);
	                    do_abort.setEnabled(false);
	                }
		            int x = e.getX();
		            int y = e.getY();
		            do_command_menu.show(executingList, x, y);
		        } 
		    }
		});
             
        failedList = new ToolTipJList(failed_list);
        failedList.setToolTipText("");
        failedList.setCellRenderer(new FailedCellRenderer());
        JScrollPane sp_failed = new JScrollPane();
        sp_failed.getViewport().setView(failedList);

	    build     = createInternalFrame("Build Table", sp_build);
	    executing = createInternalFrame("Executing", sp_executing);
	    failed    = createInternalFrame("Failed", sp_failed);

   	    this.getContentPane().add(desktop, BorderLayout.CENTER);
   	    
   	    p1 = new JPanel();
   	    p1.setBackground(Color.white);
        p1.setBorder(BorderFactory.createLoweredBevelBorder());
   	    JLabel l1 = new JLabel("Dispatched           ");
   	    l1.setForeground(Color.lightGray);
   	    p1.add(l1);
   	    l1 = new JLabel("Doing               ");
   	    l1.setForeground(Color.black);
   	    p1.add(l1);
   	    l1 = new JLabel("Done                ");
   	    l1.setForeground(Color.blue);
   	    p1.add(l1);
   	    l1 = new JLabel("Failed              ");
   	    l1.setForeground(Color.red);
   	    p1.add(l1);
   	    this.getContentPane().add(p1, BorderLayout.SOUTH);
    
    }
    

    /** 
      * Create an internal frame 
      */ 
     public JInternalFrame createInternalFrame(String title, Container panel) 
     { 
 	    JInternalFrame jif = new JInternalFrame(); 
      
 	    jif.setTitle(title); 
              
 	    // set properties 
 	    jif.setClosable(false);
 	    jif.setMaximizable(true);
 	    jif.setIconifiable(false); 
 	    jif.setResizable(true); 
      
 	    jif.setContentPane(panel);
           	 
 	    desktop.add(jif, 1);   
                          
        jif.setVisible(true);
        num_window++;
      
 	    return jif; 
     } 

       
    public void exitApplication()
    {
        System.exit(0);
    }

    
    public void openConnection()
    {
        try
        {
            if(mds_server != null)
            {
                if(mds_server.connected)
                {
                    Object[] options = { "DISCONNECT", "CANCEL" };
                    int val = JOptionPane.showOptionDialog(null, 
                                                "Dispatch monitor is already connected to "+mds_server.provider,
                                                "Warning",
                                                JOptionPane.DEFAULT_OPTION, 
                                                JOptionPane.WARNING_MESSAGE,
                                                null, options, options[0]);
                    
                    if(val == 0) 
                    {
                        mds_server.shutdown();
                        mds_server = null;
                        setWindowTitle();
                    } else
                        return;
                } else
                    mds_server.shutdown();    
            } 
                        
            String mon_srv = JOptionPane.showInputDialog("Dispatch monitor server");
            if(mon_srv == null)
                return;
            openConnection(mon_srv);
        }
        catch (IOException exc)
        {
            JOptionPane.showMessageDialog(jDispatchMonitor.this, exc.getMessage(), "alert", JOptionPane.ERROR_MESSAGE);
        }
    }
    
    public void openConnection(String mon_srv) throws IOException
    {
        if(mon_srv == null)
            throw(new IOException("Can't connect to null address"));
        mds_server = new MdsServer(mon_srv);
        mds_server.addMdsServerListener(jDispatchMonitor.this);
        mds_server.addConnectionListener(jDispatchMonitor.this);
        Descriptor reply = mds_server.monitorCheckin();       
        setWindowTitle();
    }
    
    private void abortAction(MdsMonitorEvent me) throws IOException
    {
        if(me.mode == MdsMonitorEvent.MonitorDoing)
            doCommand("Abort", me);
    }
    
    private void redispatch(MdsMonitorEvent me) throws IOException
    {
        if(me.mode == MdsMonitorEvent.MonitorDone)
            doCommand("Dispatch", me);
    }

    private void doCommand(String command, MdsMonitorEvent me) throws IOException
    {
        if(dispatcher == null)
        {
            dispatcher = new MdsServer(MdsHelper.getDispatcher());
            dispatcher.addConnectionListener(jDispatchMonitor.this);
        }
        
        Descriptor reply = dispatcher.dispatchCommand("tcl", command+" "+me.nid);
        if((reply.status & 1) == 0)
        {
	        JOptionPane.showMessageDialog(null, 
	                                    command+" action (nid="+me.nid+") error", 
	                                    "Alert", 
	                                    JOptionPane.ERROR_MESSAGE);
	    }
	    else
	    {
	       disp_count--;
	       done_count--;
        }
    }
    
    private void setWindowTitle()
    {
        if(mds_server != null && mds_server.connected)
            setTitle("jDispatchMonitor - Connected to "+ mds_server.provider+" receive on port "+mds_server.rcv_port);
        else
            setTitle("jDispatchMonitor - Not Connected");        
    }
    
    public synchronized void processMdsServerEvent(MdsServerEvent e)
    {
        int status = 1;
        if(e instanceof MdsMonitorEvent)
        {
            MdsMonitorEvent me = (MdsMonitorEvent)e;
            switch(me.mode)
            {
                case MdsMonitorEvent.MonitorBuildBegin :
                    resetAll(me);
                case MdsMonitorEvent.MonitorBuild      : 
                case MdsMonitorEvent.MonitorBuildEnd   :
                    build_list.addElement(me);
                    break;
                case MdsMonitorEvent.MonitorCheckin    : 
                    break;
                case MdsMonitorEvent.MonitorDone       :
                    status = me.ret_status;
                case MdsMonitorEvent.MonitorDispatched : 
                case MdsMonitorEvent.MonitorDoing      :
                    int idx = getIndex(me, executing_list.size());
                    if(idx == -1)
                    {
                        executing_list.addElement(me);
                    } else
                        executing_list.set(idx,me);
                    if((status & 1) == 0) 
                    {
                        failed_list.addElement(me);
                        showUpdateAction(failedList, -1);  
                    }
                    showUpdateAction(executingList, idx);  
                    
                    break;
            }
            counter(me);
        }
    }
    
    private void showUpdateAction(JList list, int idx)
    {
        if(auto_scroll && show_phase == -1)
        {
            if(idx == -1)
                idx = list.getModel().getSize() - 1;
            list.ensureIndexIsVisible(idx);
        }   
    }
    
    private void counter(MdsMonitorEvent me)
    {
        switch(me.mode)
        {
            case MdsMonitorEvent.MonitorBuildBegin :
                total_count++;
                break;
            case MdsMonitorEvent.MonitorBuild      : 
            case MdsMonitorEvent.MonitorBuildEnd   : 
                total_count++;
                break;
            case MdsMonitorEvent.MonitorDispatched :
                disp_count++;
               break;
            case MdsMonitorEvent.MonitorDoing      :
                doing_count++;
                break;
            case MdsMonitorEvent.MonitorDone       :
                doing_count--;
                done_count++;
                if((me.ret_status & 1) == 0)
                    failed_count++;         
                break;
        }
        dispatched_l.setText(" "+disp_count);
        doing_l.setText(" "+doing_count);
        done_l.setText(" "+done_count);
        total_actions_l.setText(" "+total_count);
        failed_l.setText(" "+failed_count);
    }
    
    private void resetAll(MdsMonitorEvent me)
    {
        total_count  = 0;
        doing_count  = 0;
        done_count   = 0;
        failed_count = 0;
        disp_count   = 0;        
        
        phase_m.removeAll();
        build_list.removeAllElements();
        failed_list.removeAllElements();
        executing_list.removeAllElements();
        phase_hash.clear();
        phase_name.clear();
        if(me != null)
        {
            exp_l.setText("Experiment: "+me.tree);
            shot_l.setText("Shot: "     +me.shot);
            phase_l.setText("Phase: "   +me.phase);
            tree = new String(me.tree);
            shot = me.shot;
        }
    }
    
    private int getIndex(MdsMonitorEvent me, int idx)
    {
        Hashtable actions;
        if(phase_hash.containsKey(new Integer(me.phase)))
            actions = (Hashtable)phase_hash.get(new Integer(me.phase));
        else 
        {
            String phase_st = MdsHelper.toPhaseName(me.phase);
            boolean new_phase = addPhaseItem(me.phase, phase_st);
            executing_list.removeAllElements();
            phase_l.setText("Phase: "+phase_st);
            curr_phase = me.phase;
            
            actions = new Hashtable();
            phase_hash.put(new Integer(me.phase), actions);
            idx = 0;
        }
        
        if(actions.containsKey(new Integer(me.nid)))
        {
            MdsMonitorEvent e = (MdsMonitorEvent)actions.get(new Integer(me.nid));
            if(me.mode == MdsMonitorEvent.MonitorDone)
            {
                if(e.mode == MdsMonitorEvent.MonitorDispatched)
                    doing_count++;
            }
            e.ret_status = me.ret_status;
            e.mode = me.mode;
            e.date = me.date;
            return e.jobid;
        }
        else
        {
            me.jobid = idx;
            actions.put(new Integer(me.nid), me);
            return -1;
        }            
    }
    
    private boolean addPhaseItem(int phase, String phase_st)
    {
        
        for (Enumeration e = phase_group.getElements() ; e.hasMoreElements() ;)
        {
            JCheckBoxMenuItem c = (JCheckBoxMenuItem)e.nextElement();
            if(c.getText().equals("Phase_"+phase))
                return false;
        }
        
        
        JCheckBoxMenuItem phase_cb = new JCheckBoxMenuItem(phase_st);
        phase_cb.setActionCommand(""+phase);
        phase_cb.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                String cmd =((JCheckBoxMenuItem)e.getSource()).getActionCommand();
                show_phase = Integer.parseInt(cmd);
                showPhase(cmd);
            }
        });
        phase_group.add(phase_cb);
        phase_m.add(phase_cb);
        return true;
    }
    
    private void showPhase(String phase)
    {
        Hashtable actions;
        MdsMonitorEvent me;
        DefaultListModel show_executing_list = new DefaultListModel();
        DefaultListModel show_failed_list = new DefaultListModel();
        int phase_id = Integer.parseInt(phase);
        
        if(phase_hash.containsKey(new Integer(phase)))
        {
            if(curr_phase != phase_id)
            {
                show_phase = Integer.parseInt(phase);
                executingList.setModel(show_executing_list);
                failedList.setModel(show_failed_list);
                actions = (Hashtable)phase_hash.get(new Integer(phase));
                int s_done = 0, s_failed = 0;
                for (Enumeration e = actions.elements() ; e.hasMoreElements() ;) 
                {
                    me = (MdsMonitorEvent)e.nextElement();
                    show_executing_list.addElement(me);
                    s_done++;
                    if((me.ret_status & 1) == 0)
                    {
                        show_failed_list.addElement(me);
                        s_failed++;
                    }
                }
                executing.setTitle("Executed in " + MdsHelper.toPhaseName(phase_id) + "phase Done :" + s_done + " Failed: " + s_failed);
                failed.setTitle("Executed in " + MdsHelper.toPhaseName(phase_id));
            } 
            else 
            {
              phase_l.setText(MdsHelper.toPhaseName(curr_phase));
              executing.setTitle("Executing");
              failed.setTitle("Failed");
              executingList.setModel(executing_list);
              failedList.setModel(failed_list);
              show_phase = -1;
            }    
        }
    }
    
    public synchronized void processConnectionEvent(ConnectionEvent e)
    {
        if(e.getSource() == mds_server)
        {
            if(e.getID() == ConnectionEvent.LOST_CONNECTION)
            {
	            // do the following on the gui thread
	            ShowMessage alert = new ShowMessage(e.info); 
                SwingUtilities.invokeLater(alert);          
                setWindowTitle();
                mds_server = null;
                try
                {
                    if(dispatcher != null);
                        dispatcher.shutdown();
                    dispatcher = null;
                } catch (Exception exc) {dispatcher = null;}
	            return;
            }
        }
        if(e.getSource() == dispatcher)
        {
            if(e.getID() == ConnectionEvent.LOST_CONNECTION)
                dispatcher = null;
            
        }
    }
    
    public static void main(String arg[])
    {
        
        jDispatchMonitor dm;
        
        if(arg != null &&  arg.length > 0 && arg[0].length() != 0)
            dm = new jDispatchMonitor(arg[0]);
        else
            dm = new jDispatchMonitor();
        dm.pack();
        dm.setSize(600, 700);
        dm.show();
    }
}