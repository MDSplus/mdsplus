package mds.jtraverser;
import java.io.File;
import java.io.FileOutputStream;
import java.util.Properties;
import java.util.Vector;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Text;

public class DecompileTree
{
	static MDSplus.Tree mdsTree;
	static Document document;

	public static void main(String args[])
	{
		if (args.length < 1)
		{
			System.err.println("Usage: java DecompileTree <treeName> [<shot>]");
			System.exit(0);
		}
		final String treeName = args[0];
		int shot = -1;
		if (args.length > 1)
		{
			try
			{
				shot = Integer.parseInt(args[1]);
			}
			catch (final Exception exc)
			{
				System.err.println("Invalid shot number");
				System.exit(0);
			}
		}
		final Properties properties = System.getProperties();
		final String full = properties.getProperty("full");
		boolean isFull = false;
		if (full != null && full.equals("yes"))
			isFull = true;
		String outName = properties.getProperty("out");
		if (outName == null)
			outName = args[0] + ".xml";
		final DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		try
		{
			final DocumentBuilder builder = factory.newDocumentBuilder();
			document = builder.newDocument(); // Create from whole cloth
		}
		catch (final Exception exc)
		{
			System.err.println("Cannot instantiate a new Document: " + exc);
			document = null;
			System.exit(0);
		}
		final Element tree = document.createElement("tree");
		MDSplus.TreeNode topNid = null;
		try
		{
			mdsTree = new MDSplus.Tree(treeName, shot);
			topNid = new MDSplus.TreeNode(0, mdsTree);
		}
		catch (final Exception exc)
		{
			System.err.println("Cannot open tree: " + exc);
			System.exit(0);
		}
		MDSplus.TreeNodeArray sons = null;
		try
		{
			sons = topNid.getChildren();
		}
		catch (final Exception exc)
		{}
		for (int i = 0; i < sons.size(); i++)
		{
			final Element docSon = document.createElement("node");
			tree.appendChild(docSon);
			recDecompile(sons.getElementAt(i), docSon, false, isFull);
		}
		MDSplus.TreeNodeArray members = null;
		try
		{
			members = topNid.getMembers();
		}
		catch (final Exception exc)
		{}
		for (int i = 0; i < members.size(); i++)
		{
			Element docMember = null;
			try
			{
				if (members.getElementAt(i).getUsage() == "DEVICE")
					docMember = document.createElement("device");
				else if (members.getElementAt(i).getUsage().equals("COMPOUND_DATA"))
					docMember = document.createElement("compound_data");
				else
					docMember = document.createElement("member");
			}
			catch (final Exception exc)
			{
				System.err.println(exc);
			}
			tree.appendChild(docMember);
			recDecompile(members.getElementAt(i), docMember, false, isFull);
		}
		final TransformerFactory transFactory = TransformerFactory.newInstance();
		try
		{
			final Transformer transformer = transFactory.newTransformer();
			final DOMSource source = new DOMSource(tree);
			final File newXML = new File(outName);
			final FileOutputStream os = new FileOutputStream(newXML);
			final StreamResult result = new StreamResult(os);
			transformer.setOutputProperty(OutputKeys.INDENT, "yes");
			transformer.transform(source, result);
		}
		catch (final Exception exc)
		{
			System.err.println(exc);
		}
	}

