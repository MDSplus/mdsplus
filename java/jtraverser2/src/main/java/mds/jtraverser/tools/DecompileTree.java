package mds.jtraverser.tools;

import java.awt.Component;
import java.io.File;
import java.io.FileOutputStream;
import java.util.Properties;
import java.util.Vector;
import javax.swing.JFileChooser;
import javax.swing.filechooser.FileNameExtensionFilter;
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
import mds.MdsException;
import mds.data.TREE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Conglom;
import mds.data.descriptor_s.NODE;
import mds.data.descriptor_s.NODE.Flags;
import mds.data.descriptor_s.Nid;
import mds.mdsip.MdsIp;

public class DecompileTree
{
	public static final void main(final String args[])
	{
		if (args.length < 1)
		{
			System.out.println("Usage: java DecompileTree <provider> <treeName> [<shot>]");
			System.exit(0);
		}
		final String provider = args[0];
		final String expt = args[1];
		int shot = -1;
		if (args.length > 1)
			try
			{
				shot = Integer.parseInt(args[2]);
			}
			catch (final Exception exc)
			{
				System.err.println("Invalid shot number");
				System.exit(-1);
			}
		final Properties properties = System.getProperties();
		final String full = properties.getProperty("full");
		boolean isFull = true;
		if (full != null && full.equals("no"))
			isFull = false;
		String filename = properties.getProperty("out");
		if (filename == null)
			filename = args[1] + "@" + args[0];
		try
		{
			System.exit(new DecompileTree().decompile(provider, expt, shot, filename, isFull));
		}
		catch (final MdsException e)
		{
			e.printStackTrace();
			System.exit(-1);
		}
	}

	private TREE tree = null;
	private Document document = null;
	public String error = null;

	public final int decompile(final String provider, final String expt, final int shot, final String filename,
			final boolean isFull) throws MdsException
	{
		final int result;
		try(final TREE subtree = new TREE(new MdsIp(provider), expt, shot, TREE.READONLY))
		{
			result = this.decompile(subtree, filename, isFull);
		}
		return result;
	}

	public int decompile(final TREE tree_in, final Component parent, final boolean isFull)
	{
		final JFileChooser fc = new JFileChooser();
		fc.addChoosableFileFilter(new FileNameExtensionFilter("XML file", "xml"));
		fc.setFileFilter(new FileNameExtensionFilter("XML document", "xml"));
		final int returnVal = fc.showSaveDialog(parent);
		if (returnVal != JFileChooser.APPROVE_OPTION)
			return 0;
		return this.decompile(tree_in, fc.getSelectedFile().getAbsolutePath(), isFull);
	}

	public final int decompile(final TREE dtree, String filename, final boolean isFull)
	{
		this.tree = dtree;
		if (!filename.contains("."))
			filename += ".xml";
		final DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		try
		{
			final DocumentBuilder builder = factory.newDocumentBuilder();
			this.document = builder.newDocument(); // Create from whole cloth
		}
		catch (final Exception e)
		{
			System.err.println(this.error = "Cannot instantiate a new Document: " + e);
			this.document = null;
			return -1;
		}
		final Element etree = this.document.createElement("tree");
		final Nid topNid = this.tree.getTop();
		try
		{
			final Nid[] sons = topNid.getNciChildrenNids().toArray();
			for (final Nid son : sons)
			{
				final Element docSon = this.document.createElement("node");
				etree.appendChild(docSon);
				this.recDecompile(son, docSon, false, isFull);
			}
		}
		catch (final MdsException e)
		{
			System.err.println(this.error = "Error setting sons: " + e);
		}
		Nid[] members;
		try
		{
			members = topNid.getNciMemberNids().toArray();
			for (final Nid member : members)
			{
				Element docMember = null;
				try
				{
					final byte usage = member.getNciUsage();
					if (usage == NODE.USAGE_DEVICE)
						docMember = this.document.createElement("device");
					else if (usage == NODE.USAGE_COMPOUND_DATA)
						docMember = this.document.createElement("compound_data");
					else
						docMember = this.document.createElement("member");
				}
				catch (final Exception e)
				{
					System.err.println(this.error = e.toString());
				}
				etree.appendChild(docMember);
				this.recDecompile(member, docMember, false, isFull);
			}
		}
		catch (final Exception e)
		{
			System.err.println(this.error = "Error setting members: " + e);
		}
		final TransformerFactory transFactory = TransformerFactory.newInstance();
		try
		{
			final Transformer transformer = transFactory.newTransformer();
			final DOMSource source = new DOMSource(etree);
			final File newXML = new File(filename);
			try (final FileOutputStream os = new FileOutputStream(newXML))
			{
				final StreamResult result = new StreamResult(os);
				transformer.setOutputProperty(OutputKeys.INDENT, "yes");
				transformer.transform(source, result);
			}
		}
		catch (final Exception e)
		{
			System.err.println(this.error = e.toString());
		}
		return 0;
	}

