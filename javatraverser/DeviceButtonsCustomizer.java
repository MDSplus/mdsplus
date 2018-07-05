import java.awt.BorderLayout;
import java.awt.Button;
import java.awt.GridLayout;
import java.awt.Label;
import java.awt.Panel;
import java.awt.TextField;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.beans.Customizer;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.util.StringTokenizer;
import java.util.Vector;

public class DeviceButtonsCustomizer extends Panel implements Customizer
{
    DeviceButtons bean = null;
    PropertyChangeSupport listeners = new PropertyChangeSupport(this);

    TextField expression, message, methods;
    java.awt.List exprList;
    Vector expressionsV = new Vector(), messagesV = new Vector();

    Button addButton, removeButton, doneButton;

    public DeviceButtonsCustomizer()
    {
    }
    public void setObject(Object o)
    {
        bean = (DeviceButtons)o;

        setLayout(new BorderLayout());
        Panel jp = new Panel();
        jp.setLayout(new BorderLayout());
        jp.add(exprList = new java.awt.List(10, false), "Center");
        exprList.addItemListener(new ItemListener() {
            public void itemStateChanged(ItemEvent e)
            {
                int idx = exprList.getSelectedIndex();
                expression.setText((String)(expressionsV.elementAt(idx)));
                message.setText((String)(messagesV.elementAt(idx)));
            }
        });


        String exprs[] = bean.getCheckExpressions();
        if(exprs != null)
        {
            String messgs[] = bean.getCheckMessages();
            if(messgs != null)
            {
                for(int i = 0; i < exprs.length; i++)
                {
                    if(i >= messgs.length) break;
                    messagesV.addElement(messgs[i]);
                    expressionsV.addElement(exprs[i]);
                    exprList.add(exprs[i] + " : " + messgs[i]);
                }
            }
        }
        Panel jp1 = new Panel();
        jp1.add(new Label("Check expr.: "));
        jp1.add(expression = new TextField(30));
        jp1.add(new Label("Error message: "));
        jp1.add(message = new TextField(30));
        jp.add(jp1, "South");

        jp1 = new Panel();
        jp1.setLayout(new GridLayout(2, 1));
        Panel jp2 = new Panel();
        jp2.add(addButton = new Button("Add"));
        jp1.add(jp2);
        jp2 = new Panel();
        jp2.add(removeButton = new Button("Remove"));
        jp1.add(jp2);
        jp.add(jp1, "East");
        addButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                String currExpr = expression.getText();
                if(currExpr != null && currExpr.length() > 0)
                {
                    String currMess = message.getText();
                    if(currMess != null && currMess.length() > 0)
                    {
                        messagesV.addElement(currMess);
                        expressionsV.addElement(currExpr);
                        exprList.add(currExpr + " : " + currMess);
                    }
                }
            }
        });
        removeButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                int idx = exprList.getSelectedIndex();
        //        exprList.delItem(idx);
                exprList.remove(idx);
                messagesV.removeElementAt(idx);
                expressionsV.removeElementAt(idx);
            }
        });
        add(jp, "Center");
        jp = new Panel();
        jp.setLayout(new BorderLayout());
        jp1 = new Panel();
        jp1.add(new Label("Methods: "));
        jp1.add(methods = new TextField(40));
        String [] methodList = bean.getMethods();
        if(methodList != null && methodList.length > 0)
        {
            String method_txt = methodList[0];
            for(int i = 1; i < methodList.length; i++)
            {
                method_txt += " " + methodList[i];
            }
            methods.setText(method_txt);
        }


        jp.add(jp1, "North");
        jp1 = new Panel();
        jp1.add(doneButton = new Button("Apply"));
        jp.add(jp1, "South");
        doneButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
                String [] messages = new String[messagesV.size()];
                String [] expressions = new String[expressionsV.size()];
                for(int i = 0; i < messagesV.size(); i++)
                {
                    messages[i] = (String)(messagesV.elementAt(i));
                    expressions[i] = (String)(expressionsV.elementAt(i));
                }
                String []oldCheckMessages = bean.getCheckMessages();
                bean.setCheckMessages(messages);
                listeners.firePropertyChange("checkMessages", oldCheckMessages, bean.getCheckMessages());
                String []oldCheckExpressions = bean.getCheckExpressions();
                bean.setCheckExpressions(expressions);
                listeners.firePropertyChange("checkExpressions", oldCheckExpressions, bean.getCheckExpressions());
                String method_list = methods.getText();
                //System.out.println(method_list);
                StringTokenizer st = new StringTokenizer(method_list, " ,;");
                int num_methods = st.countTokens();
                if(num_methods > 0)
                {
                    String [] methods = new String[num_methods];
                    int i = 0;
                    while(st.hasMoreTokens())
                    {
                        methods[i] = st.nextToken();
                        //System.out.println(methods[i]);
                        i++;
                    }
                    String []oldMethods = bean.getMethods();
                    bean.setMethods(methods);
                    listeners.firePropertyChange("methods", oldMethods, bean.getMethods());

                }
            }
        });
        add(jp,"South");
    }

    public void addPropertyChangeListener(PropertyChangeListener l)
    {
        listeners.addPropertyChangeListener(l);
    }

    public void removePropertyChangeListener(PropertyChangeListener l)
    {
        listeners.removePropertyChangeListener(l);
    }
  }

