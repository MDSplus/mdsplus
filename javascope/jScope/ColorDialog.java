package jScope;

/* $Id$ */
import jScope.Waveform;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;
import javax.swing.*;
import javax.swing.event.*;

class ColorDialog
    extends JDialog
    implements ActionListener, ItemListener
{
    JList<String> colorList;
    DefaultListModel<String> listModel = new DefaultListModel<>();
    JTextField colorName;
    JComboBox<String> color;
    JSlider red, green, blue;
    JButton ok, reset, cancel, add, erase;
    jScopeFacade main_scope;
    JLabel label;
    Canvas color_test;
    int red_i, green_i, blue_i;
    boolean changed = false;
    Vector<Item> color_set = new Vector<>();
    Vector<Item> color_set_clone;
    Color color_vector[];
    String color_name[];
    int colorMapIndex[] = null;
    private boolean reversed = false;

    static class Item
    {
        String name;
        Color color;

        Item(String n, Color c)
        {
            name = new String(n);
            color = c;
        }
    }

    ColorDialog(Frame dw, String title)
    {

        super(dw, title, true);

        main_scope = (jScopeFacade) dw;

        GetPropertiesValue();

        GridBagConstraints c = new GridBagConstraints();
        GridBagLayout gridbag = new GridBagLayout();
        getContentPane().setLayout(gridbag);

        c.insets = new Insets(4, 4, 4, 4);
        c.fill = GridBagConstraints.BOTH;


        c.gridwidth = GridBagConstraints.REMAINDER;
        label = new JLabel("Color list customization");
        gridbag.setConstraints(label, c);
        getContentPane().add(label);

//	Panel p0 = new Panel();
//      p0.setLayout(new FlowLayout(FlowLayout.LEFT));

        c.gridwidth = GridBagConstraints.BOTH;
        label = new JLabel("Name");
        gridbag.setConstraints(label, c);
        getContentPane().add(label);

        colorName = new JTextField(15);
        colorName.addKeyListener(
            new KeyAdapter()
        {
            public void keyPressed(KeyEvent e)
            {
                keyPressedAction(e);
            }
        });
        gridbag.setConstraints(colorName, c);
        getContentPane().add(colorName);

        if (GetNumColor() == 0)
            ColorSetItems(Waveform.COLOR_NAME, Waveform.COLOR_SET);

        SetColorVector();
        GetColorsName();

        color = new JComboBox<>();
        for (String name : color_name)
            color.addItem(name);

        color.addItemListener(this);
        gridbag.setConstraints(color, c);
        getContentPane().add(color);

        c.gridwidth = GridBagConstraints.REMAINDER;
        color_test = new Canvas();
//	color_test.setBounds(10,10,10,10);
        color_test.setBackground(Color.black);
        gridbag.setConstraints(color_test, c);
        getContentPane().add(color_test);

        c.gridwidth = 2;
        c.gridheight = 5;
        colorList = new JList<>(listModel);
        JScrollPane scrollColorList = new JScrollPane(colorList);
        colorList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        colorList.addListSelectionListener(
            new ListSelectionListener()
        {
            public void valueChanged(ListSelectionEvent e)
            {
                int color_idx = ( (JList) e.getSource()).getSelectedIndex();
                if (color_idx >= 0 && color_idx < color_set.size())
                {
                    Item c_item = color_set.elementAt(color_idx);
                    ColorDialog.this.SetSliderToColor(c_item.color);
                    ColorDialog.this.colorName.setText(c_item.name);
                }
            }
        });
        colorList.addKeyListener(
            new KeyAdapter()
        {
            public void keyPressed(KeyEvent e)
            {
                keyPressedAction(e);
            }
        });
        gridbag.setConstraints(scrollColorList, c);
        getContentPane().add(scrollColorList);

        label = new JLabel("Red");
        //label.setForeground(Color.red);
        c.gridheight = 1;
        gridbag.setConstraints(label, c);
        getContentPane().add(label);

        Hashtable<Integer, JLabel> labelTable = new Hashtable<>();
        labelTable.put(0, new JLabel("0"));
        labelTable.put(64, new JLabel("64"));
        labelTable.put(128, new JLabel("128"));
        labelTable.put(192, new JLabel("192"));
        labelTable.put(255, new JLabel("255"));

        c.gridwidth = GridBagConstraints.REMAINDER;
        c.gridheight = 1;
        red = new JSlider(SwingConstants.HORIZONTAL, 0, 255, 0);
        red.setMinorTickSpacing(8);
        red.setPaintTicks(true);
        red.setPaintLabels(true);
        red.setLabelTable(labelTable);
        red.setBorder(BorderFactory.createEmptyBorder(0, 0, 20, 0));
        red.addChangeListener(
            new ChangeListener()
        {
            public void stateChanged(ChangeEvent e)
            {
                ColorDialog.this.colorValueChanged(e);
            }
        });
        gridbag.setConstraints(red, c);
        getContentPane().add(red);

        c.gridwidth = GridBagConstraints.BOTH;
        label = new JLabel("Green");
        //label.setForeground(Color.green);
        c.gridheight = 1;
        gridbag.setConstraints(label, c);
        getContentPane().add(label);

        c.gridwidth = GridBagConstraints.REMAINDER;
        green = new JSlider(SwingConstants.HORIZONTAL, 0, 255, 0);
        green.setMinorTickSpacing(8);
        green.setPaintTicks(true);
        green.setPaintLabels(true);
        green.setLabelTable(labelTable);
        green.setBorder(BorderFactory.createEmptyBorder(0, 0, 20, 0));
        green.addChangeListener(
            new ChangeListener()
        {
            public void stateChanged(ChangeEvent e)
            {
                ColorDialog.this.colorValueChanged(e);
            }
        });
        gridbag.setConstraints(green, c);
        getContentPane().add(green);

        c.gridwidth = GridBagConstraints.BOTH;
        label = new JLabel("Blue");
        //label.setForeground(Color.blue);
        c.gridheight = 1;
        gridbag.setConstraints(label, c);
        getContentPane().add(label);

        c.gridwidth = GridBagConstraints.REMAINDER;
        blue = new JSlider(SwingConstants.HORIZONTAL, 0, 255, 0);
        blue.setMinorTickSpacing(8);
        blue.setPaintTicks(true);
        blue.setPaintLabels(true);
        blue.setLabelTable(labelTable);
        blue.setBorder(BorderFactory.createEmptyBorder(0, 0, 20, 0));
        blue.addChangeListener(
            new ChangeListener()
        {
            public void stateChanged(ChangeEvent e)
            {
                ColorDialog.this.colorValueChanged(e);
            }
        });
        gridbag.setConstraints(blue, c);
        getContentPane().add(blue);

        JPanel p1 = new JPanel();
        p1.setLayout(new FlowLayout(FlowLayout.CENTER));

        ok = new JButton("Ok");
        ok.addActionListener(this);
        p1.add(ok);

        add = new JButton("Add/Apply");
        add.addActionListener(this);
        p1.add(add);

        erase = new JButton("Erase");
        erase.addActionListener(this);
        p1.add(erase);

        reset = new JButton("Reset");
        reset.addActionListener(this);
        p1.add(reset);

        cancel = new JButton("Cancel");
        cancel.addActionListener(this);
        p1.add(cancel);

        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(p1, c);
        getContentPane().add(p1);
    }

    private void GetPropertiesValue()
    {
        Properties js_prop = main_scope.js_prop;
        if (js_prop == null)
            return;

        String prop;
        for (int i = 0; ; ++i) {
            prop = js_prop.getProperty("jScope.item_color_" + i);
            if (prop == null)
                break;

            int len = prop.indexOf(",");
            String name = new String(prop.substring(0, len));
            Color cr = StringToColor(new String(prop.substring(len + 2, prop.length())));
            InsertItemAt(name, cr, i);
        }
    }

    private Vector<Item> CopyColorItemsVector(Vector<Item> in)
    {
        Vector<Item> out = new Vector<>(in.size());
        for (int i = 0; i < in.size(); i++)
            out.addElement(new Item( ( (Item) in.elementAt(i)).name,
                                    ( (Item) in.elementAt(i)).color));
        return out;
    }

    public void ShowColorDialog(Component f)
    {
        setColorItemToList();
//	color_set_clone = (Vector)color_set.clone();
        color_set_clone = CopyColorItemsVector(color_set);
        pack();
        this.setLocationRelativeTo(f);
        setVisible(true);
    }

    public void SetReversed(boolean reversed)
    {
        if (this.reversed != reversed)
        {
            this.reversed = reversed;
            ReversedColor(color_name, color_vector);
            SetColorVector();
            GetColorsName();
        }
    }

    private void ReversedColor(String[] color_name, Color[] colors)
    {
        for (int i = 0; i < color_name.length; i++)
        {
            if (reversed)
            {
                if (colors[i].equals(Color.black))
                {
                    Item c_item = new Item("White", Color.white);
                    color_set.setElementAt(c_item, i);
                    break;
                }
            }
            else
            {
                if (colors[i].equals(Color.white))
                {
                    Item c_item = new Item("Black", Color.black);
                    color_set.setElementAt(c_item, i);
                    break;
                }
            }
        }
    }

    public void ColorSetItems(String[] color_name, Color[] colors)
    {
        for (int i = 0; i < color_name.length; i++)
        {
            if (reversed)
            {
                if (colors[i].equals(Color.black))
                {
                    Item c_item = new Item("White", Color.white);
                    color_set.addElement(c_item);
                    continue;
                }
            }
            else
            {
                if (colors[i].equals(Color.white))
                {
                    Item c_item = new Item("Black", Color.black);
                    color_set.addElement(c_item);
                    continue;
                }
            }

            Item c_item = new Item(color_name[i], colors[i]);
            color_set.addElement(c_item);
        }
    }

    public Color GetColorAt(int idx)
    {
        if (idx >= 0 && idx < color_set.size())
        {
            Color color = ( (Item) color_set.elementAt(idx)).color;
            return color;
        }
        return null;
    }

    public String GetNameAt(int idx)
    {
        if (idx >= 0 && idx < color_set.size())
        {
            String name = ( (Item) color_set.elementAt(idx)).name;
            return name;
        }
        return null;
    }

    public int GetNumColor()
    {
        return color_set.size();
    }

    public String[] GetColorsName()
    {
        color_name = null;

        if (color_set.size() > 0)
        {
            color_name = new String[color_set.size()];
            for (int i = 0; i < color_set.size(); i++)
                color_name[i] = color_set.elementAt(i).name;
        }
        return color_name;
    }

    public void itemStateChanged(ItemEvent e)
    {
        Object ob = e.getSource();
        if (ob == color)
        {
            int color_idx = color.getSelectedIndex();
            colorName.setText(Waveform.COLOR_NAME[color_idx]);
            SetSliderToColor(Waveform.COLOR_SET[color_idx]);

        }
    }

    public void removeAllColorItems()
    {
        if (color_set.size() != 0)
            color_set.removeAllElements();
    }

    public int InsertItemAt(String name, Color color)
    {
        int i;
        String ext = "";
        int extIdx = 1;

        for (i = 0; i < color_set.size(); i++)
        {
            if ( ( ( (Item) color_set.elementAt(i)).name.equals(name + ext)))
            {
                ext = "_" + extIdx;
                extIdx++;
            }
            if ( ( ( (Item) color_set.elementAt(i)).color.equals(color)))
                return i;
        }

        Item c_item = new Item(name + ext, color);
        color_set.insertElementAt(c_item, i);
        return i;
    }

    public void InsertItemAt(String name, Color color, int idx)
    {
        Item c_item = new Item(name, color);
        color_set.insertElementAt(c_item, idx);
    }

    private Color getColor()
    {
        return new Color(red.getValue(), green.getValue(), blue.getValue());
    }

    public Color[] SetColorVector()
    {
        color_vector = new Color[color_set.size()];

        for (int i = 0; i < color_set.size(); i++)
            color_vector[i] = ( (Item) color_set.elementAt(i)).color;
        return color_vector;
    }

    public Color[] GetColors()
    {
        return color_vector;
    }

    public int[] getColorMapIndex()
    {
        return colorMapIndex;
    }

    private void SetSliderToColor(Color c)
    {
        red.setValue(c.getRed());
        green.setValue(c.getGreen());
        blue.setValue(c.getBlue());
        color_test.setBackground(c);
        color_test.repaint();
    }

    private void AddUpdateItem(String name, Color color)
    {
        int i;
        if (name == null || name.length() == 0)
            return;

        Item c_item = new Item(name, color);
        String c_name[] = GetColorsName();
        for (i = 0;
             c_name != null && i < c_name.length && !c_name[i].equals(name); i++)
            ;
        if (c_name == null || i == c_name.length)
        {
            color_set.addElement(c_item);
            listModel.addElement(name);
        }
        else
            color_set.setElementAt(c_item, i);
    }

    public void keyPressedAction(KeyEvent e)
    {
        Object ob = e.getSource();
        char key = e.getKeyChar();

        if (key == KeyEvent.CHAR_UNDEFINED)
            return;

        if (key == KeyEvent.VK_DELETE)
        {
            if (ob == colorList)
            {
                int idx = colorList.getSelectedIndex();
                listModel.remove(idx);
                color_set.removeElementAt(idx);
                colorName.setText("");
            }
        }
        if (key == KeyEvent.VK_ENTER)
        {
            if (ob == colorName)
            {
                AddUpdateItem(colorName.getText(), getColor());
            }
        }
    }

    public void setColorItemToList()
    {
        if (listModel.getSize() > 0)
            listModel.clear();
        for (int i = 0; i < color_set.size(); i++)
        {
            listModel.addElement( ( (Item) color_set.elementAt(i)).name);
        }
    }

    public void colorValueChanged(ChangeEvent e)
    {
        color_test.setBackground(getColor());
        color_test.repaint();
    }

    private Color StringToColor(String str)
    {
        int pos;
        String tmp = str.substring(str.indexOf("=") + 1, pos = str.indexOf(","));
        int r = Integer.parseInt(tmp);
        tmp = str.substring(pos + 3, pos = str.indexOf(",", pos + 1));
        int g = Integer.parseInt(tmp);
        tmp = str.substring(pos + 3, str.indexOf("]", pos + 1));
        int b = Integer.parseInt(tmp);
        int c = (r << 16 | g << 8 | b);
        return (new Color(c));
    }

    public void FromFile(Properties pr, String prompt) throws IOException
    {
        String prop;
        int idx = 0;
        Vector<Integer> newColorMap = new Vector<>();
	removeAllColorItems();

         //Syntax  Scope.color_x: <name>,java.awt.Color[r=xxx,g=xxx,b=xxx]

         while ( (prop = pr.getProperty(prompt + idx)) != null)
        {
            StringTokenizer st = new StringTokenizer(prop, ",");
            String name = st.nextToken();
            st.nextToken("["); // dummy java.awt.Color[
            Color cr = StringToColor(st.nextToken("")); //remained string r=xxx,g=xxx,b=xxx]
//              InsertItemAt(name, cr, idx);
            newColorMap.addElement(new Integer(InsertItemAt(name, cr)));
            idx++;
        }

        colorMapIndex = new int[newColorMap.size()];
        for (int i = 0; i < newColorMap.size(); i++)
        {
            colorMapIndex[i] = ( (Integer) newColorMap.elementAt(i)).intValue();
        }

        //Set default color list if is not defined color
        //in configuration file
        if (GetNumColor() == 0)
        {
            if (main_scope.js_prop != null)
                GetPropertiesValue();
            else
                ColorSetItems(Waveform.COLOR_NAME, Waveform.COLOR_SET);
        }
        SetColorVector();
        GetColorsName();
    }

    public void toFile(PrintWriter out, String prompt)
    {
        for (int i = 0; i < GetNumColor(); i++)
        {
            out.println(prompt + i + ": " + GetNameAt(i) + "," + GetColorAt(i));
        }
        out.println("");
    }

    public void actionPerformed(ActionEvent e)
    {

        Object ob = e.getSource();

        if (ob == ok)
        {
            if (ob == ok)
            {
                color_set_clone = null;
                setVisible(false);
            }
            AddUpdateItem(colorName.getText(), getColor());
            SetColorVector();
            main_scope.UpdateColors();
            main_scope.RepaintAllWaves();
            main_scope.setChange(true);
        }

        if (ob == add)
        {
            AddUpdateItem(colorName.getText(), getColor());
        }

        if (ob == erase)
        {
            colorName.setText("");
            removeAllColorItems();
            if (listModel.getSize() > 0)
                listModel.clear();
            AddUpdateItem(Waveform.COLOR_NAME[0], Waveform.COLOR_SET[0]);
            SetColorVector();
        }

        if (ob == reset)
        {
            color_set = CopyColorItemsVector(color_set_clone);
            setColorItemToList();
            SetColorVector();
        }

        if (ob == cancel)
        {
            color_set = CopyColorItemsVector(color_set_clone);
            setColorItemToList();
            SetColorVector();
            color_set_clone = null;
            setVisible(false);
        }
    }
}
