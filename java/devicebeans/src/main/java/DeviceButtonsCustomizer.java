import java.awt.*;
import java.awt.event.*;
import java.beans.*;
import java.util.StringTokenizer;
import java.util.Vector;

public class DeviceButtonsCustomizer extends Panel implements Customizer
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	DeviceButtons bean = null;
	PropertyChangeSupport listeners = new PropertyChangeSupport(this);
	TextField expression, message, methods;
	java.awt.List exprList;
	Vector<String> expressionsV = new Vector<>(), messagesV = new Vector<>();
	Button addButton, removeButton, doneButton;

	public DeviceButtonsCustomizer()
	{}

	@Override
	public void addPropertyChangeListener(PropertyChangeListener l)
	{
		listeners.addPropertyChangeListener(l);
	}

	@Override
	public void removePropertyChangeListener(PropertyChangeListener l)
	{
		listeners.removePropertyChangeListener(l);
	}

	@Override
	public void setObject(Object o)
	{
		bean = (DeviceButtons) o;
		setLayout(new BorderLayout());
		Panel jp = new Panel();
		jp.setLayout(new BorderLayout());
		jp.add(exprList = new java.awt.List(10, false), "Center");
		exprList.addItemListener(new ItemListener()
		{
			@Override
			public void itemStateChanged(ItemEvent e)
			{
				final int idx = exprList.getSelectedIndex();
				expression.setText((expressionsV.elementAt(idx)));
				message.setText((messagesV.elementAt(idx)));
			}
		});
		final String exprs[] = bean.getCheckExpressions();
		if (exprs != null)
		{
			final String messgs[] = bean.getCheckMessages();
			if (messgs != null)
			{
				for (int i = 0; i < exprs.length; i++)
				{
					if (i >= messgs.length)
						break;
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
			@Override
			public void actionPerformed(ActionEvent e)
			{
				final String currExpr = expression.getText();
				if (currExpr != null && currExpr.length() > 0)
				{
					final String currMess = message.getText();
					if (currMess != null && currMess.length() > 0)
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
			@Override
			public void actionPerformed(ActionEvent e)
			{
				final int idx = exprList.getSelectedIndex();
				// exprList.delItem(idx);
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
		final String[] methodList = bean.getMethods();
		if (methodList != null && methodList.length > 0)
		{
			String method_txt = methodList[0];
			for (int i = 1; i < methodList.length; i++)
			{
				method_txt += " " + methodList[i];
			}
			methods.setText(method_txt);
		}
		jp.add(jp1, "North");
		jp1 = new Panel();
		jp1.add(doneButton = new Button("Apply"));
		jp.add(jp1, "South");
		doneButton.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				final String[] messages = new String[messagesV.size()];
				final String[] expressions = new String[expressionsV.size()];
				for (int i = 0; i < messagesV.size(); i++)
				{
					messages[i] = (messagesV.elementAt(i));
					expressions[i] = (expressionsV.elementAt(i));
				}
				final String[] oldCheckMessages = bean.getCheckMessages();
				bean.setCheckMessages(messages);
				listeners.firePropertyChange("checkMessages", oldCheckMessages, bean.getCheckMessages());
				final String[] oldCheckExpressions = bean.getCheckExpressions();
				bean.setCheckExpressions(expressions);
				listeners.firePropertyChange("checkExpressions", oldCheckExpressions, bean.getCheckExpressions());
				final String method_list = methods.getText();
				// System.out.println(method_list);
				final StringTokenizer st = new StringTokenizer(method_list, " ,;");
				final int num_methods = st.countTokens();
				if (num_methods > 0)
				{
					final String[] methods = new String[num_methods];
					int i = 0;
					while (st.hasMoreTokens())
					{
						methods[i] = st.nextToken();
						// System.out.println(methods[i]);
						i++;
					}
					final String[] oldMethods = bean.getMethods();
					bean.setMethods(methods);
					listeners.firePropertyChange("methods", oldMethods, bean.getMethods());
				}
			}
		});
		add(jp, "South");
	}
}
