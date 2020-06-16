package mds.jscope;

import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.net.*;
import java.util.StringTokenizer;
import java.util.Vector;

import javax.swing.*;
import javax.swing.event.HyperlinkEvent;
import javax.swing.event.HyperlinkListener;
import javax.swing.text.html.HTMLDocument;
import javax.swing.text.html.HTMLFrameHyperlinkEvent;

public class jScopeBrowseUrl extends JDialog
{
	private static final long serialVersionUID = 1L;
	private static final String USER_AGENT;
	static
	{
		final String version = jScopeBrowseUrl.class.getPackage().getImplementationVersion();
		USER_AGENT = "jScopeBrowseUrl.java ($Revision$) for " + (version == null ? "unknown" : version);
	}
	protected JEditorPane html;
	URLConnection url_con;
	protected String mime_type;
	Vector<URL> url_list = new Vector<>();
	JButton back;
	JButton forward;
	JButton home;
	int curr_url = 0;
	JPanel p;
	boolean connected = false;

	public jScopeBrowseUrl(JFrame owner)
	{
		super(owner);
		html = new JEditorPane();
		html.setEditable(false);
		html.addHyperlinkListener(createHyperLinkListener());
		final JScrollPane scroller = new JScrollPane();
		final JViewport vp = scroller.getViewport();
		vp.add(html);
		getContentPane().add(scroller, BorderLayout.CENTER);
		p = new JPanel();
		back = new JButton("Back");
		back.setSelected(true);
		p.add(back);
		back.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				if (curr_url - 1 >= 0)
				{
					try
					{
						curr_url--;
						// html.setPage(url_list.elementAt(curr_url));
						setPage(url_list.elementAt(curr_url));
					}
					catch (final IOException ioe)
					{
						System.out.println("IOE: " + ioe);
					}
				}
			}
		});
		forward = new JButton("Forward");
		back.setSelected(true);
		p.add(forward);
		forward.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				if (curr_url + 1 < url_list.size())
				{
					try
					{
						curr_url++;
						html.setPage(url_list.elementAt(curr_url));
					}
					catch (final IOException ioe)
					{
						System.out.println("IOE: " + ioe);
					}
				}
			}
		});
		home = new JButton("Home");
		home.setSelected(true);
		p.add(home);
		home.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				if (url_list.size() != 0)
				{
					try
					{
						curr_url = 0;
						html.setPage(url_list.elementAt(0));
					}
					catch (final IOException ioe)
					{
						System.out.println("IOE: " + ioe);
					}
				}
			}
		});
		getContentPane().add(p, BorderLayout.NORTH);
		pack();
		setSize(680, 700);
	}

	public void connectToBrowser(String url_path) throws Exception
	{
		try
		{
			URL url = null;
			url = new URL(url_path);
			connectToBrowser(url);
			connected = true;
		}
		catch (final Exception e)
		{
			connected = false;
			throw (new IOException("Unable to locate the signal server " + url_path + " : " + e.getMessage()));
		}
	}

	public void connectToBrowser(URL url) throws Exception
	{
		if (url != null)
		{
			url_list.addElement(url);
			setPage(url);
		}
	}

	public HyperlinkListener createHyperLinkListener()
	{
		return new HyperlinkListener()
		{
			@Override
			public void hyperlinkUpdate(HyperlinkEvent e)
			{
				if (e.getEventType() == HyperlinkEvent.EventType.ACTIVATED)
				{
					if (e instanceof HTMLFrameHyperlinkEvent)
					{
						((HTMLDocument) html.getDocument()).processHTMLFrameHyperlinkEvent((HTMLFrameHyperlinkEvent) e);
					}
					else
					{
						try
						{
							URL u = e.getURL();
							// To fix JVM 1.1 Bug
							if (u == null)
							{
								final HTMLDocument hdoc = (HTMLDocument) html.getDocument();
								try
								{
									final StringTokenizer st = new StringTokenizer(hdoc.getBase().toString(), "/");
									final int num_token = st.countTokens();
									String base = st.nextToken() + "//";
									for (int i = 0; i < num_token - 2; i++)
										base = base + st.nextToken() + "/";
									if (jScopeFacade.is_debug)
										System.out.println("JDK1.1 url = " + base + e.getDescription());
									u = new URL(base + e.getDescription());
								}
								catch (final MalformedURLException m)
								{
									u = null;
								}
							}
							// end fix bug JVM 1.1
							// html.setPage(u);
							setPage(u);
							final int sz = url_list.size();
							for (int i = curr_url + 1; i < sz; i++)
								url_list.removeElementAt(curr_url + 1);
							url_list.addElement(u);
							curr_url++;
						}
						catch (final IOException ioe)
						{
							JOptionPane.showMessageDialog(jScopeBrowseUrl.this, "IOE: " + ioe, "alert",
									JOptionPane.ERROR_MESSAGE);
						}
					}
				}
			}
		};
	}

	public boolean isConnected()
	{ return connected; }

	protected void setPage(URL url) throws IOException
	{
		url_con = url.openConnection();
		url_con.setRequestProperty("User-Agent", USER_AGENT);
		mime_type = url_con.getContentType();
		// Assume (like browsers) that missing mime-type indicates text/html.
		if (mime_type == null || mime_type.indexOf("text") != -1)
			html.setPage(url);
		else
		{
			final String path = "TWU_image_message.html";
			final URL u = getClass().getClassLoader().getResource(path);
			html.setPage(u);
		}
	}
}
