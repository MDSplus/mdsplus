import javax.xml.parsers.*;
import javax.xml.transform.*;
import javax.xml.transform.dom.*;
import javax.xml.transform.stream.*;
import org.xml.sax.*;
import org.xml.sax.helpers.*;
import org.w3c.dom.*;
import java.io.*;
import java.util.*;


public class DecompileTree
{
    static Database mdsTree;
    static Document document;
    public static void main(String args[])
    {

        if(args.length < 1)
        {
            System.err.println("Usage: java DecompileTree <treeName> [<shot>]");
            System.exit(0);
        }

        String treeName = args[0];
        int shot = -1;
        if(args.length > 1)
        {
            try {
                shot = Integer.parseInt(args[1]);
            } catch(Exception exc) {System.err.println("Invalid shot number"); System.exit(0);}
        }

        Properties properties = System.getProperties();
        String full = properties.getProperty("full");
        boolean isFull = false;
        if(full != null && full.equals("yes"))
            isFull = true;

        String outName = properties.getProperty("out");
        if(outName == null)
            outName = args[0]+".xml";


        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        try {
          DocumentBuilder builder = factory.newDocumentBuilder();
          document = builder.newDocument();  // Create from whole cloth
        }catch(Exception exc)
        {
            System.err.println("Cannot instantiate a new Document: " + exc);
            document = null;
            System.exit(0);
        }
        Element tree = (Element) document.createElement("tree");

        mdsTree = new Database(treeName, shot);
        try {
            mdsTree.open();
        }catch(Exception exc)
        {
            System.err.println("Cannot open tree: " + exc);
            System.exit(0);
        }
        NidData topNid = new NidData(0);
        NidData [] sons;
        try {
            sons = mdsTree.getSons(topNid, 0);
        }catch(Exception exc) {sons = new NidData[0];}
        for(int i = 0; i < sons.length; i++)
        {
            Element docSon = (Element) document.createElement("node");
            tree.appendChild(docSon);
            NidData prevNid = null;
            recDecompile(sons[i], docSon, false, isFull);
         }
        NidData [] members;
        try {
            members = mdsTree.getMembers(topNid, 0);
	    if(members == null) members = new NidData[0];
        }catch(Exception exc) {members = new NidData[0];}

        for(int i = 0; i < members.length; i++)
        {
            Element docMember = null;
            try {
                NodeInfo info = mdsTree.getInfo(members[i], 0);
                if(info.getUsage() == NodeInfo.USAGE_DEVICE)
                    docMember = (Element) document.createElement("device");
                if(info.getUsage() == NodeInfo.USAGE_COMPOUND_DATA)
                    docMember = (Element) document.createElement("compound_data");
                else
                    docMember = (Element) document.createElement("member");
            }catch(Exception exc){System.err.println(exc);}
            tree.appendChild(docMember);
            NidData prevNid = null;
             recDecompile(members[i], docMember, false, isFull);
        }
        TransformerFactory transFactory = TransformerFactory.newInstance();
        try {
            Transformer transformer = transFactory.newTransformer();
            DOMSource source = new DOMSource(tree);
            File newXML = new File(outName);
            FileOutputStream os = new FileOutputStream(newXML);
            StreamResult result = new StreamResult(os);
            transformer.setOutputProperty(OutputKeys.INDENT, "yes");
            transformer.transform(source, result);
        }catch(Exception  exc){System.err.println(exc);}
    }



