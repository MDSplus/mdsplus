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
    Database tree;
    int numPMUnits = 0;
    String PCconnection = "";
    String PVconnection = "";
    float rTransfer = 0;

    static class NodeDescriptor
    {
        String path;
        String decompiled;
        boolean on, parentOn, noWriteModel;
        NodeDescriptor(String path, String decompiled, boolean on,
                       boolean parentOn, boolean noWriteModel)
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

        boolean isOn()
        {
            return on;
        }

        boolean isParentOn()
        {
            return parentOn;
        }

        boolean isNoWriteModel()
        {
            return noWriteModel;
        }
    }

    public static void main(String args[])
    {

        int outShot = -1;
        if (args.length < 2)
        {
            System.out.println(
                "Usage: java LoadPulse <experiment> <input shot> [<output shot>]");
            System.exit(0);
        }
        if (args.length >= 2)
        {
            try
            {
                outShot = Integer.parseInt(args[2]);
            }
            catch (Exception exc)
            {
                outShot = -1;
            }
            int shot = Integer.parseInt(args[1]);
            LoadPulse lp = new LoadPulse();
            try
            {
                lp.load(args[0], shot, outShot);
            }
            catch (Exception exc)
            {
                System.out.println(exc);
            }
        }
    }

    void load(String experiment, int shot, int outShot) throws Exception
    {
        System.out.println("LOAD PULSE");
        Vector nodesV = getNodes(experiment, shot);
        try
        {
            tree = new Database(experiment, outShot);
            tree.open();
            for (int i = 0; i < nodesV.size(); i++)
            {
                NodeDescriptor currNode = (NodeDescriptor) nodesV.elementAt(i);

                try
                {
                    NidData currNid = tree.resolve(new PathData(currNode.
                        getPath()), 0);

                    //if(currNode.isNoWriteModel()) System.out.println("NO WRITE MODEL!!" + currNode.getPath());

                    if (currNode.getDecompiled() != null &&
                        !currNode.isNoWriteModel())
                    {
                        Data currData = Data.fromExpr(currNode.getDecompiled());
                        tree.putData(currNid, currData, 0);
                    }
                    if (currNode.isOn() && currNode.isParentOn())
                        tree.setOn(currNid, true, 0);
                    else if (currNode.isParentOn())
                    {
                        tree.setOn(currNid, false, 0);
                    }
                }
                catch (Exception exc)
                {
                    System.out.println("Error writing " + currNode.getPath() +
                                       " in model: " + exc);
                }
            }
            tree.close(0);
        }
        catch (Exception exc)
        {
            System.out.println("FATAL ERROR: " + exc);
        }
    }

    Vector getNodes(String experiment, int shot) throws Exception
    {
        Vector nodesV = new Vector();
        tree = new Database(experiment, shot);
        tree.open();
        BufferedReader br = new BufferedReader(new FileReader(
            confFileName));
        String basePathLine;
        String currPath = "";
        String outPath = null;
        NidData defNid = tree.getDefault(0);
        while ( (basePathLine = br.readLine()) != null)
        {
            NidData currNid;
            if(basePathLine.trim().equals("")) continue;
            System.out.println(basePathLine);
            String basePath = "";
            try
            {
                
                StringTokenizer st = new StringTokenizer(basePathLine, " ");
                basePath = st.nextToken();
                currNid = tree.resolve(new PathData(basePath), 0);
                outPath = null;
                if(st.hasMoreTokens())
                {
                    String next = st.nextToken();
                    if(next.toUpperCase().equals("STATE")) //If only state has to retrieved
                    {
                        NodeInfo currInfo = tree.getInfo(currNid, 0);
                        currPath = currInfo.getFullPath();
                         try
                        {
                            nodesV.addElement(new NodeDescriptor(currPath,
                                    null, currInfo.isOn(),
                                    currInfo.isParentOn(),
                                    currInfo.isNoWriteModel() ||
                                    currInfo.isWriteOnce()));
                        }
                        catch (Exception exc)
                        {
                            System.out.println("Error reading state of " + currPath + ": " + exc);
                        }
                        continue;
                    }
                    else  //An alternate out pathname is provided in next 
                    {
                        outPath = next.toUpperCase();
                    }
                }
                tree.setDefault(currNid, 0);
                NidData[] nidsNumeric = tree.getWild(NodeInfo.USAGE_NUMERIC, 0);
                if (nidsNumeric == null) nidsNumeric = new NidData[0];
                NidData[] nidsText = tree.getWild(NodeInfo.USAGE_TEXT, 0);
                if (nidsText == null) nidsText = new NidData[0];
                NidData[] nidsSignal = tree.getWild(NodeInfo.USAGE_SIGNAL, 0);
                if (nidsSignal == null) nidsSignal = new NidData[0];
                NidData[] nidsStruct = tree.getWild(NodeInfo.USAGE_STRUCTURE, 0);
                if (nidsStruct == null) nidsStruct = new NidData[0];

                ////Get also data from subtree root

                int addedLen;
                try
                {
                    tree.getData(currNid, 0);
                    addedLen = 1;
                }
                catch (Exception exc)
                {
                    addedLen = 0;
                }

                NidData nids[] = new NidData[nidsNumeric.length +
                    nidsText.length +
                    nidsSignal.length + nidsStruct.length + addedLen];

                if (addedLen > 0)
                    nids[nidsNumeric.length + nidsText.length +
                        nidsSignal.length + nidsStruct.length] = currNid;
                ///////////////////////

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
                    if(i == (nids.length - 1))//If IT IS the node described in LoadPulse.congf (and not any descendant)
                    {
                        if(outPath != null)
                        {
                            System.out.println(currPath + " --> "+outPath);
                            currPath = outPath;
                        }
                        else
                            System.out.println(currPath);
                    }
                    else
                       System.out.println(currPath);
                    try
                    {
                        Data currData;
                        try {
                            currData = tree.getData(nids[i], 0);
                        }catch(Exception exc) {currData = null;}
                        if (currData != null)
                        {

                            String currDecompiled = currData.toString();
                            nodesV.addElement(new NodeDescriptor(currPath,
                                currDecompiled, currInfo.isOn(),
                                currInfo.isParentOn(),
                                currInfo.isNoWriteModel() ||
                                currInfo.isWriteOnce()));
                        }
                        else
                            nodesV.addElement(new NodeDescriptor(currPath,
                                null, currInfo.isOn(), currInfo.isParentOn(),
                                currInfo.isNoWriteModel() ||
                                currInfo.isWriteOnce()));
                    }
                    catch (Exception exc)
                    {
                        nodesV.addElement(new NodeDescriptor(currPath,
                            null, currInfo.isOn(), currInfo.isParentOn(),
                            currInfo.isNoWriteModel() || currInfo.isWriteOnce()));
                    }
                }
            }
            catch (Exception exc)
            {
                System.out.println("Error reading " + basePath + ": " + exc);
            }
        }
        numPMUnits = countPMUnits();
        evaluatePCConnection();
        evaluatePVConnection();
        evaluateRTransfer();
        tree.close(0);
        br.close();
     return nodesV;
    }



    //Load setup and expands path names into abs path names with reference to pathRefShot
    public void getSetupWithAbsPath(String experiment, int shot, int pathRefShot, Hashtable setupHash, Hashtable setupOnHash) throws Exception
    {
        Hashtable currSetupHash = new Hashtable();
        getSetup(experiment, shot, currSetupHash, setupOnHash);
        try {
            Database tree = new Database(experiment, pathRefShot);
            tree.open();
            Enumeration pathNamesEn = currSetupHash.keys();
            while(pathNamesEn.hasMoreElements())
            {
                String currPath = (String)pathNamesEn.nextElement();
                try {
                    NidData currNid = tree.resolve(new PathData(currPath), 0);
                    NodeInfo currInfo = tree.getInfo(currNid, 0);
                    String currAbsPath = currInfo.getFullPath();
                    setupHash.put(currAbsPath, currSetupHash.get(currPath));
                }catch(Exception exc)
                {
                    System.out.println("LoadSetup: Cannot expand path name " + currPath + " : "+exc);
                }
            }
            tree.close(0);

        }catch(Exception exc)
        {
            System.out.println("Cannot expand path names in LoadSetup: "+exc);
        }
    }
    void getSetup(String experiment, int shot, Hashtable setupHash, Hashtable setupOnHash) throws Exception
    {
        Vector nodesV = getNodes(experiment, shot);
        int i;
        NodeDescriptor currNode;
        for (i = 0; i < nodesV.size(); i++)
        {
            currNode = (NodeDescriptor) nodesV.elementAt(i);
            String decompiled = currNode.getDecompiled();
            if(decompiled != null)
            {
                try
                {
                    setupHash.put(currNode.getPath(), currNode.getDecompiled());
                }
                catch (Exception exc)
                {
                    System.out.println(
                        "Internal error in LoadPulse.getSetup(): " + exc);
                }
            }
            try {
                if(currNode.getPath().equals("\\RFX::TOP.RFX.SETUP:TIMES:OPEN_PTSO_1"))
                {
                    int cacca = 1;
                }
                setupOnHash.put(currNode.getPath(), new Boolean(currNode.isOn()));
            }
            catch (Exception exc)
            {
                System.out.println(
                        "Internal error in LoadPulse.getSetup(): " + exc);
            }
        }
     }

     int countPMUnits()
     {
         try
         {
             NidData rootNid = tree.resolve(new PathData("\\PM_SETUP"), 0);
             NidData unitsNid = new NidData(rootNid.getInt() + 5);
             Data unitsData = tree.evaluateData(unitsNid, 0);
             String units = unitsData.toString();
             StringTokenizer st = new StringTokenizer(units, " ,\"");
             return st.countTokens();
         }
         catch (Exception exc)
         {
             System.out.println("Error getting num enabled PM: " + exc);
             return 0;
         }
     }

     void evaluatePCConnection()
     {
         try
         {
             NidData rootNid = tree.resolve(new PathData("\\PC_SETUP"), 0);
             NidData connectionNid = new NidData(rootNid.getInt() + 2);
             Data connectionData = tree.evaluateData(connectionNid, 0);
             PCconnection = connectionData.getString();
         }
         catch (Exception exc)
         {
             System.out.println("Error getting PC connection: " + exc);
         }
     }

     void evaluatePVConnection()
     {
         try
         {
             NidData rootNid = tree.resolve(new PathData("\\PV_SETUP"), 0);
             NidData connectionNid = new NidData(rootNid.getInt() + 2);
             Data connectionData = tree.evaluateData(connectionNid, 0);
             PVconnection = connectionData.getString();
         }
         catch (Exception exc)
         {
             System.out.println("Error getting PV connection: " + exc);
         }
     }

     void evaluateRTransfer()
     {
         try
         {
             NidData rootNid = tree.resolve(new PathData("\\P_CONFIG"), 0);
             NidData rTransferNid = new NidData(rootNid.getInt() + 20);
             Data rTransferData = tree.evaluateData(rTransferNid, 0);
             rTransfer = rTransferData.getFloat();
         }
         catch (Exception exc)
         {
             System.out.println("Error getting R transfer: " + exc);
         }
     }


     String getPCConnection(){return PCconnection;}
     String getPVConnection(){return PVconnection;}
     float getRTransfer(){return rTransfer;}

     int getPMUnits()
     {
         return numPMUnits;
     }
}
