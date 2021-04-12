package mds.jtraverser.editor;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JPanel;
import mds.MdsException;
import mds.data.CTX;
import mds.data.descriptor.ARRAY;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.Int32;
import mds.data.descriptor_s.StringDsc;

public class EnumEditor extends Editor
{
	public enum MODE
	{
		INDEX, STRING, ASIS;
	}

	private static final long serialVersionUID = 1L;
	private final JComboBox<String> combo;
	private String expr;
	private final JButton left, right;
	private final MODE mode;
	private boolean quotes_added;
	private final String items[];
	private final Color defaultbg, defaultfg;

	public EnumEditor(final Descriptor<?> data, final boolean editable, final CTX ctx, final String tooltip,
			final MODE mode, final String... items)
	{
		super(data, editable, ctx, 0);
		this.mode = mode;
		this.items = items;
		this.setLayout(new BorderLayout());
		this.combo = new JComboBox<>();
		this.combo.setEditable(editable);
		this.defaultbg = this.combo.getBackground();
		this.defaultfg = this.combo.getForeground();
		for (final String item : items)
			this.combo.addItem(item);
		final JPanel panel = new JPanel(new BorderLayout());
		panel.add(this.combo, BorderLayout.CENTER);
		final JButton menu = new JButton("-");
		menu.setMargin(new Insets(0, 3, 0, 3));
		menu.addMouseListener(new Editor.EvalPopupMenu(Editor.POPUP_MODE.CLICK));
		panel.add(menu, BorderLayout.EAST);
		this.add(panel, BorderLayout.CENTER);
		this.left = new JButton("\"");
		this.right = new JButton("\"");
		final Insets insets = new Insets(0, 0, 0, 0);
		this.left.setMargin(insets);
		this.right.setMargin(insets);
		if (this.editable)
		{
			final ActionListener leftright = new ActionListener()
			{
				@Override
				public void actionPerformed(final ActionEvent e)
				{
					EnumEditor.this.quotes_added = false;
					EnumEditor.this.remove(EnumEditor.this.left);
					EnumEditor.this.remove(EnumEditor.this.right);
					EnumEditor.this.expr = (String) EnumEditor.this.combo.getSelectedItem();
					EnumEditor.this.expr = StringDsc.addQuotes(EnumEditor.this.expr);
					final String itms[] = new String[EnumEditor.this.combo.getItemCount()];
					for (int i = 0; i < itms.length; i++)
						itms[i] = StringDsc.addQuotes(EnumEditor.this.combo.getItemAt(i));
					EnumEditor.this.combo.removeAllItems();
					for (final String item : itms)
						EnumEditor.this.combo.addItem(item);
					EnumEditor.this.combo.setSelectedItem(EnumEditor.this.expr);
					EnumEditor.this.validate();
					EnumEditor.this.repaint();
				}
			};
			this.left.addActionListener(leftright);
			this.right.addActionListener(leftright);
		}
		if (tooltip != null)
		{
			this.setToolTipText(tooltip);
			this.combo.setToolTipText(tooltip);
		}
		this.setData(data);
	}

	@Override
	public final Descriptor<?> getData() throws MdsException
	{
		if (this.mode == MODE.INDEX)
		{
			final int index = this.combo.getSelectedIndex();
			if (index >= 0)
				return new Int32(index);
		}
		this.expr = ((String) this.combo.getSelectedItem()).trim();
		if (this.expr.isEmpty())
			return null;
		if (this.quotes_added)
			return new StringDsc(this.expr);
		return this.getMds().getAPI().tdiCompile(this.ctx, this.expr).getData();
	}

	@Override
	public final void reset(final boolean hard)
	{
		if (this.mode == MODE.INDEX && this.data instanceof Int32)
		{
			final int index = this.data.toInt();
			this.expr = this.combo.getItemAt(index);
			if (this.expr == null)
			{
				this.expr = Integer.toString(index);
				this.combo.setSelectedItem(this.expr);
			}
			else
				this.combo.setSelectedIndex(index);
			this.quotes_added = false;
		}
		else if (this.mode == MODE.STRING && this.data instanceof StringDsc)
		{
			this.expr = this.data.toString();
			this.quotes_added = this.expr.trim().length() == this.expr.length();
			if (!this.quotes_added)
				this.expr = this.data.decompile();
			this.updateItems();
		}
		else if (Editor.isNoData(this.data))
		{
			this.expr = null;
			this.quotes_added = this.mode == MODE.STRING;
			this.updateItems();
		}
		else
		{
			this.quotes_added = false;
			this.expr = this.data.decompile();
			this.updateItems();
		}
		if (this.expr != null && this.expr.contains(ARRAY.ETC))
		{
			this.combo.setBackground(Color.PINK);
			this.combo.setForeground(Color.RED);
		}
		else
		{
			this.combo.setBackground(this.defaultbg);
			this.combo.setForeground(this.defaultfg);
		}
		this.validate();
		this.repaint();
	}

	@Override
	public final void setData(final Descriptor<?> data)
	{
		this.data = data;
		this.reset(false);
	}

	private void updateItems()
	{
		if (this.mode == MODE.STRING)
		{
			this.combo.removeAllItems();
			if (this.quotes_added)
			{
				this.add(this.left, BorderLayout.LINE_START);
				this.add(this.right, BorderLayout.LINE_END);
				for (final String item : this.items)
					this.combo.addItem(item);
			}
			else
			{
				this.remove(this.left);
				this.remove(this.right);
				for (final String item : this.items)
					this.combo.addItem(StringDsc.addQuotes(item));
			}
		}
		this.combo.setSelectedItem(this.expr);
	}
}