    static void recDecompile(NidData nid, Element node, boolean isDeviceField, boolean isFull)
    {
        try {
            NidData prevNid = mdsTree.getDefault(0);
            mdsTree.setDefault(nid, 0);

            NodeInfo info = null;
            try {
                info = mdsTree.getInfo(nid, 0);
            }catch(Exception exc)
            {
                System.err.println("Error getting info: " + exc);
            }
            System.out.println(info.getFullPath());
            String [] tags;
            try {
                tags = mdsTree.getTags(nid, 0);
            }catch(Exception exc){tags = new String[0];}
           if(isDeviceField) //Handle device field display
            {
                Data data = null;
		
		//TACON
		if(info.getName().endsWith("_GAIN"))
			System.out.println("TACON: "+ info.getName());
		
		if(info.isSetup() || isFull || info.getName().endsWith("_GAIN"))
		
		
                //if(info.isSetup() || isFull)
                {
                    try {
                        data = mdsTree.getData(nid, 0);
                    }catch(Exception exc) { data = null;}
                }
                //Handle Possible non-device subtree. Never, never do that!!.....
                NidData sons[], members[];
                Vector subtreeNodes = new Vector();
                try {
                    sons = mdsTree.getSons(nid, 0);
                }catch(Exception exc) { sons = new NidData[0];}
                for(int i = 0; i < sons.length; i++)
                {
                    try {
                        NodeInfo currInfo = mdsTree.getInfo(sons[i], 0);
                        if(currInfo.getConglomerateElt() == 1) //Descendant NOT belonging to the device
                            subtreeNodes.addElement(sons[i]);
                    }catch(Exception exc){}
                }
                Vector subtreeMembers = new Vector();
                try {
                    members = mdsTree.getMembers(nid, 0);
		    if(members == null) members = new NidData[0];
                }catch(Exception exc) { members = new NidData[0];}
                for(int i = 0; i < members.length; i++)
                {
                    try {
                        NodeInfo currInfo = mdsTree.getInfo(members[i], 0);
                        if(currInfo.getConglomerateElt() == 1) //Descendant NOT belonging to the device
                            subtreeMembers.addElement(members[i]);
                    }catch(Exception exc){}
                }

                if((!info.isOn() && info.isParentOn())
                    || (info.isOn() && !info.isParentOn())
                    || (info.isSetup()&& data != null) || tags.length > 0
                    || subtreeNodes.size() > 0 || subtreeMembers.size() > 0
                    || isFull

//TACON		    
||info.getName().endsWith("_GAIN")		    
		    
		    
		    ) //show it only at these conditions
                {
                    Element fieldNode = (Element) document.createElement("field");
                    node.appendChild(fieldNode);
                    int conglomerateElt = info.getConglomerateElt();
                    NidData deviceNid = new NidData(nid.getInt() - conglomerateElt + 1);
                    NodeInfo deviceInfo = null;
                    try {
                        deviceInfo = mdsTree.getInfo(deviceNid, 0);
                    }catch(Exception exc)
                    {
                        System.err.println("Error getting device info: " + exc);
                    }
                    String devicePath = deviceInfo.getFullPath();
                    String fieldPath = info.getFullPath();
                    if(fieldPath.startsWith(devicePath)) //if the field has not been renamed
                    {
                        fieldNode.setAttribute("NAME", fieldPath.substring(devicePath.length(), fieldPath.length()));
                        if(!info.isOn() && info.isParentOn())
                            fieldNode.setAttribute("STATE", "OFF");
                        if(info.isOn() && !info.isParentOn())
                            fieldNode.setAttribute("STATE", "ON");
                        if(tags.length > 0)
                        {
                            String tagList = "";
                            for(int i = 0; i < tags.length; i++)
                                tagList += (i == tags.length - 1)?tags[i]:(tags[i]+",");
                            fieldNode.setAttribute("TAGS", tagList);
                        }
                        if(data != null)
                        {
                            Element dataNode = (Element) document.createElement("data");
                            Text dataText = (Text) document.createTextNode(data.toString());
                            dataNode.appendChild(dataText);
                            fieldNode.appendChild(dataNode);
                        }
                    }
                }
                //Display possible non device subtrees
                for(int i = 0; i < subtreeNodes.size(); i++)
                {
                    Element currNode = (Element) document.createElement("node");
                    recDecompile((NidData)subtreeNodes.elementAt(i), currNode, false, isFull);
                }
                for(int i = 0; i < subtreeMembers.size(); i++)
                {
                    Element currNode = (Element) document.createElement("member");
                    recDecompile((NidData)subtreeMembers.elementAt(i), currNode, false, isFull);
                }

            } //End management of device fields
            else
            {

                node.setAttribute("NAME", info.getName());
                if(info.getUsage() == NodeInfo.USAGE_DEVICE || info.getUsage() == NodeInfo.USAGE_COMPOUND_DATA)
                {
                    ConglomData deviceData = null;
                    try {
                        deviceData = (ConglomData)mdsTree.getData(nid, 0);
                    }catch(Exception exc)
                    {
                        System.err.println("Error reading device data: " + exc);
                    }
                    String model = deviceData.getModel().toString();
                    node.setAttribute("MODEL", model.substring(1, model.length() - 1));
                }
                int conglomerateElt = info.getConglomerateElt();

                //Handle renamed device fields
                if(conglomerateElt > 1)
                {
                    NidData deviceNid = new NidData(nid.getInt() - conglomerateElt + 1);
                    NodeInfo deviceInfo = null;
                    try {
                        deviceInfo = mdsTree.getInfo(deviceNid, 0);
                    }catch(Exception exc)
                    {
                        System.err.println("Error getting device info: " + exc);
                    }
                    node.setAttribute("DEVICE", deviceInfo.getFullPath());
                    node.setAttribute("OFFSET_NID", "" + conglomerateElt);
                }
                //tags
                try {
                    tags = mdsTree.getTags(nid, 0);
                }catch(Exception exc)
                {
                    System.err.println("Error getting tags: " + exc);
                    tags = new String[0];
                }
                if(tags.length > 0)
                {
                    String tagList = "";
                    for(int i = 0; i < tags.length; i++)
                        tagList += (i == tags.length - 1)?tags[i]:(tags[i]+",");
                    node.setAttribute("TAGS", tagList);
                }
                //state
                if(!info.isOn() && info.isParentOn())
                    node.setAttribute("STATE", "OFF");
                if(info.isOn() && !info.isParentOn())
                    node.setAttribute("STATE", "ON");
                //flags
                String flags = "";
                if(info.isWriteOnce()) flags += (flags.length() > 0)?",WRITE_ONCE":"WRITE_ONCE";
                if(info.isCompressible()) flags += (flags.length() > 0)?",COMPRESSIBLE":"COMPRESSIBLE";
                if(info.isCompressOnPut()) flags += (flags.length() > 0)?",COMPRESS_ON_PUT":"COMPRESS_ON_PUT";
                if(info.isNoWriteModel()) flags += (flags.length() > 0)?",NO_WRITE_MODEL":"NO_WRITE_MODEL";
                if(info.isNoWriteShot()) flags += (flags.length() > 0)?",NO_WRITE_SHOT":"NO_WRITE_SHOT";

                if(flags.length() > 0)
                    node.setAttribute("FLAGS", flags);

                //usage
                int usage = info.getUsage();
                if(usage != NodeInfo.USAGE_STRUCTURE
                    && usage != NodeInfo.USAGE_DEVICE && usage != NodeInfo.USAGE_COMPOUND_DATA)
                {
                    String usageStr = "";
                    switch(usage) {
                        case NodeInfo.USAGE_NONE : usageStr = "NONE"; break;
                        case NodeInfo.USAGE_ACTION : usageStr = "ACTION"; break;
                        case NodeInfo.USAGE_DISPATCH : usageStr = "DISPATCH"; break;
                        case NodeInfo.USAGE_NUMERIC : usageStr = "NUMERIC"; break;
                        case NodeInfo.USAGE_SIGNAL : usageStr = "SIGNAL"; break;
                        case NodeInfo.USAGE_TASK : usageStr = "TASK"; break;
                        case NodeInfo.USAGE_TEXT : usageStr = "TEXT"; break;
                        case NodeInfo.USAGE_WINDOW : usageStr = "WINDOW"; break;
                        case NodeInfo.USAGE_AXIS : usageStr = "AXIS"; break;
                        case NodeInfo.USAGE_SUBTREE : usageStr = "SUBTREE"; break;
                    }
                    node.setAttribute("USAGE", usageStr);

                    //if(info.isSetup())
                    {
                        Data data;
                        try {
                            data = mdsTree.getData(nid, 0);
                        }catch(Exception exc) { data = null;}
                        if(data != null)
                        {
                            Element dataNode = (Element) document.createElement("data");
                            Text dataText = (Text) document.createTextNode(data.toString());
                            dataNode.appendChild(dataText);
                            node.appendChild(dataNode);
                        }
                    }
                }

            //handle descendants, if not subtree
                if(usage != NodeInfo.USAGE_SUBTREE)
                {
                    NidData [] sons;
                    try {
                        sons = mdsTree.getSons(nid, 0);
                    }catch(Exception exc){sons = new NidData[0];}
                    if(info.getUsage() == NodeInfo.USAGE_DEVICE || info.getUsage() == NodeInfo.USAGE_COMPOUND_DATA)
                    {
                     // int numFields = info.getConglomerateNids() - 1;
                      int numFields = info.getConglomerateNids();
                        for(int i = 1; i < numFields; i++)
                            recDecompile(new NidData(nid.getInt() + i), node, true, isFull);
                    }
                    else
                    {
                        for(int i = 0; i < sons.length; i++)
                        {
                            Element docSon = (Element) document.createElement("node");
                            node.appendChild(docSon);
                            recDecompile(sons[i], docSon, false, isFull);
                        }
                        NidData [] members;
                        try {
                            members = mdsTree.getMembers(nid, 0);
                        }catch(Exception exc){members = new NidData[0];}
                        for(int i = 0; i < members.length; i++)
                        {
                            Element docMember;
                            NodeInfo currInfo = mdsTree.getInfo(members[i], 0);
                            if(currInfo.getUsage() == NodeInfo.USAGE_DEVICE)
                                docMember = (Element) document.createElement((currInfo.getUsage() == NodeInfo.USAGE_DEVICE )?"device":"compound_data");
                            else if(currInfo.getUsage() == NodeInfo.USAGE_COMPOUND_DATA)
                                docMember = (Element) document.createElement("compound_data");
                            else
                                docMember = (Element) document.createElement("member");
                            node.appendChild(docMember);
                            recDecompile(members[i], docMember, false, isFull);
                        }
                    }
                }
            }
            mdsTree.setDefault(prevNid, 0);
        }catch(Exception exc)
        {
            System.err.println(exc);
        }
    }


/*
          Element member = (Element) document.createElement("member");
          member.setAttribute("USAGE", "TEXT");
          tree.appendChild(member);
          Element data = (Element) document.createElement("data");
          Text dataText = (Text) document.createTextNode("2+3");
          data.appendChild(dataText);
          member.appendChild(data);
*/


}
