/**
 *Load Configuration from a pulse file. It reads from file LoadPulse.conf. Each line
 * of the file is interpteted as a node freference, and the whole subtree will be read and stored
 * (as decompiled). The read values are then written into the model.
 *
 *+
 *
 *
 *  */
import java.io.*;
import java.util.*;

public class LoadPulse
{
    static final String confFileName = "LoadPulse.conf";

    static class NodeDescriptor
    {
        String path;
        String decompiled;
        boolean on, parentOn, noWriteModel;
        NodeDescriptor(String path, String decompiled, boolean on, boolean parentOn, boolean noWriteModel)
        {
            this.path = path;
            this.decompiled = decompiled;
            this.on = on;
            this.parentOn = parentOn;
            this.noWriteModel = noWriteModel;
        }

        String getPath()
        {
            return path;
        }

        String getDecompiled()
        {
            return decompiled;
        }

        boolean isOn(){return on;}
        boolean isParentOn(){return parentOn;}
        boolean isNoWriteModel(){return noWriteModel;}
    }


    public static void main(String args[])
    {

        Vector nodesV = new Vector();
        if (args.length != 2)
        {
            System.out.println("Usage: java LoadPulse <experiment> <shot>");
            System.exit(0);
        }
        try
        {
            Database tree = new Database(args[0], Integer.parseInt(args[1]));
            tree.open();
            BufferedReader br = new BufferedReader(new FileReader(confFileName));
            String basePath;
            String currPath = "";
            NidData defNid = tree.getDefault(0);
            while ( (basePath = br.readLine()) != null)
            {
               NidData currNid;
                try
                {
                    currNid = tree.resolve(new PathData(basePath), 0);
                    tree.setDefault(currNid, 0);
                    NidData[] nidsNumeric = tree.getWild( NodeInfo.USAGE_NUMERIC, 0);
                    if(nidsNumeric == null) nidsNumeric = new NidData[0];
                    NidData[] nidsText = tree.getWild( NodeInfo.USAGE_TEXT, 0);
                    if(nidsText == null) nidsText = new NidData[0];
                    NidData[] nidsSignal = tree.getWild( NodeInfo.USAGE_SIGNAL, 0);
                    if(nidsSignal == null) nidsSignal = new NidData[0];
                    NidData[] nidsStruct = tree.getWild( NodeInfo.USAGE_STRUCTURE, 0);
                    if(nidsStruct == null) nidsStruct = new NidData[0];
                    NidData nids[] = new NidData[nidsNumeric.length + nidsText.length + nidsSignal.length + nidsStruct.length];
                    int j = 0;
                    for (int i = 0; i < nidsNumeric.length; i++)
                        nids[j++] = nidsNumeric[i];
                    for (int i = 0; i < nidsText.length; i++)
                        nids[j++] = nidsText[i];
                    for (int i = 0; i < nidsSignal.length; i++)
                        nids[j++] = nidsSignal[i];
                    for (int i = 0; i < nidsStruct.length; i++)
                        nids[j++] = nidsStruct[i];

                    tree.setDefault(defNid, 0);

                    for (int i = 0; i < nids.length; i++)
                    {
                        NodeInfo currInfo = tree.getInfo(nids[i], 0);
                        currPath = currInfo.getFullPath();
                        System.out.println(currPath);
                        try
                        {
                            Data currData = tree.getData(nids[i], 0);
                            if (currData != null)
                            {
                                String currDecompiled = currData.toString();
                                nodesV.addElement(new NodeDescriptor(currPath,
                                    currDecompiled, currInfo.isOn(), currInfo.isParentOn(),
                                    currInfo.isNoWriteModel()||currInfo.isWriteOnce()));
                            }
                            else
                                nodesV.addElement(new NodeDescriptor(currPath,
                                    null, currInfo.isOn(), currInfo.isParentOn(),
                                    currInfo.isNoWriteModel()||currInfo.isWriteOnce()));
                        }
                        catch (Exception exc)
                        {
                            nodesV.addElement(new NodeDescriptor(currPath,
                                    null, currInfo.isOn(), currInfo.isParentOn(),
                                    currInfo.isNoWriteModel()||currInfo.isWriteOnce()));
                        }
                    }
                }
                catch (Exception exc)
                {
                    System.err.println("Error reading " + currPath + ": " + exc);
                }
            }
            tree.close(0);
            br.close();
            tree = new Database(args[0], -1);
            tree.open();
            for (int i = 0; i < nodesV.size(); i++)
            {
                NodeDescriptor currNode = (NodeDescriptor) nodesV.elementAt(i);

                try
                {
                    NidData currNid = tree.resolve(new PathData(currNode.getPath()), 0);

                    //if(currNode.isNoWriteModel()) System.out.println("NO WRITE MODEL!!" + currNode.getPath());

                    if(currNode.getDecompiled() != null && !currNode.isNoWriteModel())
                    {
                         Data currData = Data.fromExpr(currNode.getDecompiled());
                        tree.putData(currNid, currData, 0);
                    }
                    if(currNode.isOn() && currNode.isParentOn())
                       tree.setOn(currNid, true, 0);
                    else if(currNode.isParentOn())
                    {
                        tree.setOn(currNid, false, 0);
                    }
               }
                catch (Exception exc)
                {
                    System.err.println("Error writing " + currNode.getPath() +
                                       " in model: " + exc);
                }
            }
            tree.close(0);
        }
        catch (Exception exc)
        {
            System.err.println("FATAL ERROR: " + exc);
        }
    }
}
