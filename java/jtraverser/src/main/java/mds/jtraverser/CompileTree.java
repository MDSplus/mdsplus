package mds.jtraverser;

import java.util.StringTokenizer;
import java.util.Vector;

import javax.xml.parsers.*;

import org.w3c.dom.*;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;

public class CompileTree extends Thread
{
	public static void main(String args[])
	{
		String experiment;
		int shot = -1;
		if (args.length < 1)
		{
			System.out.println("Usage: java CompileTree <experiment> [<shot>]");
			System.exit(0);
		}
		experiment = args[0];
		if (args.length > 1)
		{
			try
			{
				shot = Integer.parseInt(args[1]);
			}
			catch (final Exception exc)
			{
				System.out.println("Error Parsing shot number");
				System.exit(0);
			}
		}
		(new CompileTree(experiment, shot)).start();
	}

	MDSplus.Tree tree;
	String experiment;
	int shot;
	// originalNames and renamedNames keep info about nodes to be renamed
	Vector<String> renamedDevices = new Vector<>();
	Vector<String> renamedFieldNids = new Vector<>();
	Vector<String> newNames = new Vector<>();
	Vector<String> unresolvedExprV = new Vector<>();
	Vector<MDSplus.TreeNode> unresolvedNidV = new Vector<>();
	Vector<MDSplus.TreeNode> subtreeNids = new Vector<>();

	CompileTree(String experiment, int shot)
	{
		this.experiment = experiment;
		this.shot = shot;
	}

	MDSplus.TreeNode addNode(String name, String usage) throws Exception
	{
		tree.getDefault();
		String currName = name.substring(1);
		String prevNode = "";
		while (currName.length() > 12)
		{
			final String shortName = currName.substring(0, 12);
			currName = currName.substring(12);
			try
			{
				tree.addNode(prevNode + "." + shortName, "STRUCTURE");
			}
			catch (final Exception exc)
			{}
			prevNode += "." + shortName;
		}
		return tree.addNode(prevNode + name.substring(0, 1) + currName, usage);
	}

