//package jTraverser;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class DataEditor
    extends JPanel
    implements ActionListener, Editor
{
  LabeledExprEditor expr_edit, units_edit;
  ParameterEditor param_edit;
  PythonEditor python_edit;
  JPanel panel;
  JComboBox combo;
  int mode_idx, curr_mode_idx;
  Data data;
  Data units;
  boolean editable = true;
  TreeDialog dialog;

  public DataEditor(Data data, TreeDialog dialog)
  {
    this.dialog = dialog;
    this.data = data;
    if (data == null)
    {
      mode_idx = 0;
      this.data = null;
      units = null;
    }
    else
    {
      if (data instanceof ParameterData)
          mode_idx = 2;
      else if(data instanceof FunctionData && ((FunctionData)data).opcode == PythonEditor.OPC_FUN)
      {
          Data[] args = ((FunctionData)data).getArgs();
          try {
          if(args != null && args.length > 2 && args[1] != null && (args[1] instanceof StringData) &&
            args[1].getString()!= null && args[1].getString().toUpperCase().equals("PY"))
                mode_idx = 3;
            else
                mode_idx = 1;
          }catch(Exception exc){mode_idx = 1;}
      } 
      else
          mode_idx = 1;
      if (data.dtype == Data.DTYPE_WITH_UNITS)
      {
        this.data = ( (WithUnitsData) data).getDatum();
        units = ( (WithUnitsData) data).getUnits();
      }
      else
      {
        this.data = data;
        units = null;
      }
    }

    curr_mode_idx = mode_idx;
    String names[] = {"Undefined", "Expression", "Parameter", "Python Expression"};
    combo = new JComboBox(names);
    combo.setEditable(false);
    combo.setSelectedIndex(mode_idx);
    combo.addActionListener(this);
    setLayout(new BorderLayout());
	JPanel jp = new JPanel();
	jp.add(combo);
    add(jp, BorderLayout.NORTH);
    addEditor();
  }

  private void addEditor()
  {
    panel = new JPanel();
    panel.setLayout(new BorderLayout());
    switch (curr_mode_idx)
    {
      case 0:
        return;
      case 1:
        panel.add(expr_edit = new LabeledExprEditor(data));
        break;
      case 2:
        Data _data, _help = null, _validation = null;
        if (data != null && data instanceof ParameterData)
        {
            _data = ( (ParameterData)data).getDatum();
            _help = ( (ParameterData)data).getHelp();
            _validation = ( (ParameterData)data).getValidation();
        }
        else
            _data = data;
        param_edit = new ParameterEditor(new ExprEditor( _data, false, 3, 20),
                                         new ExprEditor( _help, true, 4, 20),
                                         new ExprEditor( _validation, false, 1, 20));
        panel.add(param_edit);
        break;
      case 3: 
        if (data != null && data instanceof FunctionData)
        {
          python_edit = new PythonEditor(((FunctionData)data).getArgs());
        }
        else
        {
          python_edit = new PythonEditor(null);
        }
        panel.add(python_edit);
        break;
    }
    units_edit = new LabeledExprEditor("Units", new ExprEditor(units, true));
    panel.add(units_edit, BorderLayout.NORTH);
    add(panel, BorderLayout.CENTER);
  }

  public void actionPerformed(ActionEvent e)
  {
    if (!editable)
    {
      combo.setSelectedIndex(curr_mode_idx);
      return;
    }
    int idx = combo.getSelectedIndex();
    if (idx == curr_mode_idx)
      return;
    remove(panel);
    curr_mode_idx = idx;
    addEditor();
    validate();
    dialog.repack();
  }

  public void reset()
  {
    if (curr_mode_idx>0)
        remove(panel);
    curr_mode_idx = mode_idx;
    combo.setSelectedIndex(mode_idx);
    addEditor();
    validate();
    dialog.repack();
  }

  public Data getData()
  {
    Data units;
    switch (curr_mode_idx)
    {
      case 0:
        return null;
      case 1:
        units = units_edit.getData();
        if (units != null)
        {
          if (units instanceof StringData &&
              ( (StringData) units).datum.equals(""))
            return expr_edit.getData();
          else
            return new WithUnitsData(expr_edit.getData(), units);
        }
        else
          return expr_edit.getData();
        case 2:
          units = units_edit.getData();
          if (units != null)
          {
            if (units instanceof StringData &&
                ( (StringData) units).datum.equals(""))
              return param_edit.getData();
            else
              return new WithUnitsData(param_edit.getData(), units);
          }
          else
            return param_edit.getData();

        case 3:
          units = units_edit.getData();
          if (units != null)
          {
            if (units instanceof StringData &&
                ( (StringData) units).datum.equals(""))
              return python_edit.getData();
            else
              return new WithUnitsData(python_edit.getData(), units);
          }
          else
            return python_edit.getData();
    }
    return null;
  }

  public void setData(Data data)
  {
    this.data = data;
    if (data == null)
    {
      mode_idx = 0;
      this.data = null;
      units = null;
    }
    else
    {
      if(data instanceof ParameterData)
        mode_idx = 2;
      else if(data instanceof FunctionData && ((FunctionData)data).opcode == PythonEditor.OPC_FUN)
      {
          Data[] args = ((FunctionData)data).getArgs();
          try {
          if(args != null && args.length > 2 && args[1] != null && (args[1] instanceof StringData) &&
            args[1].getString()!= null && args[1].getString().toUpperCase().equals("PY"))
                mode_idx = 3;
            else
                mode_idx = 1;
          }catch(Exception exc){mode_idx = 1;}
      } 
      else
        mode_idx = 1;
      if (data.dtype == Data.DTYPE_WITH_UNITS)
      {
        this.data = ( (WithUnitsData) data).getDatum();
        units = ( (WithUnitsData) data).getUnits();
      }
      else
      {
        this.data = data;
        units = null;
      }
    }
    reset();
  }

  public void setEditable(boolean editable)
  {
    this.editable = editable;
    if (expr_edit != null)
      expr_edit.setEditable(editable);
    if (python_edit != null)
      python_edit.setEditable(editable);
    if (units_edit != null)
      units_edit.setEditable(editable);
  }

}