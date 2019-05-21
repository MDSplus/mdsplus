import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.table.*;
import java.util.*;
import java.text.*;
import jScope.*;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.FlavorListener;
import java.awt.datatransfer.FlavorEvent;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.StringSelection;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.io.BufferedReader;
import java.io.IOException;


public class DeviceWave extends DeviceComponent
{
    static final int MAX_POINTS = 500;
    static final double MIN_STEP = 1E-5;


    public boolean maxXVisible = false;
    public boolean minXVisible = false;
    public boolean maxYVisible = true;
    public boolean minYVisible = false;
    public boolean waveEditable = false;
    public String updateExpression = null;
    protected int prefHeight = 200;

    public void setPrefHeight(int prefHeight)
    {
        this.prefHeight = prefHeight;
    }

    public int getPrefHeight(){return prefHeight;}

    public void setMaxXVisible(boolean visible)
    {
        maxXVisible = visible;
    }

    public void setMinXVisible(boolean visible)
    {
        minXVisible = visible;
    }

    public void setMaxYVisible(boolean visible)
    {
        maxYVisible = visible;
    }

    public void setMinYVisible(boolean visible)
    {
        minYVisible = visible;
    }

    public boolean getMaxXVisible()
    {
        return maxXVisible;
    }

    public boolean getMinXVisible()
    {
        return minXVisible;
    }

    public boolean getMaxYVisible()
    {
        return maxYVisible;
    }

    public boolean getMinYVisible()
    {
        return minYVisible;
    }

    public void setWaveEditable(boolean editable)
    {
        waveEditable = editable;
    }

    public boolean getWaveEditable()
    {
        return waveEditable;
    }

    public void setUpdateExpression(String updateExpression)
    {
        this.updateExpression = updateExpression;
    }

    public String getUpdateExpression()
    {
        return updateExpression;
    }

    protected boolean initializing = false;
    protected WaveformEditor waveEditor;
    protected JTable table;
    protected JTextField maxXField = null, minXField = null, maxYField = null,
        minYField = null;
    protected JCheckBox editCB;
    protected JScrollPane scroll;
    protected int numPoints;
    protected float[] waveX = null, waveY = null;
    protected float[] waveXOld = null, waveYOld = null;
    protected float maxX, minX, maxY, minY;
    protected float maxXOld, minXOld, maxYOld, minYOld;

    private NumberFormat nf = NumberFormat.getInstance(Locale.ENGLISH);

    protected static float savedMinX, savedMinY, savedMaxX, savedMaxY;
    protected static float savedWaveX[] = null, savedWaveY[] = null;
    JPopupMenu copyPastePopup;
    JMenuItem copyI, copyC, pasteI, pasteC;
    public DeviceWave()
    {}

	private boolean isClipboardText(Clipboard clip)
	{
		Transferable contents = clip.getContents(null);
		DataFlavor[] flavors = contents.getTransferDataFlavors();
	       
		//System.out.println("isClipboardText: " + flavors);

		boolean isText = false;
		for( int k=0 ; k < flavors.length ; k++) {
		    //System.out.println( flavors[k].getHumanPresentableName());
		    if( !flavors[k].getHumanPresentableName().equals("Unicode String") )
		        continue;
		    isText = true;
		    break;
		}
		return isText;
	}