	void recCompile(Element node)
	{
		final String type = node.getNodeName();
		final String name = node.getAttribute("NAME");
		final String usageStr = node.getAttribute("USAGE");
		MDSplus.TreeNode nid = null;
		boolean success;
		// System.out.println(name);
		try
		{
			final MDSplus.TreeNode parentNid = tree.getDefault();
			success = false;
			if (type.equals("data"))
			{
				final boolean isDeviceField = node.getNodeName().equals("field");
				final Text dataNode = (Text) node.getFirstChild();
				if (dataNode != null)
				{
					final String dataStr = dataNode.getData();
					final MDSplus.Data data = null;
					/*
					 * try { System.out.println(dataStr);
					 *
					 * data = Data.fromExpr(dataStr); }catch(Exception exc)
					 */
					{
						unresolvedExprV.addElement(dataStr);
						unresolvedNidV.addElement(tree.getDefault());
					}
					try
					{
						nid = tree.getDefault();
						if (isDeviceField)
						{
							MDSplus.Data oldData;
							try
							{
								oldData = nid.getData();
							}
							catch (final Exception exc)
							{
								oldData = null;
							}
							if (oldData == null || !dataStr.equals(oldData.toString()))
								nid.putData(data);
						}
						else
							nid.putData(data);
					}
					catch (final Exception exc)
					{
						System.out.println("Error writing data: " + exc);
					}
				}
				return;
			}
			// First handle renamed nodes: they do not need to be created, but to be renamed
			final String originalDevice = node.getAttribute("DEVICE");
			final String deviceOffsetStr = node.getAttribute("OFFSET_NID");
			if (originalDevice != null && deviceOffsetStr != null && !originalDevice.equals("")
					&& !deviceOffsetStr.equals(""))
			{
				String newName;
				try
				{
					newName = parentNid.getFullPath();
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
				newNames.addElement(newName);
				renamedDevices.addElement(originalDevice);
				renamedFieldNids.addElement(deviceOffsetStr);
				return; // No descedents for a renamed node
			}
			if (type.equals("node"))
			{
				try
				{
//                    if(name.length() > 12) name = name.substring(0,12);
//                    nid = tree.addNode("."+name, NodeInfo.USAGE_STRUCTURE, 0);
					nid = addNode("." + name, "STRUCTURE");
					if (usageStr != null && usageStr.equals("SUBTREE"))
						subtreeNids.addElement(nid);
					tree.setDefault(nid);
					success = true;
				}
				catch (final Exception exc)
				{
					System.out.println("Error adding member " + name + " : " + exc);
				}
			}
			if (type.equals("member"))
			{
				try
				{
//                    if(name.length() > 12) name = name.substring(0,12);
//                    nid = tree.addNode(":"+name, usage, 0);
					nid = addNode(":" + name, usageStr);
					tree.setDefault(nid);
					success = true;
				}
				catch (final Exception exc)
				{
					System.out.println("Error adding member " + name + " : " + exc);
				}
			}
			if (type.equals("device"))
			{
				final String model = node.getAttribute("MODEL");
				try
				{
					Thread.currentThread();
					Thread.sleep(100);
					tree.addDevice(name.trim(), model);
					tree.setDefault(nid = tree.getNode(name.trim()));
					success = true;
				}
				catch (final Exception exc)
				{}
			}
			if (type.equals("field"))
			{
				try
				{
					nid = tree.getNode(name);
					tree.setDefault(nid);
					success = true;
				}
				catch (final Exception exc)
				{
					System.out.println("WARNING: device field  " + name + " not found in model ");
				}
			}
			if (success)
			{
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
						for (final String tag : tags)
							nid.addTag(tag);
					}
					catch (final Exception exc)
					{
						System.out.println("Error adding tags " + tagsStr + " : " + exc);
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
							flags |= NodeInfo.WRITE_ONCE;
						if (flag.equals("COMPRESSIBLE"))
							flags |= NodeInfo.COMPRESSIBLE;
						if (flag.equals("COMPRESS_ON_PUT"))
							flags |= NodeInfo.COMPRESS_ON_PUT;
						if (flag.equals("NO_WRITE_MODEL"))
							flags |= NodeInfo.NO_WRITE_MODEL;
						if (flag.equals("NO_WRITE_SHOT"))
							flags |= NodeInfo.NO_WRITE_SHOT;
						if (flag.equals("COMPRESS_SEGMENTS"))
							flags |= NodeInfo.COMPRESS_SEGMENTS;
					}
					try
					{
						nid.setNciFlags(flags);
					}
					catch (final Exception exc)
					{
						System.out.println("Error setting flags to node " + name + " : " + exc);
					}
				}
				// state
				final String stateStr = node.getAttribute("STATE");
				if (stateStr != null && stateStr.length() > 0)
				{
					try
					{
						if (stateStr.equals("ON"))
							nid.setOn(true);
						if (stateStr.equals("OFF"))
							nid.setOn(false);
					}
					catch (final Exception exc)
					{
						// System.out.println("Error setting state of node " + name + " : " + exc);
					}
				}
				// Descend
				final NodeList nodes = node.getChildNodes();
				for (int i = 0; i < nodes.getLength(); i++)
				{
					final org.w3c.dom.Node currNode = nodes.item(i);
					if (currNode.getNodeType() == org.w3c.dom.Node.ELEMENT_NODE) // Only element nodes at this
						recCompile((Element) currNode);
				}
			}
			tree.setDefault(parentNid);
		}
		catch (final Exception exc)
		{
			System.out.println("Internal error in recCompile: " + exc);
			exc.printStackTrace();
		}
	}

	@Override
	public void run()
	{
		final DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		Document document = null;
		try
		{
			final DocumentBuilder builder = factory.newDocumentBuilder();
			document = builder.parse(experiment + ".xml");
		}
		catch (final SAXParseException spe)
		{
			// Error generated by the parser
			System.out.println("\n** Parsing error" + ", line " + spe.getLineNumber() + ", uri " + spe.getSystemId());
			System.out.println("   " + spe.getMessage());
			// Use the contained exception, if any
			Exception x = spe;
			if (spe.getException() != null)
				x = spe.getException();
			x.printStackTrace();
		}
		catch (final SAXException sxe)
		{
			// Error generated during parsing)
			Exception x = sxe;
			if (sxe.getException() != null)
				x = sxe.getException();
			x.printStackTrace();
		}
		catch (final ParserConfigurationException pce)
		{
			// Parser with specified options can't be built
			pce.printStackTrace();
		}
		catch (final Exception ioe)
		{
			// I/O error
			ioe.printStackTrace();
		}
		try
		{
			tree = new MDSplus.Tree(experiment, shot, "NEW");
		}
		catch (final Exception exc)
		{
			System.out.println(exc);
			System.exit(0);
		}
		final Element rootNode = document.getDocumentElement();
		final NodeList nodes = rootNode.getChildNodes();
		for (int i = 0; i < nodes.getLength(); i++)
		{
			final org.w3c.dom.Node currNode = nodes.item(i);
			if (currNode.getNodeType() == org.w3c.dom.Node.ELEMENT_NODE) // Only element nodes at this
				recCompile((Element) currNode);
		}
		// handle renamed nodes
		for (int i = 0; i < newNames.size(); i++)
		{
			final String newName = newNames.elementAt(i);
			final String deviceName = renamedDevices.elementAt(i);
			final String offsetStr = renamedFieldNids.elementAt(i);
			try
			{
				final int deviceOffset = Integer.parseInt(offsetStr);
				final MDSplus.TreeNode deviceNid = tree.getNode(new MDSplus.TreePath(deviceName));
				final MDSplus.TreeNode renamedNid = new MDSplus.TreeNode(deviceNid.getNid() + deviceOffset);
				renamedNid.setCtxTree(Tree.curr_experiment);
				renamedNid.rename(newName);
			}
			catch (final Exception exc)
			{
				System.out.println("Error renaming node of " + deviceName + " to " + newName + " : " + exc);
			}
		}
		for (int i = 0; i < unresolvedNidV.size(); i++)
		{
			if (unresolvedExprV.elementAt(i).trim().equals("<no-node>"))
				continue;
			MDSplus.Data data = null;
			try
			{
				// System.out.println((String) unresolvedExprV.elementAt(i));
				tree.setDefault(unresolvedNidV.elementAt(i));
				data = tree.tdiCompile(unresolvedExprV.elementAt(i));
			}
			catch (final Exception exc)
			{
				System.out.println("Error parsing expression " + unresolvedExprV.elementAt(i) + " : " + exc);
			}
			try
			{
				unresolvedNidV.elementAt(i).putData(data);
			}
			catch (final Exception exc)
			{
				System.out.println("Error writing data: " + exc);
			}
		}
//Set subtrees (apparently this must be done at the end....
		for (int i = 0; i < subtreeNids.size(); i++)
		{
			try
			{
				subtreeNids.elementAt(i).setSubtree(true);
			}
			catch (final Exception exc)
			{
				System.out.println("Error setting subtree: " + exc);
			}
		}
		try
		{
			tree.write();
			tree.close();
		}
		catch (final Exception exc)
		{
			System.out.println("Error closeing tree: " + exc);
		}
	}
}
