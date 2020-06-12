package mds.jtraverser.editor;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JButton;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.text.JTextComponent;
import mds.MdsException;
import mds.data.CTX;
import mds.data.descriptor.ARRAY;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.StringDsc;

public class ExprEditor extends Editor
{
	private static final long serialVersionUID = 1L;
	private final boolean default_to_string;
	private final JButton left, right;
	private final JTextComponent text_edit;
	private final JScrollPane scroll_pane;
	private String expr;
	private boolean quotes_added;
	private final Color defaultbg, defaultfg;

	public ExprEditor(final boolean editable, final CTX ctx, final boolean default_to_string, final boolean asField)
	{
		this((Descriptor<?>) null, editable, ctx, null, default_to_string, asField);
	}

	public ExprEditor(final boolean editable, final CTX ctx, final String tooltip, final boolean default_to_string,
			final boolean asField)
	{
		this(null, editable, ctx, tooltip, default_to_string, asField);
	}

	public ExprEditor(final Descriptor<?> data, final boolean editable, final CTX ctx)
	{
		this(data, editable, ctx, null, (data != null && data instanceof StringDsc), true);
	}

	public ExprEditor(final Descriptor<?> data, final boolean editable, final CTX ctx, final String tooltip,
			final boolean default_to_string, final boolean isField)
	{
		super(data, editable, ctx, 0);
		this.default_to_string = default_to_string;
		this.setLayout(new BorderLayout());
		if (isField)
		{// default_scroll
			this.text_edit = new JTextArea(7, 24);
			this.text_edit.setEditable(this.editable);
			this.text_edit.addMouseListener(new Editor.EvalPopupMenu(Editor.POPUP_MODE.CONTEXT));
			((JTextArea) this.text_edit).setTabSize(2);
			((JTextArea) this.text_edit).setLineWrap(true);
			final Dimension d = this.text_edit.getPreferredSize();
			d.height += 20;
			d.width += 20;
			this.scroll_pane = new JScrollPane(this.text_edit);
			this.scroll_pane.setPreferredSize(d);
			this.add(this.scroll_pane, BorderLayout.CENTER);
		}
		else
		{
			this.scroll_pane = null;
			this.text_edit = new JTextField(24);
			this.text_edit.setEditable(this.editable);
			this.text_edit.addMouseListener(new Editor.EvalPopupMenu(Editor.POPUP_MODE.CONTEXT));
			this.add(this.text_edit, BorderLayout.CENTER);
		}
		this.defaultbg = this.text_edit.getBackground();
		this.defaultfg = this.text_edit.getForeground();
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
					ExprEditor.this.remove(ExprEditor.this.left);
					ExprEditor.this.remove(ExprEditor.this.right);
					ExprEditor.this.quotes_added = false;
					ExprEditor.this.expr = ExprEditor.this.text_edit.getText();
					ExprEditor.this.expr = StringDsc.addQuotes(ExprEditor.this.expr);
					ExprEditor.this.text_edit.setText(ExprEditor.this.expr);
					ExprEditor.this.validate();
					ExprEditor.this.repaint();
				}
			};
			this.left.addActionListener(leftright);
			this.right.addActionListener(leftright);
		}
		if (tooltip != null)
		{
			this.setToolTipText(tooltip);
			this.text_edit.setToolTipText(tooltip);
		}
		this.setData(data);
	}

	@Override
	public final Descriptor<?> getData() throws MdsException
	{
		this.expr = this.text_edit.getText().trim();
		if (this.expr.isEmpty())
			return null;
		if (this.quotes_added)
			return new StringDsc(this.expr);
		return this.getMds().getAPI().tdiCompile(this.ctx, this.expr).getData();
	}

	@Override
	public final void reset(final boolean hard)
	{
		if (this.data instanceof StringDsc)
		{
			this.expr = this.data.toString();
			this.quotes_added = this.expr.trim().length() == this.expr.length();
			if (!this.quotes_added)
				this.expr = this.data.decompile();
		}
		else if (Editor.isNoData(this.data))
		{
			this.expr = null;
			this.quotes_added = this.default_to_string;
		}
		else
		{
			this.quotes_added = false;
			this.expr = (this.scroll_pane == null) ? this.data.decompile() : this.data.decompileX();
		}
		this.text_edit.setText(this.expr);
		if (this.quotes_added)
		{
			this.add(this.left, BorderLayout.LINE_START);
			this.add(this.right, BorderLayout.LINE_END);
		}
		else
		{
			this.remove(this.left);
			this.remove(this.right);
		}
		if (this.expr != null && this.expr.contains(ARRAY.ETC))
		{
			this.text_edit.setBackground(Color.PINK);
			this.text_edit.setForeground(Color.RED);
		}
		else
		{
			this.text_edit.setBackground(this.defaultbg);
			this.text_edit.setForeground(this.defaultfg);
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
}