	static void recDecompile(MDSplus.TreeNode nid, Element node, boolean isDeviceField, boolean isFull)
	{
		try
		{
			final MDSplus.TreeNode prevNid = mdsTree.getDefault();
			mdsTree.setDefault(nid);
			System.out.println(nid.getFullPath());
			String[] tags;
			try
			{
				tags = nid.findTags();
			}
			catch (final Exception exc)
			{
				tags = new String[0];
			}
			if (isDeviceField) // Handle device field display
			{
				MDSplus.Data data = null;
				if (nid.isSetup() || isFull) // || nid.getNodeName().endsWith("_GAIN"))
				{
					try
					{
						data = nid.getData();
					}
					catch (final Exception exc)
					{
						data = null;
					}
				}
				// Handle Possible non-device subtree. Never, never do that!!.....
				MDSplus.TreeNodeArray sons = null, members = null;
				final Vector<MDSplus.TreeNode> subtreeNodes = new Vector<>();
				try
				{
					sons = nid.getChildren();
				}
				catch (final Exception exc)
				{}
				for (int i = 0; i < sons.size(); i++)
				{
					try
					{
						if (sons.getElementAt(i).getConglomerateElt() == 1) // Descendant NOT belonging to the device
							subtreeNodes.addElement(sons.getElementAt(i));
					}
					catch (final Exception exc)
					{}
				}
				final Vector<MDSplus.TreeNode> subtreeMembers = new Vector<>();
				try
				{
					members = nid.getMembers();
				}
				catch (final Exception exc)
				{}
				for (int i = 0; i < members.size(); i++)
				{
					try
					{
						if (members.getElementAt(i).getConglomerateElt() == 1) // Descendant NOT belonging to the device
							subtreeMembers.addElement(members.getElementAt(i));
					}
					catch (final Exception exc)
					{}
				}
				if ((!nid.isOn() && !nid.isParentOff()) || (nid.isOn() && nid.isParentOff())
						|| (nid.isSetup() && data != null) || tags.length > 0 || subtreeNodes.size() > 0
						|| subtreeMembers.size() > 0 || isFull) // show it only at these conditions
				{
					final Element fieldNode = document.createElement("field");
					node.appendChild(fieldNode);
					final int conglomerateElt = nid.getConglomerateElt();
					final MDSplus.TreeNode deviceNid = new MDSplus.TreeNode(nid.getNid() - conglomerateElt + 1,
							mdsTree);
					final String devicePath = deviceNid.getFullPath();
					final String fieldPath = nid.getFullPath();
					if (fieldPath.startsWith(devicePath)) // if the field has not been renamed
					{
						fieldNode.setAttribute("NAME", fieldPath.substring(devicePath.length(), fieldPath.length()));
						if (!nid.isOn() && !nid.isParentOff())
							fieldNode.setAttribute("STATE", "OFF");
						if (nid.isOn() && nid.isParentOff())
							fieldNode.setAttribute("STATE", "ON");
						if (tags.length > 0)
						{
							String tagList = "";
							for (int i = 0; i < tags.length; i++)
								tagList += (i == tags.length - 1) ? tags[i] : (tags[i] + ",");
							fieldNode.setAttribute("TAGS", tagList);
						}
						if (data != null)
						{
							final Element dataNode = document.createElement("data");
							final Text dataText = document.createTextNode(data.toString());
							dataNode.appendChild(dataText);
							fieldNode.appendChild(dataNode);
						}
					}
				}
				// Display possible non device subtrees
				for (int i = 0; i < subtreeNodes.size(); i++)
				{
					final Element currNode = document.createElement("node");
					recDecompile(subtreeNodes.elementAt(i), currNode, false, isFull);
				}
				for (int i = 0; i < subtreeMembers.size(); i++)
				{
					final Element currNode = document.createElement("member");
					recDecompile(subtreeMembers.elementAt(i), currNode, false, isFull);
				}
			} // End management of device fields
			else
			{
				node.setAttribute("NAME", nid.getNodeName());
				if (nid.getUsage().equals("DEVICE") || nid.getUsage().equals("COMPOUND_DATA"))
				{
					MDSplus.Conglom deviceData = null;
					try
					{
						deviceData = (MDSplus.Conglom) nid.getData();
					}
					catch (final Exception exc)
					{
						System.err.println("Error reading device data: " + exc);
					}
					final String model = deviceData.getModel().toString();
					node.setAttribute("MODEL", model.substring(1, model.length() - 1));
				}
				final int conglomerateElt = nid.getConglomerateElt();
				// Handle renamed device fields
				if (conglomerateElt > 1)
				{
					final MDSplus.TreeNode deviceNid = new MDSplus.TreeNode(nid.getNid() - conglomerateElt + 1,
							mdsTree);
					node.setAttribute("DEVICE", deviceNid.getFullPath());
					node.setAttribute("OFFSET_NID", "" + conglomerateElt);
				}
				// tags
				try
				{
					tags = nid.findTags();
				}
				catch (final Exception exc)
				{
					System.err.println("Error getting tags: " + exc);
					tags = new String[0];
				}
				if (tags.length > 0)
				{
					String tagList = "";
					for (int i = 0; i < tags.length; i++)
						tagList += (i == tags.length - 1) ? tags[i] : (tags[i] + ",");
					node.setAttribute("TAGS", tagList);
				}
				// state
				if (!nid.isOn() && !nid.isParentOff())
					node.setAttribute("STATE", "OFF");
				if (nid.isOn() && nid.isParentOff())
					node.setAttribute("STATE", "ON");
				// flags
				String flags = "";
				if (nid.isWriteOnce())
					flags += (flags.length() > 0) ? ",WRITE_ONCE" : "WRITE_ONCE";
				if (nid.isCompressOnPut())
					flags += (flags.length() > 0) ? ",COMPRESSIBLE" : "COMPRESSIBLE";
				if (nid.isCompressOnPut())
					flags += (flags.length() > 0) ? ",COMPRESS_ON_PUT" : "COMPRESS_ON_PUT";
				if (nid.isNoWriteModel())
					flags += (flags.length() > 0) ? ",NO_WRITE_MODEL" : "NO_WRITE_MODEL";
				if (nid.isNoWriteShot())
					flags += (flags.length() > 0) ? ",NO_WRITE_SHOT" : "NO_WRITE_SHOT";
				if (nid.isCompressSegments())
					flags += (flags.length() > 0) ? ",COMPRESS_SEGMENTS" : "COMPRESS_SEGMENTS";
				if (flags.length() > 0)
					node.setAttribute("FLAGS", flags);
				// usage
				final String usage = nid.getUsage();
				if (!usage.equals("STRUCTURE") && !usage.equals("DEVICE") && !usage.equals("COMPOUND_DATA"))
				{
					node.setAttribute("USAGE", usage);
					// if(info.isSetup())
					{
						MDSplus.Data data;
						try
						{
							data = nid.getData();
						}
						catch (final Exception exc)
						{
							data = null;
						}
						if (data != null)
						{
							final Element dataNode = document.createElement("data");
							final Text dataText = document.createTextNode(data.toString());
							dataNode.appendChild(dataText);
							node.appendChild(dataNode);
						}
					}
				}
				// handle descendants, if not subtree
				if (!usage.equals("SUBTREE"))
				{
					MDSplus.TreeNodeArray sons = null;
					try
					{
						sons = nid.getChildren();
					}
					catch (final Exception exc)
					{}
					if (nid.getUsage().equals("DEVICE") || nid.getUsage().equals("COMPOUND_DATA"))
					{
						// int numFields = info.getConglomerateNids() - 1;
						final int numFields = nid.getConglomerateNodes().size();
						for (int i = 1; i < numFields; i++)
							recDecompile(new MDSplus.TreeNode(nid.getNid() + i, mdsTree), node, true, isFull);
					}
					else
					{
						for (int i = 0; i < sons.size(); i++)
						{
							final Element docSon = document.createElement("node");
							node.appendChild(docSon);
							recDecompile(sons.getElementAt(i), docSon, false, isFull);
						}
						MDSplus.TreeNodeArray members = null;
						try
						{
							members = nid.getMembers();
						}
						catch (final Exception exc)
						{}
						for (int i = 0; i < members.size(); i++)
						{
							Element docMember;
							if (members.getElementAt(i).getUsage().equals("DEVICE"))
								docMember = document.createElement("device");
							else if (members.getElementAt(i).getUsage().equals("COMPOUND_DATA"))
								docMember = document.createElement("compound_data");
							else
								docMember = document.createElement("member");
							node.appendChild(docMember);
							recDecompile(members.getElementAt(i), docMember, false, isFull);
						}
					}
				}
			}
			mdsTree.setDefault(prevNid);
		}
		catch (final Exception exc)
		{
			System.err.println(exc);
		}
	}
	/*
	 * Element member = (Element) document.createElement("member");
	 * member.setAttribute("USAGE", "TEXT"); tree.appendChild(member); Element data
	 * = (Element) document.createElement("data"); Text dataText = (Text)
	 * document.createTextNode("2+3"); data.appendChild(dataText);
	 * member.appendChild(data);
	 */
}
