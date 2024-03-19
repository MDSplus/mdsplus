package mds.jtraverser.tools;

import java.util.StringTokenizer;
import java.util.Vector;

import javax.xml.parsers.*;

import org.w3c.dom.*;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;

import mds.MdsException;
import mds.data.TREE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.*;
import mds.mdsip.MdsIp;

public class CompileTree extends Thread
{
	public static void main(final String args[])
	{
		String filepath, provider, experiment;
		int shot = -1;
		if (args.length < 1)
		{
			System.err.println("Usage: java CompileTree <file> <provider> <experiment> [<shot>]");
			System.exit(0);
		}
		filepath = args[0];
		provider = args[1];
		experiment = args[2];
		if (args.length > 3)
			try
			{
				shot = Integer.parseInt(args[3]);
			}
			catch (final Exception exc)
			{
				System.err.println("Error Parsing shot number");
				System.exit(0);
			}
		(new CompileTree(filepath, provider, experiment, shot)).start();
	}

	String filepath, provider, experiment;
	Vector<String> newNames = new Vector<>();
	// originalNames and renamedNames keep info about nodes to be renamed
	Vector<String> renamedDevices = new Vector<>();
	Vector<String> renamedFieldNids = new Vector<>();
	int shot;
	Vector<Nid> subtreeNids = new Vector<>();
	TREE tree;
	Vector<String> unresolvedExprV = new Vector<>();
	Vector<Nid> unresolvedNidV = new Vector<>();

	CompileTree(final String filepath, final String provider, final String experiment, final int shot)
	{
		this.filepath = filepath;
		this.provider = provider;
		this.experiment = experiment;
		this.shot = shot;
	}

