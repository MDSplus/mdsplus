package mds.jscope;

/* $Id$ */
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.TableModel;

import mds.wave.WaveInterface;

public class SignalsBoxDialog extends JDialog
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	jScopeFacade scope;
	JTable table;
	TableModel dataModel;

	SignalsBoxDialog(JFrame f, String title, boolean modal)
	{
		super(f, title, false);
		scope = (jScopeFacade) f;
		dataModel = new AbstractTableModel()
		{
			/**
			 *
			 */
			private static final long serialVersionUID = 1L;

			@Override
			public int getColumnCount()
			{ return 2; }

			@Override
			public String getColumnName(int col)
			{
				switch (col)
				{
				case 0:
					return "Y expression";
				case 1:
					return "X expression";
				}
				return null;
			}

			@Override
			public int getRowCount()
			{ return WaveInterface.sig_box.signals_name.size(); }

			@Override
			public Object getValueAt(int row, int col)
			{
				switch (col)
				{
				case 0:
					return WaveInterface.sig_box.getYexpr(row);
				case 1:
					return WaveInterface.sig_box.getXexpr(row);
				}
				return null;
			}
		};
		table = new JTable(dataModel);
		final JScrollPane scrollpane = new JScrollPane(table);
		getContentPane().add("Center", scrollpane);
		final JPanel p = new JPanel();
		p.setLayout(new FlowLayout(FlowLayout.CENTER));
		final JButton add = new JButton("Add");
		add.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				final int idx[] = table.getSelectedRows();
				final String x_expr[] = new String[idx.length];
				final String y_expr[] = new String[idx.length];
				for (int i = 0; i < idx.length; i++)
				{
					y_expr[i] = (String) table.getValueAt(idx[i], 0);
					x_expr[i] = (String) table.getValueAt(idx[i], 1);
				}
				scope.wave_panel.AddSignals(null, null, x_expr, y_expr, true, false);
			}
		});
		p.add(add);
		/*
		 * JButton remove = new JButton("Remove"); remove.addActionListener(new
		 * ActionListener() { public void actionPerformed(ActionEvent e) { int idx[] =
		 * table.getSelectedRows(); table.clearSelection();
		 *
		 * for(int i = 0; i < idx.length; i++) {
		 * WaveInterface.sig_box.removeExpr(idx[i]-i); } table.updateUI(); } });
		 * p.add(remove);
		 */
		final JButton cancel = new JButton("Cancel");
		cancel.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				dispose();
			}
		});
		p.add(cancel);
		getContentPane().add("South", p);
		pack();
		setLocationRelativeTo(f);
	}
}
