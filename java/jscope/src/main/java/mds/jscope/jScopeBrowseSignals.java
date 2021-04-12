package mds.jscope;

import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.net.URL;

/* $Id$ */
import javax.swing.JButton;
import javax.swing.JFrame;

public abstract class jScopeBrowseSignals extends jScopeBrowseUrl
{
	private static final long serialVersionUID = 2L;
	jScopeWaveContainer wave_panel;
	JButton add_sig;
	JButton add_sig_shot;
	String prev_type = new String("text");

	public jScopeBrowseSignals()
	{
		this(null);
	}

	public jScopeBrowseSignals(JFrame owner)
	{
		super(owner);
		add_sig = new JButton("Add signal");
		add_sig.setSelected(true);
		p.add(add_sig);
		add_sig.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				addSignal(false);
			}
		});
		add_sig_shot = new JButton("Add signal & shot");
		add_sig_shot.setSelected(true);
		p.add(add_sig_shot);
		add_sig_shot.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				addSignal(true);
			}
		});
		pack();
		setSize(p.getPreferredSize().width + 50, Toolkit.getDefaultToolkit().getScreenSize().height - 80);
	}

	public void addSignal(boolean with_shot)
	{
		if (wave_panel != null)
		{
			final URL u = url_list.elementAt(curr_url);
			final String url_name = (u.toString());
			final String sig_path = getSignal(url_name);
			final String shot = (with_shot) ? getShot() : null;
			if (sig_path != null)
			{
				final boolean is_image = (mime_type != null && mime_type.indexOf("image") != -1);
				wave_panel.AddSignal(getTree(), shot, "", sig_path, true, is_image);
			}
		}
	}

	abstract protected String getServerAddr();

	abstract protected String getShot();

	abstract protected String getSignal(String url);

	abstract protected String getTree();

	@Override
	protected void setPage(URL url) throws IOException
	{
		super.setPage(url);
		final boolean equal = (prev_type == null) ? (mime_type == null)
				: (mime_type != null && prev_type.equals(mime_type));
		if (equal)
			return;
		prev_type = mime_type;
		// Assume (like browsers) that missing mime-type indicates text/html.
		if (mime_type == null || mime_type.indexOf("text") != -1)
		{
			add_sig.setText("Add signal");
			add_sig.setEnabled(true);
			add_sig_shot.setText("Add signal & shot");
		}
		else
		{
			add_sig.setText("Add frames");
			add_sig.setEnabled(false);
			add_sig_shot.setText("Add frames & shot");
		}
	}

	public void setWaveContainer(jScopeWaveContainer wave_panel)
	{ this.wave_panel = wave_panel; }
}