	private void recDecompile(final Nid nid, final Element node, final boolean isDeviceField, final boolean isFull)
	{
		try
		{
			final Nid prevNid = this.tree.getDefaultNid();
			nid.setDefault();
			final Flags flags;
			try
			{
				flags = new Flags(nid.getNciFlags());
			}
			catch (final MdsException exc)
			{
				System.err.println(this.error = "Error getting info for " + nid.getNciFullPath() + ": " + exc);
				return;
			}
			String[] tags;
			try
			{
				tags = nid.getTags();
			}
			catch (final Exception exc)
			{
				tags = new String[0];
			}
			if (isDeviceField) // Handle device field display
			{
				Descriptor<?> data = null;
				// TACON
				final String name = nid.getNciNodeName();
				if (name.endsWith("_GAIN"))
					System.out.println("TACON: " + name);
				if (flags.isSetup() || isFull || name.endsWith("_GAIN"))
					try
					{
						data = nid.getRecord();
					}
					catch (final MdsException exc)
					{/**/}
				// Handle Possible non-device subtree. Never, never do that!!.....
				final Vector<Nid> subtreeNodes = new Vector<>();
				try
				{
					final Nid[] sons = nid.getNciChildrenNids().toArray();
					for (final Nid son : sons)
						try
						{
							if (son.getNciConglomerateElt() == 1) // Descendant NOT belonging to the device
								subtreeNodes.addElement(son);
						}
						catch (final MdsException exc)
						{/**/}
				}
				catch (final MdsException exc)
				{/**/}
				final Vector<Nid> subtreeMembers = new Vector<>();
				try
				{
					final Nid[] members = nid.getNciMemberNids().toArray();
					for (final Nid member : members)
						try
						{
							if (member.getNciConglomerateElt() == 1) // Descendant NOT belonging to the device
								subtreeMembers.addElement(member);
						}
						catch (final MdsException exc)
						{/**/}
				}
				catch (final MdsException exc)
				{/**/}
				if ((!flags.isOn() && flags.isParentOn()) || (flags.isOn() && !flags.isParentOn())
						|| (flags.isSetup() && data != null) || tags.length > 0 || subtreeNodes.size() > 0
						|| subtreeMembers.size() > 0 || isFull
						// TACON
						|| name.endsWith("_GAIN")) // show it only at these conditions
				{
					final Element fieldNode = this.document.createElement("field");
					node.appendChild(fieldNode);
					final int conglomerateElt = nid.getNciConglomerateElt();
					final Nid deviceNid = new Nid(nid, -conglomerateElt + 1);
					final String devicePath = deviceNid.getNciFullPath();
					final String fieldPath = nid.getNciFullPath();
					if (fieldPath.startsWith(devicePath)) // if the field has not been renamed
					{
						fieldNode.setAttribute("NAME", fieldPath.substring(devicePath.length(), fieldPath.length()));
						if (!flags.isOn() && flags.isParentOn())
							fieldNode.setAttribute("STATE", "OFF");
						if (flags.isOn() && !flags.isParentOn())
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
							final Element dataNode = this.document.createElement("data");
							final Text dataText = this.document.createTextNode(data.toString());
							dataNode.appendChild(dataText);
							fieldNode.appendChild(dataNode);
						}
					}
				}
				// Display possible non device subtrees
				for (int i = 0; i < subtreeNodes.size(); i++)
				{
					final Element currNode = this.document.createElement("node");
					this.recDecompile(subtreeNodes.elementAt(i), currNode, false, isFull);
				}
				for (int i = 0; i < subtreeMembers.size(); i++)
				{
					final Element currNode = this.document.createElement("member");
					this.recDecompile(subtreeMembers.elementAt(i), currNode, false, isFull);
				}
			} // End management of device fields
			else
			{
				node.setAttribute("NAME", nid.getNciNodeName());
				final byte usage = nid.getNciUsage();
				if (usage == NODE.USAGE_DEVICE || usage == NODE.USAGE_COMPOUND_DATA)
				{
					Conglom deviceData = null;
					try
					{
						deviceData = (Conglom) nid.getRecord();
						final String model = deviceData.getModel().toString();
						node.setAttribute("MODEL", model.substring(1, model.length() - 1));
					}
					catch (final Exception exc)
					{
						System.err.println("Error reading device data: " + exc);
					}
				}
				final int conglomerateElt = nid.getNciConglomerateElt();
				// Handle renamed device fields
				if (conglomerateElt > 1)
				{
					final Nid deviceNid = new Nid(nid, -conglomerateElt + 1);
					try
					{
						node.setAttribute("DEVICE", deviceNid.getNciFullPath());
						node.setAttribute("OFFSET_NID", "" + conglomerateElt);
					}
					catch (final Exception exc)
					{
						System.err.println("Error getting device info: " + exc);
					}
				}
				// tags
				try
				{
					tags = nid.getTags();
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
				if (!flags.isOn() && flags.isParentOn())
					node.setAttribute("STATE", "OFF");
				if (flags.isOn() && !flags.isParentOn())
					node.setAttribute("STATE", "ON");
				// flags
				String flagsStr = "";
				if (flags.isWriteOnce())
					flagsStr += (flagsStr.length() > 0) ? ",WRITE_ONCE" : "WRITE_ONCE";
				if (flags.isCompressible())
					flagsStr += (flagsStr.length() > 0) ? ",COMPRESSIBLE" : "COMPRESSIBLE";
				if (flags.isCompressOnPut())
					flagsStr += (flagsStr.length() > 0) ? ",COMPRESS_ON_PUT" : "COMPRESS_ON_PUT";
				if (flags.isNoWriteModel())
					flagsStr += (flagsStr.length() > 0) ? ",NO_WRITE_MODEL" : "NO_WRITE_MODEL";
				if (flags.isNoWriteShot())
					flagsStr += (flagsStr.length() > 0) ? ",NO_WRITE_SHOT" : "NO_WRITE_SHOT";
				if (flagsStr.length() > 0)
					node.setAttribute("FLAGS", flagsStr);
				// usage
				if (usage != NODE.USAGE_STRUCTURE && usage != NODE.USAGE_DEVICE && usage != NODE.USAGE_COMPOUND_DATA)
				{
					String usageStr;
					switch (usage)
					{
					default:
						usageStr = "ANY";
						break;
					case NODE.USAGE_ACTION:
						usageStr = "ACTION";
						break;
					case NODE.USAGE_DISPATCH:
						usageStr = "DISPATCH";
						break;
					case NODE.USAGE_NUMERIC:
						usageStr = "NUMERIC";
						break;
					case NODE.USAGE_SIGNAL:
						usageStr = "SIGNAL";
						break;
					case NODE.USAGE_TASK:
						usageStr = "TASK";
						break;
					case NODE.USAGE_TEXT:
						usageStr = "TEXT";
						break;
					case NODE.USAGE_WINDOW:
						usageStr = "WINDOW";
						break;
					case NODE.USAGE_AXIS:
						usageStr = "AXIS";
						break;
					case NODE.USAGE_SUBTREE:
						usageStr = "SUBTREE";
						break;
					}
					node.setAttribute("USAGE", usageStr);
					// if(info.isSetup())
					{
						Descriptor<?> data;
						try
						{
							data = nid.getRecord();
						}
						catch (final Exception exc)
						{
							data = null;
						}
						if (data != null)
						{
							final Element dataNode = this.document.createElement("data");
							final Text dataText = this.document.createTextNode(data.decompile());
							dataNode.appendChild(dataText);
							node.appendChild(dataNode);
						}
					}
				}
				// handle descendants, if not subtree
				if (usage != NODE.USAGE_SUBTREE)
					if (usage == NODE.USAGE_DEVICE || usage == NODE.USAGE_COMPOUND_DATA)
					{
						// int numFields = info.getConglomerateNids() - 1;
						final Nid[] nids = nid.getNciConglomerateNids().toArray();
						for (final Nid n : nids)
							this.recDecompile(n, node, true, isFull);
					}
					else
					{
						try
						{
							final Nid[] sons = nid.getNciChildrenNids().toArray();
							for (final Nid son : sons)
							{
								final Element docSon = this.document.createElement("node");
								node.appendChild(docSon);
								this.recDecompile(son, docSon, false, isFull);
							}
						}
						catch (final MdsException exc)
						{/**/}
						try
						{
							final Nid[] members = nid.getNciMemberNids().toArray();
							for (final Nid member : members)
							{
								Element docMember;
								final byte cusage = member.getNciUsage();
								if (cusage == NODE.USAGE_DEVICE)
									docMember = this.document
											.createElement((cusage == NODE.USAGE_DEVICE) ? "device" : "compound_data");
								else if (cusage == NODE.USAGE_COMPOUND_DATA)
									docMember = this.document.createElement("compound_data");
								else
									docMember = this.document.createElement("member");
								node.appendChild(docMember);
								this.recDecompile(member, docMember, false, isFull);
							}
						}
						catch (final MdsException exc)
						{/**/}
					}
			}
			prevNid.setDefault();
		}
		catch (final Exception exc)
		{
			System.err.println(exc);
		}
	}
}
