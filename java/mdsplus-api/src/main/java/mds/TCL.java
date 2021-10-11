package mds;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.event.*;
import java.util.Vector;

import javax.swing.*;
import javax.swing.text.*;

import mds.data.CTX;
import mds.mdsip.MdsIp;

public final class TCL
{
	private static abstract class Listener extends KeyAdapter implements ActionListener
	{
		/**/}

	private static class SelectAll extends TextAction
	{
		private static final long serialVersionUID = 1L;

		public SelectAll()
		{
			super("Select All");
			this.putValue(Action.ACCELERATOR_KEY, KeyStroke.getKeyStroke("control A"));
		}

		@Override
		public void actionPerformed(final ActionEvent e)
		{
			final JPopupMenu menu = (JPopupMenu) ((Component) e.getSource()).getParent();
			final JTextComponent component = (JTextComponent) menu.getInvoker();
			component.requestFocusInWindow();
			component.selectAll();
		}
	}

	final static Vector<String> hist = new Vector<>();

	public final static JPanel getComponent(final Mds mds)
	{
		final TCL tcl = mds.getTCL();
		final JPanel panel = new JPanel(new BorderLayout());
		final JTextField tf = new JTextField(48);
		tf.setComponentPopupMenu(TCL.newTextEditorPopup(true));
		final JTextArea ta = new JTextArea(16, 48);
		ta.setComponentPopupMenu(TCL.newTextEditorPopup(false));
		ta.setEditable(false);
		panel.add(tf, BorderLayout.SOUTH);
		final JScrollPane sp = new JScrollPane(ta);
		panel.add(sp, BorderLayout.CENTER);
		tf.requestFocus();
		final Listener listen = new Listener()
		{
			int pos = 0;

			@Override
			public void actionPerformed(final ActionEvent e)
			{
				final String command = e.getActionCommand();
				ta.append(command);
				ta.append("\n");
				final String result = tcl.execute(command);
				if (result != null && result.length() > 0)
				{
					ta.append(result);
					ta.append("\n");
				}
				tf.setText("");
				TCL.hist.remove(command);
				TCL.hist.add(command);
				this.pos = 0;
			}

			@Override
			public void keyPressed(final KeyEvent evt)
			{
				final int keyCode = evt.getKeyCode();
				if (keyCode == KeyEvent.VK_UP || keyCode == KeyEvent.VK_DOWN)
				{
					evt.consume();
					if (TCL.hist.size() == 0)
						return;
					tf.setText(TCL.hist.get(TCL.hist.size() - 1 - this.pos));
					if (keyCode == KeyEvent.VK_UP)
					{
						if (this.pos < TCL.hist.size() - 1)
							this.pos++;
						else
							tf.selectAll();
					}
					else if (this.pos > 0)
						this.pos--;
					else
						tf.selectAll();
				}
			}
		};
		tf.addActionListener(listen);
		tf.addKeyListener(listen);
		return panel;
	}

	public static final JFrame getPrompt(final Mds mds)
	{
		final JFrame d = new JFrame();
		d.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
		d.setContentPane(TCL.getComponent(mds));
		d.setTitle("MdsTCL @ " + mds.toString());
		d.pack();
		return d;
	}

	public static void main(final String... args)
	{
		try (MdsIp mdsip = new MdsIp())
		{
			TCL.getPrompt(mdsip).setVisible(true);
		}
	}

	static public JPopupMenu newTextEditorPopup(final boolean editable)
	{
		final JPopupMenu menu = new JPopupMenu();
		if (editable)
		{
			final Action cut = new DefaultEditorKit.CutAction();
			cut.putValue(Action.NAME, "Cut");
			cut.putValue(Action.ACCELERATOR_KEY, KeyStroke.getKeyStroke("control X"));
			menu.add(cut);
		}
		final Action copy = new DefaultEditorKit.CopyAction();
		copy.putValue(Action.NAME, "Copy");
		copy.putValue(Action.ACCELERATOR_KEY, KeyStroke.getKeyStroke("control C"));
		menu.add(copy);
		if (editable)
		{
			final Action paste = new DefaultEditorKit.PasteAction();
			paste.putValue(Action.NAME, "Paste");
			paste.putValue(Action.ACCELERATOR_KEY, KeyStroke.getKeyStroke("control V"));
			menu.add(paste);
		}
		menu.add(new SelectAll());
		return menu;
	}

	private final Mds mds;
	boolean setcommandtcl = false;

	public TCL(final Mds mds)
	{
		this.mds = mds;
	}

	public final int doAction(final CTX ctx, final int nid) throws MdsException
	{
		return this.mds.getInteger(ctx,
				new StringBuilder(37).append("TCL('do '//GetNci(").append(nid).append(",'PATH'))").toString());
	}

	public final String doMethod(final String devicepath, final String method, final String arg, final boolean override)
	{
		final StringBuilder cmd = new StringBuilder(128).append("DO/METHOD ");
		cmd.append(devicepath).append(' ').append(method);
		if (arg != null)
			cmd.append(" /ARG=\"").append(arg.replace("\"", "\"\"")).append("\"");
		if (override)
			cmd.append(" /OVERRIDE");
		return this.execute(cmd.toString());
	}

	public String execute(final String command)
	{
		try
		{
			if (!this.setcommandtcl)
			{
				this.mds.dcl(null, "set command tcl");
				this.setcommandtcl = true;
			}
			return this.mds.dcl(null, command);
		}
		catch (final MdsException e)
		{
			return e.toString();
		}
	}

	public final String showAttribute()
	{
		return this.execute("show a");
	}

	public final String showCurrent(final String expt)
	{
		return this.execute("show c " + expt);
	}

	public final String showData(final String nodepath)
	{
		return this.execute("show d " + nodepath);
	}

	public final String showDatabase()
	{
		return this.execute("show db");
	}

	public final String showDefault()
	{
		return this.execute("show def");
	}

	public final String showVersion()
	{
		return this.execute("show v");
	}
}
