package mds.jtraverser.editor;

import java.awt.*;
import java.awt.datatransfer.StringSelection;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.text.*;

import mds.Mds;
import mds.MdsException;
import mds.data.CTX;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;
import mds.data.descriptor_r.Signal;
import mds.data.descriptor_s.NODE;
import mds.data.descriptor_s.NUMBER;
import mds.jtraverser.dialogs.DataDialog;
import mds.jtraverser.dialogs.GraphPanel;

public abstract class Editor extends JPanel
{
	protected final class EvalPopupMenu extends JPopupMenu implements MouseListener
	{
		public final class DisplaySignal implements ActionListener
		{
			@Override
			public final void actionPerformed(final ActionEvent ae)
			{
				try
				{
					final Descriptor<?> local;
					if (Descriptor.isMissing(Editor.this.value))
						local = Editor.this.getData().getLocal();
					else
						local = new Signal(Editor.this.getData(), Editor.this.value.getLocal(), null).getLocal();
					GraphPanel.newPlot(local.toFloatArray(), null, JOptionPane.getRootFrame(), Editor.this.getName())
							.setVisible(true);
				}
				catch (final Exception e)
				{
					e.printStackTrace();
				}
			}
		}

		protected class EvalActionListener implements ActionListener
		{
			private final String exec;

			EvalActionListener(final Editor editor, final String exec)
			{
				this.exec = exec;
			}

			@Override
			public void actionPerformed(final ActionEvent e)
			{
				String aeeval;
				if (this.exec == null)
					aeeval = MdsException.getMdsMessage(MdsException.TdiINVCLADSC);
				else
					try
					{
						final Descriptor<?> aedata = Editor.this.getMds().getDescriptor(Editor.this.ctx, this.exec,
								Editor.this.getData());
						if (Descriptor.isMissing(aedata))
							aeeval = "*";
						else
							aeeval = aedata.toString();
					}
					catch (final MdsException de)
					{
						aeeval = de.getMessage();
					}
				Toolkit.getDefaultToolkit().getSystemClipboard().setContents(new StringSelection(aeeval), null);
				JOptionPane.showMessageDialog(Editor.this,
						"<html><body><p style='width: 360px;'>" + EvalPopupMenu.this.escapeHTML(aeeval)
								+ "</p></body></html>",
						"Evaluated Data (also copied to Clipboard)", JOptionPane.PLAIN_MESSAGE);
			}
		}

		private static final long serialVersionUID = 1L;
		final POPUP_MODE mode;

		public EvalPopupMenu(final POPUP_MODE mode)
		{
			this.mode = mode;
			if (mode == POPUP_MODE.CONTEXT)
			{
				Editor.addTextEditorActions(this, Editor.this.editable);
				this.addSeparator();
			}
			JMenuItem item;
			this.add(item = new JMenuItem("DATA($)"));
			item.addActionListener(new EvalActionListener(Editor.this, "DATA($)"));
			this.add(item = new JMenuItem("RAW_OF($)"));
			item.addActionListener(new EvalActionListener(Editor.this, "RAW_OF($)"));
			this.add(item = new JMenuItem("DIM_OF($)"));
			item.addActionListener(new EvalActionListener(Editor.this, "DIM_OF($)"));
			this.add(item = new JMenuItem("Plot DATA($)"));
			item.addActionListener(new DisplaySignal());
		}

		String escapeHTML(final String s)
		{
			final StringBuilder out = new StringBuilder(Math.max(16, s.length()));
			for (int i = 0; i < s.length(); i++)
			{
				final char c = s.charAt(i);
				if (c > 127 || c == '"' || c == '<' || c == '>' || c == '&')
				{
					out.append("&#");
					out.append((int) c);
					out.append(';');
				}
				else
					out.append(c);
			}
			return out.toString();
		}

		@Override
		public void mouseClicked(final MouseEvent e)
		{
			if (POPUP_MODE.CLICK == this.mode)
				EvalPopupMenu.this.show(e.getComponent(), e.getX(), e.getY());
		}

		@Override
		public void mouseEntered(final MouseEvent e)
		{
			/* stub */}

		@Override
		public void mouseExited(final MouseEvent e)
		{
			/* stub */}

		@Override
		public void mousePressed(final MouseEvent e)
		{
			if (POPUP_MODE.CONTEXT == this.mode && e.isPopupTrigger())
				EvalPopupMenu.this.show(e.getComponent(), e.getX(), e.getY());
		}

		@Override
		public void mouseReleased(final MouseEvent e)
		{
			this.mousePressed(e);
		}
	}