	void recCompile(final Element node)
	{
		final String type = node.getNodeName();
		String name = node.getAttribute("NAME");
		// final String state = node.getAttribute("STATE");
		final String usageStr = node.getAttribute("USAGE");
		Nid nid = null;
		try
		{
			final Nid parentNid = this.tree.getDefaultNid();
			if (type.equals("data"))
			{
				// final Element parentNode = (Element)node.getParentNode();
				final boolean isDeviceField = node.getNodeName().equals("field");
				final Text dataNode = (Text) node.getFirstChild();
				if (dataNode != null)
				{
					final String dataStr = dataNode.getData();
					final Descriptor<?> data = null;
					{
						this.unresolvedExprV.addElement(dataStr);
						this.unresolvedNidV.addElement(this.tree.getDefaultNid());
					}
					try
					{
						nid = this.tree.getDefaultNid();
						if (isDeviceField)
						{
							Descriptor<?> oldData;
							try
							{
								oldData = nid.getRecord();
							}
							catch (final Exception exc)
							{
								oldData = null;
							}
							if (oldData == null || !dataStr.equals(oldData.toString()))
								nid.putRecord(data);
						}
						else
							nid.putRecord(data);
					}
					catch (final Exception exc)
					{
						System.err.println("Error writing data to nid " + nid + ": " + exc);
					}
				}
				return;
			}
			// First handle renamed nodes: they do not need to be created, but
			// to be renamed
			final String originalDevice = node.getAttribute("DEVICE");
			final String deviceOffsetStr = node.getAttribute("OFFSET_NID");
			if (originalDevice != null && deviceOffsetStr != null && !originalDevice.equals("")
					&& !deviceOffsetStr.equals(""))
			{
				String newName;
				try
				{
					newName = parentNid.getNciFullPath();
				}
				catch (final Exception exc)
				{
					System.err.println("Error getting renamed path: " + exc);
					return;
				}
				if (type.equals("node"))
					newName += "." + name;
				else
					newName += ":" + name;
				this.newNames.addElement(newName);
				this.renamedDevices.addElement(originalDevice);
				this.renamedFieldNids.addElement(deviceOffsetStr);
				return; // No descendant for a renamed node
			}
			if (type.equals("node"))
				try
				{
					if (name.length() > 12)
						name = name.substring(0, 12);
					nid = this.tree.addNode(name, NODE.USAGE_STRUCTURE);
					if (usageStr != null && usageStr.equals("SUBTREE"))
						this.subtreeNids.addElement(nid);
				}
				catch (final Exception e)
				{
					System.err.println("Error adding member " + name + " : " + e);
					nid = null;
				}
			else if (type.equals("member"))
			{
				final byte usage;
				if (usageStr.equals("ACTION"))
					usage = NODE.USAGE_ACTION;
				else if (usageStr.equals("NUMERIC"))
					usage = NODE.USAGE_NUMERIC;
				else if (usageStr.equals("SIGNAL"))
					usage = NODE.USAGE_SIGNAL;
				else if (usageStr.equals("TASK"))
					usage = NODE.USAGE_TASK;
				else if (usageStr.equals("TEXT"))
					usage = NODE.USAGE_TEXT;
				else if (usageStr.equals("WINDOW"))
					usage = NODE.USAGE_WINDOW;
				else if (usageStr.equals("AXIS"))
					usage = NODE.USAGE_AXIS;
				else if (usageStr.equals("DISPATCH"))
					usage = NODE.USAGE_DISPATCH;
				else
					usage = NODE.USAGE_ANY;
				try
				{
					if (name.length() > 12)
						name = name.substring(0, 12);
					nid = this.tree.addNode(":" + name, usage);
				}
				catch (final Exception e)
				{
					System.err.println("Error adding member " + name + " : " + e);
					nid = null;
				}
			}
			else if (type.equals("device"))
			{
				final String model = node.getAttribute("MODEL");
				// final NodeInfo info = this.tree.getInfo(parentNid, 0);
				try
				{
					Thread.currentThread();
					Thread.sleep(100);
					nid = this.tree.addConglom(name.trim(), model);
				}
				catch (final MdsException exc)
				{
					/**/}
			}
			else if (type.equals("field"))
				try
				{
					nid = new Path(name).toNid();
				}
				catch (final Exception e)
				{
					System.err.println("WARNING: device field  " + name + " not found in model : " + e);
					nid = null;
				}
			if (nid != null)
			{
				nid.setDefault();
				// tags
				final String tagsStr = node.getAttribute("TAGS");
				if (tagsStr != null && tagsStr.length() > 0)
				{
					int i = 0;
					final StringTokenizer st = new StringTokenizer(tagsStr, ", ");
					final String[] tags = new String[st.countTokens()];
					while (st.hasMoreTokens())
						tags[i++] = st.nextToken();
					try
					{
						nid.setTags(tags);
					}
					catch (final Exception exc)
					{
						System.err.println("Error adding tags " + tagsStr + " : " + exc);
					}
				}
				// flags
				final String flagsStr = node.getAttribute("FLAGS");
				if (flagsStr != null && flagsStr.length() > 0)
				{
					int flags = 0;
					final StringTokenizer st = new StringTokenizer(flagsStr, ", ");
					while (st.hasMoreTokens())
					{
						final String flag = st.nextToken();
						if (flag.equals("WRITE_ONCE"))
							flags |= NODE.Flags.WRITE_ONCE;
						if (flag.equals("COMPRESSIBLE"))
							flags |= NODE.Flags.COMPRESSIBLE;
						if (flag.equals("COMPRESS_ON_PUT"))
							flags |= NODE.Flags.COMPRESS_ON_PUT;
						if (flag.equals("NO_WRITE_MODEL"))
							flags |= NODE.Flags.NO_WRITE_MODEL;
						if (flag.equals("NO_WRITE_SHOT"))
							flags |= NODE.Flags.NO_WRITE_SHOT;
					}
					try
					{
						nid.setFlags(flags);
					}
					catch (final Exception e)
					{
						System.err.println("Error setting flags to node " + name + " : " + e);
					}
				}
				// state
				final String stateStr = node.getAttribute("STATE");
				if (stateStr != null && stateStr.length() > 0)
					try
					{
						if (stateStr.equals("ON"))
							nid.setOn(true);
						if (stateStr.equals("OFF"))
							nid.setOn(false);
					}
					catch (final Exception e)
					{
						System.err.println("Error setting state of node " + name + " : " + e);
					}
				// Descend
				final NodeList nodes = node.getChildNodes();
				for (int i = 0; i < nodes.getLength(); i++)
				{
					final Node currNode = nodes.item(i);
					if (currNode.getNodeType() == Node.ELEMENT_NODE) // Only
																		// element
																		// nodes
																		// at
																		// this
						this.recCompile((Element) currNode);
				}
			}
			parentNid.setDefault();
		}
		catch (final Exception e)
		{
			System.err.println("Internal error in recCompile: " + e);
			e.printStackTrace();
		}
	}

