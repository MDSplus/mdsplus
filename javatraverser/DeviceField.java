import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;

public class DeviceField extends DeviceComponent
{
    Data data;
    public boolean textOnly = false;
    public boolean showState = false;
    public boolean displayEvaluated = false;
    public String labelString = "";
    public int numCols = 10;
    private boolean initial_state;
    protected boolean initializing = false;
    GridBagLayout gridbag;
    protected int preferredWidth = -1;
    JPanel jp;
    protected boolean isGridBag = false;

    protected String initialField;


    private boolean reportingChange = false;

    public void setNumCols(int numCols)
    {
        this.numCols = numCols;
        textF.setColumns(numCols);
        //redisplay();
    }
    public int getNumCols() {return numCols; }
    public void setEditable(boolean editable) {this.editable = editable;}
    public boolean getEditable() { return editable; }
    public boolean getDisplayEvaluated() {return displayEvaluated;}
    public void setDisplayEvaluated(boolean displayEvaluated){this.displayEvaluated = displayEvaluated;}
    public void setPreferredWidth(int preferredWidth)
    {
      this.preferredWidth = preferredWidth;
    }
    public int getPreferredWidth(){return preferredWidth;}
    public void setLabelString(String labelString)
    {
        this.labelString = labelString;
        label.setText(labelString);
        //redisplay();
    }
    public String getLabelString() {return labelString; }
    public void setShowState(boolean showState)
    {
        this.showState = showState;
        if(showState)
            checkB.setVisible(true);
        else
            checkB.setVisible(false);
        //redisplay();
    }
    public boolean getShowState() {return showState; }
    public void setTextOnly(boolean textOnly) {this.textOnly = textOnly; }
    public boolean getTextOnly() {return textOnly; }


    protected JCheckBox  checkB;
    protected JLabel label;
    protected JTextField textF;

    public DeviceField()
    {
        initializing = true;
        jp = new JPanel();
        jp.add(checkB = new JCheckBox());
        checkB.setVisible(false);
        jp.add(label = new JLabel());
        add(jp);
        add(textF = new JTextField(10));
        textF.setEnabled(editable);
        textF.setEditable(editable);
        //setLayout(gridbag = new GridBagLayout());
        initializing = false;
    }

    protected void initializeData(Data data, boolean is_on)
    {
        initializing = true;
        initial_state = is_on;

        //initialField =  Tree.dataToString(data);

       Container parent = getParent();
       if(parent.getLayout() == null)
        {
          isGridBag = false;
        }
        else
          isGridBag = true;

        GridBagConstraints gc = null;
        if(isGridBag)
        {
          setLayout(gridbag = new GridBagLayout());
          gc = new GridBagConstraints();
          gc.anchor = GridBagConstraints.WEST;
          gc.gridx = gc.gridy = 0;
          gc.gridwidth = gc.gridheight = 1;
          gc.weightx = gc.weighty = 1.;
          gc.fill = GridBagConstraints.NONE;
          gridbag.setConstraints(jp, gc);
        }
       if(showState)
        {
            //add(checkB = new JCheckBox());
            checkB.setVisible(true);
            checkB.setSelected(is_on);
            checkB.addChangeListener(new ChangeListener()
            {
                public void stateChanged(ChangeEvent e)
                {
                    boolean state = checkB.isSelected();
                    if(label != null)
                        label.setEnabled(state);
                    if(textF != null && editable)
                    {
                      textF.setEnabled(state);
                      textF.setEditable(state);
                    }
                }
            });
        }
        if(textF != null && isGridBag)
        {
          gc.gridx++;
          gc.anchor = GridBagConstraints.EAST;
          gridbag.setConstraints(textF, gc);
        }

        displayData(data, is_on);
        setEnabled(is_on);

        textF.addKeyListener(new KeyAdapter() {
          public void keyTyped(KeyEvent e) {
            reportingChange = true;
            reportDataChanged(textF.getText());
            reportingChange = false;
          }
        });

        textF.setEnabled(editable);
        textF.setEditable(editable);
        if(preferredWidth > 0)
        {
          setPreferredSize(new Dimension(preferredWidth, getPreferredSize().height));
          setSize(new Dimension(preferredWidth, getPreferredSize().height));
        }
        redisplay();
        initializing = false;
    }