	public static enum POPUP_MODE
	{
		CONTEXT, CLICK;
	}

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

	private static final long serialVersionUID = 1L;

	public static JPanel addButtons(final Editor editor, final NODE<?> node)
	{
		final JPanel panel = new JPanel(new BorderLayout());
		panel.add(editor, BorderLayout.CENTER);
		final Insets insets = new Insets(-3, 3, -3, 3);
		final JPanel buttons = new JPanel(new GridLayout(0, 1));
		if (editor.editable)
		{
			final JButton apply = new JButton("A");
			apply.setMargin(insets);
			apply.setToolTipText("apply");
			apply.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(final ActionEvent ae)
				{
					try
					{
						final Descriptor<?> newdata = editor.getData();
						node.putRecord(newdata);
						editor.setData(newdata);
					}
					catch (final MdsException e)
					{
						JOptionPane.showMessageDialog(editor, e.getMessage(), "Apply", JOptionPane.WARNING_MESSAGE);
					}
				}
			});
			final JButton reset = new JButton("R");
			reset.setMargin(insets);
			reset.setToolTipText("reset");
			reset.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(final ActionEvent ae)
				{
					try
					{
						editor.setData(node.getRecord());
					}
					catch (final MdsException e)
					{
						editor.reset(false);
					}
				}
			});
			buttons.add(apply);
			buttons.add(reset);
		}
		final JButton openedit = new JButton("E");
		openedit.setMargin(insets);
		openedit.setToolTipText("editor");
		openedit.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(final ActionEvent ae)
			{
				try
				{
					final DataDialog dialog = DataDialog.open(node, editor.editable);
					dialog.addUpdateListener(new DataDialog.UpdateListener()
					{
						@Override
						public void update(final Descriptor<?> data)
						{
							editor.setData(data);
						}
					});
				}
				catch (final Exception e)
				{
					JOptionPane.showMessageDialog(editor, e.getMessage(), "Open Editor", JOptionPane.WARNING_MESSAGE);
				}
			}
		});
		buttons.add(openedit);
		panel.add(buttons, BorderLayout.LINE_END);
		return panel;
	}

	public static final <T extends JComponent> T addLabel(final String label, final T editor)
	{
		((JComponent) editor).setBorder(BorderFactory.createTitledBorder(label));
		return editor;
	}

	static public JPopupMenu addTextEditorActions(final JPopupMenu menu, final boolean editable)
	{
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

	protected static final boolean isNoData(final Descriptor<?> data)
	{
		return data == null || data.dtype() == DTYPE.Z;
	}

	static public JPopupMenu newTextEditorPopup(final boolean editable)
	{
		return Editor.addTextEditorActions(new JPopupMenu(), editable);
	}

	protected final boolean editable;
	protected final Editor edit[];
	protected Descriptor<?> data;
	protected CTX ctx;
	private Descriptor<?> value;

	protected Editor(final Descriptor<?> data, final boolean editable, final CTX ctx, final int nargs)
	{
		this.data = data;
		this.editable = editable;
		this.edit = new Editor[nargs];
		this.ctx = ctx;
	}

	public abstract Descriptor<?> getData() throws MdsException;

	protected Mds getMds()
	{
		final Mds mds = this.ctx.getMds();
		if (mds != null)
			return mds;
		return Mds.getActiveMds();
	}

	protected NUMBER<?> getNumber() throws MdsException
	{
		final Descriptor<?> num = this.getData().getDataD();
		if (num instanceof NUMBER<?>)
			return (NUMBER<?>) num;
		throw new MdsException(MdsException.TdiNOT_NUMBER);
	}

	public void interrupt()
	{
		/* stub */}

	@SuppressWarnings("static-method")
	public boolean isNull()
	{
		return false;
	}

	public void reset(final boolean hard)
	{
		for (final Component component : this.getComponents())
			if (component instanceof Editor)
				((Editor) component).reset(hard);
	}

	public void setData(final Descriptor<?> data)
	{
		this.data = data;
		this.reset(false);
	}

	protected final void setDescR()
	{
		if (this.data instanceof Descriptor_R<?>)
		{
			final Descriptor_R<?> dsc_r = (Descriptor_R<?>) this.data;
			for (int i = 0; i < this.edit.length; i++)
				this.edit[i].setData(dsc_r.getDescriptor(i));
		}
		else
			for (final Editor element : this.edit)
				element.setData(null);
	}

	/*
	 * setValue(Descriptor<?> value) sets $VALUE PTR for evaluation
	 */
	public final void setValue(final Descriptor<?> value)
	{
		this.value = value;
	}
}