	@SuppressWarnings("resource")
	@Override
	public void run()
	{
		try
		{
			this.tree = new TREE(new MdsIp(this.provider), this.experiment, this.shot, TREE.NEW);
		}
		catch (final Exception e)
		{
			System.err.println("Error opening tree " + this.experiment + " : " + e);
			System.exit(0);
		}
		final DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		try
		{
			final DocumentBuilder builder = factory.newDocumentBuilder();
			final Document document = builder.parse(this.filepath);
			final Element rootNode = document.getDocumentElement();
			final NodeList nodes = rootNode.getChildNodes();
			for (int i = 0; i < nodes.getLength(); i++)
			{
				final Node currNode = nodes.item(i);
				if (currNode.getNodeType() == Node.ELEMENT_NODE) // Only element
																	// nodes at
																	// this
					this.recCompile((Element) currNode);
			}
		}
		catch (final SAXParseException e)
		{ // Error generated by the parser
			System.err.println("\n** Parsing error" + ", line " + e.getLineNumber() + ", uri " + e.getSystemId());
			System.err.println("   " + e.getMessage());
			final Exception x = e.getException();
			if (x != null)
				x.printStackTrace();
			else
				e.printStackTrace();
		}
		catch (final SAXException se)
		{ // Error generated during parsing
			final Exception e = se.getException();
			if (e != null)
				e.printStackTrace();
			else
				se.printStackTrace();
		}
		catch (final ParserConfigurationException e)
		{ // Parser with specified options can't be built
			e.printStackTrace();
		}
		catch (final Exception e)
		{ // I/O error
			e.printStackTrace();
		}
		// handle renamed nodes
		for (int i = 0; i < this.newNames.size(); i++)
		{
			final String newName = this.newNames.elementAt(i);
			final String deviceName = this.renamedDevices.elementAt(i);
			final String offsetStr = this.renamedFieldNids.elementAt(i);
			try
			{
				final int deviceOffset = Integer.parseInt(offsetStr);
				final Nid deviceNid = new Path(deviceName).toNid();
				final Nid renamedNid = new Nid(deviceNid, +deviceOffset);
				renamedNid.setPath(newName);
			}
			catch (final Exception e)
			{
				System.err.println("Error renaming node of " + deviceName + " to " + newName + " : " + e);
			}
		}
		for (int i = 0; i < this.unresolvedNidV.size(); i++)
		{
			Descriptor<?> data = null;
			try
			{
				this.unresolvedNidV.elementAt(i).setDefault();
				data = this.tree.getMds().getDescriptor(this.unresolvedExprV.elementAt(i));
			}
			catch (final Exception e)
			{
				System.err.println("Error parsing expression " + this.unresolvedExprV.elementAt(i) + " : " + e);
			}
			try
			{
				this.unresolvedNidV.elementAt(i).putRecord(data);
			}
			catch (final Exception e)
			{
				System.err.println("Error writing data to nid " + this.unresolvedNidV.elementAt(i) + ": " + e);
			}
		}
		// Set subtrees (apparently this must be done at the end....
		for (int i = 0; i < this.subtreeNids.size(); i++)
			try
			{
				this.subtreeNids.elementAt(i).setSubtree();
			}
			catch (final Exception e)
			{
				System.err.println("Error setting subtree: " + e);
			}
		try
		{
			this.tree.writeTree();
			this.tree.close();
		}
		catch (final Exception e)
		{
			System.err.println("Error closeing tree: " + e);
		}
	}
}