    protected void dataChanged(int offsetNid, Object data)
    {
      if(reportingChange || this.offsetNid != offsetNid)
        return;
      try {
        textF.setText( (String) data);
      }catch(Exception exc){}
    }

    void postApply()
    {
        if(editable || !displayEvaluated || data == null) return;
        //Nothing to do if the field is not editable and displays evaulated data
        String textString;
        try {
            textString = Tree.dataToString(subtree.evaluateData(data, 0));
        }catch(Exception exc){textString = Tree.dataToString(data);}
        if(textString != null)
        {
            if(textOnly && textString.charAt(0) == '"')
                textF.setText(textString.substring(1, textString.length() - 1));
            else
                textF.setText(textString);
        }
    }


    protected void displayData(Data data, boolean is_on)
    {
        this.data = data;
        initial_state = is_on;
        if(showState)
            checkB.setSelected(is_on);
        if(data != null)
        {
            String textString;
            if(displayEvaluated)
            {
                try {
                    initialField = textString = Tree.dataToString(subtree.evaluateData(data, 0));
                }catch(Exception exc){textString = Tree.dataToString(data);}
            }
            else
                textString = Tree.dataToString(data);
            if(textString != null)
            {
                if(textOnly && textString.charAt(0) == '"')
                    textF.setText(textString.substring(1, textString.length() - 1));
                else
                    textF.setText(textString);
            }
        }
        else
            textF.setText("");
        label.setEnabled(is_on);
        textF.setEnabled(is_on & editable);
        textF.setEditable(is_on & editable);
    }


    protected Data getData()
    {
        String dataString = textF.getText();
        if(dataString == null) return null;
        if(textOnly)
            return Tree.dataFromExpr("\""+dataString+"\"");
        else
            return Tree.dataFromExpr(dataString);
    }

    protected boolean getState()
    {
        if(!showState)
            return initial_state;
        else
            return checkB.isSelected();
    }
    public void setEnabled(boolean state)
    {
        //if(checkB != null) checkB.setEnabled(state);
        if(textF != null)
        {
            textF.setEnabled(state);
            textF.setEditable(state);
        }
        if(label != null) label.setEnabled(state);
        //if(checkB != null) checkB.setSelected(state);
        //initial_state = state;
    }

    public Component add(Component c)
    {
        if(!initializing)
        {
		    JOptionPane.showMessageDialog(null, "You cannot add a component to a Device Field. Please remove the component.",
		        "Error adding Device field", JOptionPane.WARNING_MESSAGE);
            return null;
        }
        return super.add(c);
    }

    public Component add(String name, Component c)
    {
        if(!initializing)
        {
		    JOptionPane.showMessageDialog(null, "You cannot add a component to a Device Field. Please remove the component.",
		        "Error adding Device field", JOptionPane.WARNING_MESSAGE);
            return null;
        }
        return super.add(c);
    }

    public Component add(Component c, int intex)
    {
        if(!initializing)
        {
		    JOptionPane.showMessageDialog(null, "You cannot add a component to a Device Field. Please remove the component.",
		        "Error adding Device field", JOptionPane.WARNING_MESSAGE);
            return null;
        }
        return super.add(c);
    }

    public boolean supportsState(){return showState;}

    public void setBounds(int x, int y, int width, int height)
    {
      super.setBounds(x,y,width,height);
      setPreferredSize(new Dimension(width, height));
    }

    protected boolean isDataChanged()
    {
      if(displayEvaluated && initialField != null)
        return !(textF.getSize().equals(initialField));
      else
        return true;
    }

}