    private void create()
    {
        savedWaveX = null;
        waveEditor = new WaveformEditor();
        nf.setMaximumFractionDigits(4);
        nf.setGroupingUsed(false);
        waveEditor.setPreferredSize(new Dimension(300, prefHeight));
        waveEditor.addWaveformEditorListener(new WaveformEditorListener()
        {
            public void waveformUpdated(float[] waveX, float[] waveY,
                                        int newIdx)
            {
                numPoints = waveX.length;
                DeviceWave.this.waveX = waveX;
                DeviceWave.this.waveY = waveY;
                if (newIdx >= 0)
                {
                    table.setRowSelectionInterval(newIdx, newIdx);
                    table.setEditingRow(newIdx);
                    int centerIdx;
                    if (newIdx > 8)
                        centerIdx = newIdx - 4;
                    else
                        centerIdx = 0;
                    int rowY = centerIdx * table.getRowHeight();
                    scroll.getViewport().setViewPosition(new Point(0, rowY));

                }
                if (maxYVisible)
                {
                    try
                    {
                        maxY = (new Float(maxYField.getText()).floatValue());
                        for (int i = 0; i < waveY.length; i++)
                            if (waveY[i] > maxY)
                                waveY[i] = maxY;

                    }
                    catch (Exception exc)
                    {}
                }
                table.repaint();
            }
        });
        waveEditor.setEditable(false);
        table = new JTable();
        /*table.setColumnModel(new DefaultTableColumnModel()
                 {
            public int getTotalColumnWidth() {return 150;}
                 });
         */
        table.setModel(new DefaultTableModel()
        {
            public void addTableModelListener(TableModelListener l)
            {}

            public Class getColumnClass(int col)
            {
                return new String().getClass();
            }

            public int getColumnCount()
            {
                return 2;
            }

            public int getRowCount()
            {
                return MAX_POINTS;
            }

            public String getColumnName(int col)
            {
                if (col == 0)
                    return "Time";
                else
                    return "Value";
            }

            public Object getValueAt(int row, int col)
            {
                if (waveX == null || row >= waveX.length)
                    return "";
                float currVal = (col == 0) ? waveX[row] : waveY[row];
                //return (new Float(currVal)).toString();
                return (nf.format(currVal));
            }

            public boolean isCellEditable(int row, int col)
            {
                if (!waveEditable)
                    return false;
                if (row == 0 && col == 0)
                    return false;
                if (row == waveX.length - 1 && col == 0)
                    return false;
                return true;
            }

            public void removeTableModelListener(TableModelListener l)
            {}

            public void setValueAt(Object val, int row, int col)
            {
                if (row >= waveX.length)
                {
                    JOptionPane.showMessageDialog(DeviceWave.this,
                        "There are misssing points in the waveform definition",
                        "Incorrect waveform definition",
                        JOptionPane.WARNING_MESSAGE);
                    return;
                }
                float valFloat;
                try
                {
                    valFloat = (new Float( (String) val)).floatValue();
                }
                catch (Exception exc)
                {
                    JOptionPane.showMessageDialog(DeviceWave.this,
                        "The value is not a correct floating point representation",
                        "Incorrect waveform definition",
                        JOptionPane.WARNING_MESSAGE);
                    return;
                }
                if (col == 0)
                {
                    if (valFloat > maxX)
                        waveX[row] = maxX;
                    else if (valFloat < minX)
                        waveX[row] = minX;
                    else
                        waveX[row] = valFloat;
                    if (row == 0 || row == waveX.length - 1)
                        return;
                    if (waveX[row] < waveX[row - 1] + MIN_STEP)
                        waveX[row] = waveX[row - 1] + (float) MIN_STEP;
                    if (waveX[row] > waveX[row + 1] - MIN_STEP)
                        waveX[row] = waveX[row + 1] - (float) MIN_STEP;
                }
                else
                {
                    //  if(valFloat > maxY)
                    //  waveY[row] = maxY;
                    //else if(valFloat < minY)
                    //   waveY[row] = minY;
                    //else
                    waveY[row] = valFloat;
                }
                waveEditor.setWaveform(waveX, waveY, minY, maxY);
            }
        });
        setLayout(new BorderLayout());
        add(waveEditor, "Center");
        scroll = new JScrollPane(table);
        scroll.setPreferredSize(new Dimension(150, 200));
        add(scroll, "East");

        JPanel jp = new JPanel();
        if (minXVisible)
        {
            jp.add(new JLabel("Min X: "));
            jp.add(minXField = new JTextField("" + minX, 6));
        }
        if (maxXVisible)
        {
            jp.add(new JLabel("Max X: "));
            jp.add(maxXField = new JTextField("" + maxX, 6));
        }
        if (minYVisible)
        {
            jp.add(new JLabel("Min Y: "));
            jp.add(minYField = new JTextField("" + minY, 6));
        }
        if (maxYVisible)
        {
            jp.add(new JLabel("Max Y: "));
            jp.add(maxYField = new JTextField("" + maxY, 6));
        }
        editCB = new JCheckBox("Edit", false);
        editCB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                waveEditable = editCB.isSelected();
                waveEditor.setEditable(editCB.isSelected());
            }
        });
        jp.add(editCB);

        JButton updateB = new JButton("Update");
        updateB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                updateLimits();
                waveEditor.setWaveform(waveX, waveY, minY, maxY);
            }
        });
        jp.add(updateB);
        add(jp, "North");

        //Add popup for copy/paste
        copyPastePopup = new JPopupMenu();
        copyI = new JMenuItem("Copy");
        copyI.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                savedMinX = minX;
                savedMinY = minY;
                savedMaxX = maxX;
                savedMaxY = maxY;
                savedWaveX = waveX;
                savedWaveY = waveY;
            }
        });
        copyPastePopup.add(copyI);
        pasteI = new JMenuItem("Paste");
        pasteI.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                if(savedWaveX == null) return;
                if(minXVisible) minX = savedMinX;
                if(minYVisible) minY = savedMinY;
                if(maxXVisible) maxX = savedMaxX;
                if(maxYVisible) maxY = savedMaxY;
                try {
                    waveX = new float[savedWaveX.length];
                    waveY = new float[savedWaveY.length];
                    for(int i = 0; i < savedWaveX.length; i++)
                    {
                        waveX[i] = savedWaveX[i];
                        waveY[i] = savedWaveY[i];
                    }
                }catch(Exception exc){}
                displayData(null, true);
            }
        });
        copyPastePopup.add(pasteI);

        pasteC = new JMenuItem("Paste from Clipboard");
        pasteC.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                int numPoint;
                boolean yxColumn = false;
                boolean setMinX=false,setMaxX=false,setMinY=false,setMaxY=false; 
		try {
   
			Clipboard clip = java.awt.Toolkit.getDefaultToolkit().getSystemClipboard();
			Transferable contents = clip.getContents(null);
			DataFlavor[] flavors = contents.getTransferDataFlavors();
                        String valType = "";
			for( int k=0 ; k<flavors.length ; k++) {
                            try {
				    //System.out.println( flavors[k].getHumanPresentableName());
                                    if(   !flavors[k].getHumanPresentableName().equals("Unicode String") )
                                       continue;

                                    BufferedReader in = new BufferedReader( flavors[k].getReaderForText(contents));
                                    String s = in.readLine();
                                    while( s!=null ) //&& s.trim().length() != 0 )	
                                    {
                                                      
                                        if( s.trim().length() != 0 )
                                        {
                                            StringTokenizer st = new StringTokenizer(s);
                                            valType = st.nextToken();

	                                    if(valType.trim().equalsIgnoreCase("xmax"))
	                                        {
	                                            savedMaxX = Float.parseFloat(st.nextToken());
	                                            setMaxX = true;
	                                        }
	                                    else if(valType.trim().equalsIgnoreCase("xmin"))
	                                        {
	                                            savedMinX = Float.parseFloat(st.nextToken());
	                                            setMinX = true;
	                                        } 
	                                    else if(valType.trim().equalsIgnoreCase("ymax"))
	                                        {
	                                            savedMaxY = Float.parseFloat(st.nextToken());
	                                            setMaxY = true;
	                                        }                                             
	                                    else if(valType.trim().equalsIgnoreCase("ymin"))
	                                        {
	                                            savedMinY = Float.parseFloat(st.nextToken());
	                                            setMinY = true;
	                                        } 
	                                    else if(valType.trim().equalsIgnoreCase("x"))
	                                    {
	                                        int i = 0;
	                                        numPoint = st.countTokens();
	                                        savedWaveX = new float[numPoint];
		                                while( st.hasMoreTokens() )
		                                {
		                                    savedWaveX[i] = Float.parseFloat(st.nextToken());
		                                    i++;
		                                }
	                                    } 
	                                    else if(valType.trim().equalsIgnoreCase("y"))
	                                    {
	                                        int i = 0;
	                                        numPoint = st.countTokens();
	                                        savedWaveY = new float[numPoint];
			                        while( st.hasMoreTokens() )
			                        {
			                             savedWaveY[i] = Float.parseFloat(st.nextToken());
			                             i++;
			                        }
	                                    } 
                                            else if( valType.trim().equalsIgnoreCase("xc") && st.nextToken().equalsIgnoreCase("yc") ||
                                                     (yxColumn = ( valType.trim().equalsIgnoreCase("yc") && st.nextToken().equalsIgnoreCase("xc") ))  )
	                                    {
	                                        int i = 0;
                    				Vector<Float> vectX = new Vector<Float>();
                    				Vector<Float> vectY = new Vector<Float>();
                                                float fv1, fv2;

	                                        //numPoint = st.countTokens();
	                                        //savedWaveY = new float[numPoint];
			                        while( (s = in.readLine()) != null )
			                        {
                                                     StringTokenizer st1 = new StringTokenizer(s);
			                             fv1 = Float.parseFloat(st1.nextToken());
			                             fv2 = Float.parseFloat(st1.nextToken());
                                                     vectX.addElement( yxColumn ? new Float( fv2 ) : new Float( fv1 ) );
                                                     vectY.addElement( yxColumn ? new Float( fv1 ) : new Float( fv2 ) );
			                        }

					        savedWaveX = new float[vectX.size()];    
					        savedWaveY = new float[vectY.size()];    
					        for(  Float val : vectX )
					     	   savedWaveX[i++] =  val.floatValue();
					        i=0;        
					        for(  Float val : vectY )
					     	   savedWaveY[i++] =  val.floatValue();       
 

	                                    } else {
						JOptionPane.showMessageDialog(DeviceWave.this,
							"Clipboard contents syntax error\nSyntax : \nx nx1 nx2 nx3 ..... nxm\ny ny1 ny2 ny3 ..... nym",
							"Invalid clipboard contents", JOptionPane.WARNING_MESSAGE);
					        return;
					    }
                                            s = in.readLine();
                                        }
                                    }
                                    break;
                            } 
                            catch(NumberFormatException ex) 
                            {
			        JOptionPane.showMessageDialog(DeviceWave.this,
					"Clipboard contents syntax error on " +  valType + " value",
					"Invalid clipboard contents", JOptionPane.WARNING_MESSAGE);
                                return;
                            }
                            catch( IOException ex) 
                            {
			        JOptionPane.showMessageDialog(DeviceWave.this,
				        "Error on clipboard data reading",
				        "Invalid clipboard contents" , JOptionPane.WARNING_MESSAGE);
                                return;
                            }
			}
		} catch( UnsupportedFlavorException ex) 
		{
                    JOptionPane.showMessageDialog(DeviceWave.this,
                        "Clipboard data type not supported",
                        "Invalid clipboard contents" , JOptionPane.WARNING_MESSAGE);
                    return;
		}                
                               
                
                /* Clip Waveform */
                try {

		    numPoint = savedWaveX.length;
		    if(savedWaveX.length != savedWaveY.length )
		    {
		            JOptionPane.showMessageDialog(DeviceWave.this,
		                "x and y array must be equal in length",
		                "Invalid clipboard contents" , JOptionPane.WARNING_MESSAGE);
		            return;
		    }

                    for( int i = 1; i < numPoint; i++ )
                       if( savedWaveX[i] < savedWaveX[i-1])
		       {
		            JOptionPane.showMessageDialog(DeviceWave.this,
		                "X must be monotonic increasing array",
		                "Invalid clipboard contents" , JOptionPane.WARNING_MESSAGE);
		            return;
		       }  	
		      
	            if(minXVisible && setMinX) minX = savedMinX;
		    if(minYVisible && setMinY) minY = savedMinY;
		    if(maxXVisible && setMaxX) maxX = savedMaxX;
		    if(maxYVisible && setMaxY) maxY = savedMaxY;

                    Vector<Float> vectX = new Vector<Float>();
                    Vector<Float> vectY = new Vector<Float>();

                    String warningMessage = "";

                    float valX;
                    float valY;
                    boolean minXfound = false;
                    boolean maxXfound = false;

/*

x   -6 100 200 300 360 
y   10 20 20 30 100    

x   -5 100 200 300 350 
y   10 20 20 30 100    

x  -7 -5 100 200 300 350 370
y   100 10 20 20 30 100  200  

x  -7 -6 100 200 300 360 370
y   100 10 20 20 30 100  200

  
x  -8 -7 -6 100 200 300 360 370 380
y   200 100 10 20 20 30 100  200 300

x  -8 -7 -5 100 200 300 350 370 380
y   200 100 10 20 20 30 100  200 300


*/

                    for(int i = 0; i < numPoint; i++)
                    {
                        valX = savedWaveX[i];
                        valY = savedWaveY[i];
			
			if( valX < minX )
			    continue;

			if( valX > maxX )
			    break;
			
			if( valX >= minX || valX <= maxX )
                        {
		                if (valY < minY)
		                {
		                    valY = minY;
		                }
		                else if ( valY > maxY)
		                {
		                    valY = maxY;
		                }
                        } else
			    valY = 0;

                        if ( valX <= minX)
			{
                            if( minXfound )
                              continue;
                            valX = minX;
                            //valY = 0; first values should be not 0
                            minXfound = true;
                        }
                        else
                        { 
                            if ( valX >= maxX)
                            {
                               if( maxXfound )
                                  break;
                               valX = maxX;
                               //valY = 0; first values should be not 0
                               maxXfound = true;

			    } 
                            else 
                            {
                    	        if( !minXfound && (i == 0 || savedWaveX[i-1] < minX ) )
                    	        {
                		    vectX.addElement(new Float(minX));
                		    vectY.addElement(new Float(0));
                		    vectX.addElement(new Float(valX));
                		    vectY.addElement(new Float(0));
                    	        }
                    	        if( !maxXfound && ( i == (numPoint-1) || savedWaveX[i+1] > maxX ) )
                    	        {
                		    vectX.addElement(new Float(valX));
                		    vectY.addElement(new Float(valY));
                		    vectX.addElement(new Float(valX));
                		    vectY.addElement(new Float(0));
                                    valX = maxX;
                                    valY = 0;
                    	        }
                            }
                        }


                        vectX.addElement(new Float(valX));
                        vectY.addElement(new Float(valY));
                    }

                    
                     int i=0;
                     waveX = new float[vectX.size()];    
                     waveY = new float[vectY.size()];    
		     for(  Float val : vectX )
                     	waveX[i++] =  val.floatValue();
		     i=0;        
		     for(  Float val : vectY )
                     	waveY[i++] =  val.floatValue();       
 										

                     if( warningMessage.length() > 0 )		
		        JOptionPane.showMessageDialog(DeviceWave.this,
		                 warningMessage,
		                "WARNING clipboard contents", JOptionPane.WARNING_MESSAGE);


                } catch(Exception exc){
                    JOptionPane.showMessageDialog(DeviceWave.this,
                        "Invalid content in clipboard",
                        "Parsing clipboard data error", JOptionPane.WARNING_MESSAGE);
		}
                displayData(null, true);
            }
        });        
        copyPastePopup.add(pasteC);
        pasteC.setEnabled( isClipboardText(Toolkit.getDefaultToolkit().getSystemClipboard()) );


	Toolkit.getDefaultToolkit().getSystemClipboard().addFlavorListener(new FlavorListener() { 
	   @Override 
	   public void flavorsChanged(FlavorEvent e) {

	        //System.out.println("ClipBoard UPDATED: " + e.getSource() + " " + e.toString());

		Clipboard clip = (Clipboard) (e.getSource());
                pasteC.setEnabled(isClipboardText(clip));
	   } 
	}); 

        copyC = new JMenuItem("Copy to Clipboard");
        copyC.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
	    {
		String myString = ""; 

                if(minXVisible) myString =  myString + "xmin\t"+minX+"\n";
                if(minYVisible) myString =  myString + "ymin\t"+minY+"\n";
                if(maxXVisible) myString =  myString + "xmax\t"+maxX+"\n";
                if(maxYVisible) myString =  myString + "ymax\t"+maxY+"\n";

		myString = myString + "x";
                for ( float val : waveX )
			myString = myString + "\t"+val;
		myString = myString + "\ny";
                for ( float val : waveY )
			myString = myString + "\t"+val;

		//System.out.println(myString);

		StringSelection stringSelection = new StringSelection(myString);
		Clipboard clipboard = Toolkit.getDefaultToolkit().getSystemClipboard();
		clipboard.setContents(stringSelection, null);
            }
        });
        copyPastePopup.add(copyC);
        
        copyPastePopup.pack();
        copyPastePopup.setInvoker(this);

        addMouseListener(new MouseAdapter()
        {
            public void mousePressed(MouseEvent e)
            {
                if ( (e.getModifiers() & Event.META_MASK) != 0)
                {
                    if(savedWaveX == null)
                        pasteI.setEnabled(false);
                    else
                        pasteI.setEnabled(true);

                    copyPastePopup.setInvoker(DeviceWave.this);
                    copyPastePopup.show(DeviceWave.this, e.getX(), e.getY());
                }
            }
        });
    }

    protected void initializeData(Data data, boolean is_on)
    {
        create();
        initializing = true;
        //Read X and Y extremes
        NidData currNid;
        Data currData;
        float []currX, currY;
        //Min X
        try {
            currNid = new NidData(nidData.getInt() + 1);
            currData = subtree.evaluateData(currNid, 0);
            minX = minXOld = currData.getFloat();
        }catch(Exception exc) {minX = minXOld = 0;}
        //Max X
        try {
            currNid = new NidData(nidData.getInt() + 2);
            currData = subtree.evaluateData(currNid, 0);
            maxX = maxXOld = currData.getFloat();
        }catch(Exception exc) {maxX = maxXOld = 1;}

        //Min Y
        try {
            currNid = new NidData(nidData.getInt() + 3);
            currData = subtree.evaluateData(currNid, 0);
            minY = minYOld = currData.getFloat();
        }catch(Exception exc) {minY = minYOld = 0;}
        //Max Y
        try {
            currNid = new NidData(nidData.getInt() + 4);
            currData = subtree.evaluateData(currNid, 0);
            maxY = maxYOld = currData.getFloat();
        }catch(Exception exc) {maxY = maxYOld = 1;}

        //Prepare waveX and waveY
        Data xData, yData;
        try {
            //yData = subtree.evaluateData(subtree.dataFromExpr("FLOAT(" + subtree.dataToString(data) + ")"), 0);
            //xData = subtree.evaluateData(subtree.dataFromExpr("FLOAT(DIM_OF(" + subtree.dataToString(data) + "))"), 0);
            currNid = new NidData(nidData.getInt());
            currData = subtree.getData(currNid, 0);
	    yData = ((SignalData)currData).getDatum();
	    xData = ((SignalData)currData).getDimension(0);
            currY = yData.getFloatArray();
            currX = xData.getFloatArray();
        }catch(Exception exc)
        {
            currX = new float[]{minX, maxX};
            currY = new float[]{0, 0};
        }

        //Check that the stored signal lies into valid X range
        if(currX[0] <= minX - (double)MIN_STEP || currX[currX.length - 1] >= maxX + (double)MIN_STEP)
        {
            currX = new float[]{minX, maxX};
            currY = new float[]{0, 0};
            JOptionPane.showMessageDialog(DeviceWave.this,
                "The stored signal lies outside the valid X range. Hit apply to override the incorrect values.",
                "Incorret waveform limits", JOptionPane.WARNING_MESSAGE);
        }
        //set extreme points, if not present
        int nPoints = currX.length;

        if(Math.abs(currX[0] - minX) > MIN_STEP)
            nPoints++;
        else
            currX[0] = minX;
        if(Math.abs(currX[currX.length - 1] - maxX) > MIN_STEP)
            nPoints++;
        else
           currX[currX.length - 1] = maxX;

        waveX = new float[nPoints];
        waveY = new float[nPoints];

        int currIdx = 0;
        if(Math.abs(currX[0] - minX) > MIN_STEP)
        {
            waveX[0] = minX;
            waveY[0] = 0;
            currIdx++;
        }
        for(int i = 0; i < currX.length; i++)
        {
            waveX[currIdx] = currX[i];
            waveY[currIdx++] = currY[i];
        }
        if(Math.abs(currX[currX.length - 1] - maxX) > MIN_STEP)
        {
            waveX[currIdx] = maxX;
            waveY[currIdx] = 0;
        }
        waveXOld = new float[waveX.length];
        waveYOld = new float[waveX.length];
        for(int i = 0; i < waveX.length; i++)
        {
            waveXOld[i] = waveX[i];
            waveYOld[i] = waveY[i];
        }


        //updateLimits();
        displayData(data, is_on);
        initializing = false;
    }



    protected void displayData(Data data, boolean is_on)
    {
        waveEditor.setWaveform(waveX, waveY, minY, maxY);

        if(maxXVisible) {maxXField.setText(""+maxX);}
        if(minXVisible) {minXField.setText(""+minX);}
        if(maxYVisible) {maxYField.setText(""+maxY);}
        if(minYVisible) {minYField.setText(""+minY);}

        table.repaint();
    }

     protected Data getData()
    {
        Data []dims = new Data[1];
//System.out.println("waveY length " + waveY.length);
        dims[0] = new FloatArray(waveX);
        Data values = new FloatArray(waveY);
        return new SignalData(values, values, dims);
    }

    protected boolean getState(){return true;}
    public void setEnabled(boolean state) {}


    public void apply() throws Exception
    {
        CellEditor ce = table.getCellEditor();
        if(ce != null)
          ce.stopCellEditing();
        super.apply();
        updateLimits();
        if(minXVisible)
        {
            try {
                subtree.putData(new NidData(nidData.getInt() + 1), new FloatData(minX), 0);
            }catch(Exception exc)
            {
                System.out.println("Error storing min X value: "+ exc);
            }
        }
        if(maxXVisible)
        {
            try {
                subtree.putData(new NidData(nidData.getInt() + 2), new FloatData(maxX), 0);
            }catch(Exception exc)
            {
                System.out.println("Error storing max X value: "+ exc);
            }
        }
        if(minYVisible)
        {
            try {
                subtree.putData(new NidData(nidData.getInt() + 3), new FloatData(minY), 0);
            }catch(Exception exc)
            {
                System.out.println("Error storing min Y value: "+ exc);
            }
        }
        if(maxYVisible)
        {
            try {
                subtree.putData(new NidData(nidData.getInt() + 4), new FloatData(maxY), 0);
            }catch(Exception exc)
            {
                System.out.println("Error storing max Y value: "+ exc);
            }
        }
    }

    public void reset()
    {
        minX = minXOld;
        maxX = maxXOld;
        minY = minYOld;
        maxY = maxYOld;
        waveX = new float[waveXOld.length];
        waveY = new float[waveXOld.length];
        for(int i = 0; i < waveXOld.length; i++)
        {
            waveX[i] = waveXOld[i];
            waveY[i] = waveYOld[i];
        }
        super.reset();
    }

    protected void updateLimits()
    {
        if(minXVisible)
        {
            while(true)
            {
                try {
                    minX = (new Float(minXField.getText()).floatValue());
                    break;
                }catch(Exception exc)
                {
                    JOptionPane.showMessageDialog(DeviceWave.this,
                        "Invalid value for min X",
                        "Incorret limit", JOptionPane.WARNING_MESSAGE);
                }
            }
        }
        if(maxXVisible)
        {
            while(true)
            {
                try {
                    maxX = (new Float(maxXField.getText()).floatValue());
                    break;
                }catch(Exception exc)
                {
                    JOptionPane.showMessageDialog(DeviceWave.this,
                        "Invalid value for max X",
                        "Incorret limit", JOptionPane.WARNING_MESSAGE);
                }
            }
        }
        if(minYVisible)
        {
            while(true)
            {
                try {
                    minY = (new Float(minYField.getText()).floatValue());
                    break;
                }catch(Exception exc)
                {
                    JOptionPane.showMessageDialog(DeviceWave.this,
                        "Invalid value for min Y",
                        "Incorret limit", JOptionPane.WARNING_MESSAGE);
                }
            }
        }
        if(maxYVisible)
        {
            while(true)
            {
                try {
                    maxY = (new Float(maxYField.getText()).floatValue());
                    break;
                }catch(Exception exc)
                {
                    JOptionPane.showMessageDialog(DeviceWave.this,
                        "Invalid value for max Y",
                        "Incorret limit", JOptionPane.WARNING_MESSAGE);
                }
            }
            for(int i = 0; i < waveX.length; i++)
            {
                if(waveY[i] > maxY)
                    waveY[i] = maxY;
            }
            repaint();
        }
    }




    public Component add(Component c)
    {
        if(!initializing)
        {
		    JOptionPane.showMessageDialog(null, "You cannot add a component to a Device Wave. Please remove the component.",
		        "Error adding Device field", JOptionPane.WARNING_MESSAGE);
            return null;
        }
        return super.add(c);
    }

    public Component add(String name, Component c)
    {
        if(!initializing)
        {
		    JOptionPane.showMessageDialog(null, "You cannot add a component to a Device Wave. Please remove the component.",
		        "Error adding Device field", JOptionPane.WARNING_MESSAGE);
            return null;
        }
        return super.add(c);
    }

    public Component add(Component c, int intex)
    {
        if(!initializing)
        {
		    JOptionPane.showMessageDialog(null, "You cannot add a component to a Device Wave. Please remove the component.",
		        "Error adding Device field", JOptionPane.WARNING_MESSAGE);
            return null;
        }
        return super.add(c);
    }

    public void fireUpdate(String updateId, Data newExpr)
    {
        if(updateIdentifier != null && updateExpression != null && updateIdentifier.equals(updateId))
        {
            //Substitute $ in expression with the new value
            StringTokenizer st = new StringTokenizer(updateExpression, "$");
            String newExprStr = "";
            try {
                String newVal = newExpr.toString();
                while(st.hasMoreTokens())
                {
                    newExprStr += st.nextToken();
                    if(st.hasMoreTokens())
                        newExprStr += newVal;
                }
                //System.out.println(newExprStr);

                //Update first current id TDI variables
                master.updateIdentifiers();
                //Compute new Max
                Data newData = subtree.evaluateData(subtree.dataFromExpr(newExprStr), 0);
                maxY = newData.getFloat();
                //System.out.println(""+maxY);
                if(maxYVisible)
                    maxYField.setText(""+maxY);
                waveEditor.setWaveform(waveX, waveY, minY, maxY);
            }catch(Exception exc)
            {
                System.err.println("Error updating Max Y: "+exc);
            }
        }
    }

    protected Object getFullData()
    {
        Vector res = new Vector();
        res.add(new Float(minX));
        res.add(new Float(maxX));
        res.add(new Float(minY));
        res.add(new Float(maxY));
        res.add(waveX);
        res.add(waveY);
        return res;
    }
    protected void dataChanged(int offsetNid, Object data)
    {
        if(offsetNid != getOffsetNid())
            return;
        Vector inVect;
        try {
            inVect = (Vector)data;
        }catch(Exception exc)
        {
            System.err.println("Internal error: wrong data passed to DeviceWave.dataChanged");
            return;
        }
        minX = ((Float)inVect.elementAt(0)).floatValue();
        maxX = ((Float)inVect.elementAt(1)).floatValue();
        minY = ((Float)inVect.elementAt(2)).floatValue();
        maxY = ((Float)inVect.elementAt(3)).floatValue();
        float [] currX = (float [])inVect.elementAt(4);
        float [] currY = (float [])inVect.elementAt(5);
        try {
            waveX = new float[currX.length];
            waveY = new float[currY.length];
            for(int i = 0; i < currX.length; i++)
            {
                waveX[i] = currX[i];
                waveY[i] = currY[i];
            }
        }catch(Exception exc){}
        displayData(null, true);
    }
    public void setHighlight(boolean highlighted)
    {
        if(highlighted)
        {
            waveEditor.SetColors(new Color[]{Color.red}, new String[]{"Red"});
        }
        else
        {
            waveEditor.SetColors(new Color[]{Color.black}, new String[]{"Black"});
       }
       super.setHighlight(highlighted);
    }


    public static void main(String args[])
    {
        DeviceWave dw = new DeviceWave();
        System.out.println("Istanziato");
    }
}

