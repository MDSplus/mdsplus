import java.rmi.*;
import java.rmi.server.*;
import java.util.*;

class TreeServer extends UnicastRemoteObject implements RemoteTree
{
    String experiment;
    int shot;
    int lastCtx = 0;
    transient Database tree;
    Vector contexts = new Vector();
    public TreeServer() throws RemoteException
    {
        super();
    }
    public void setTree(String experiment, int shot) throws RemoteException
    {
        tree = new Database(experiment, shot);
    }

    public void setEditable(boolean editable) {tree.setEditable(editable);}
    public void setReadonly(boolean readonly) {tree.setReadonly(readonly);}
    public void setRealtime(boolean realtime) {tree.setRealtime(realtime);}
    public String getName() {return tree.getName(); }
    public int getShot(){return  tree.getShot();}
    public boolean isOpen() {return tree.isOpen();}
    public boolean isEditable(){return tree.isEditable();}
    public boolean isReadonly() {return tree.isReadonly();}
    public boolean isRealtime() {return tree.isRealtime();}
   /* Low level MDS database management routines, will be  masked by the Node class*/
    public int open() throws DatabaseException
    {
        jTraverser.stdout("Server: start open lastCtx = "+lastCtx);
        tree.open();
        contexts.insertElementAt(new Long(tree.saveContext()), lastCtx);
        return lastCtx++;
    }
    public void write(int ctx) throws DatabaseException
    {
        long lctx = ((Long)contexts.elementAt(ctx)).longValue();
        tree.restoreContext(lctx);
        contexts.setElementAt(new Long(0), ctx);
        tree.write(0);
        lctx = tree.saveContext();
        contexts.setElementAt(new Long(lctx), ctx);
    }
    public void close(int ctx) throws DatabaseException
    {
        long lctx = ((Long)contexts.elementAt(ctx)).longValue();
        tree.restoreContext(lctx);
        contexts.setElementAt(new Long(0), ctx);
        tree.close(0);
        lctx = tree.saveContext();
        contexts.setElementAt(new Long(lctx), ctx);
    }
    public void quit(int ctx) throws DatabaseException
    {
        long lctx = ((Long)contexts.elementAt(ctx)).longValue();
        tree.restoreContext(lctx);
        contexts.setElementAt(new Long(0), ctx);
        tree.quit(0);
        lctx = tree.saveContext();
        contexts.setElementAt(new Long(lctx), ctx);
    }

    private void setContext(int ctx)
    {
        long lctx = ((Long)contexts.elementAt(ctx)).longValue();
        tree.restoreContext(lctx);
        lctx = tree.saveContext();
        contexts.setElementAt(new Long(lctx), ctx);
    }


    public Data getData(NidData nid, int ctx) throws DatabaseException
    {
        setContext(ctx);
        Data data =  tree.getData(nid, 0);
        return data;
    }
    public Data evaluateData(NidData nid, int ctx) throws DatabaseException
    {
        setContext(ctx);
        Data data = tree.evaluateData(nid, 0);
        return data;
    }
    public Data evaluateData(Data inData, int ctx) throws DatabaseException
    {
        setContext(ctx);
        Data data = tree.evaluateData(inData, 0);
        return data;
    }
    public void putData(NidData nid, Data data, int ctx) throws DatabaseException
    {
        setContext(ctx);
        tree.putData(nid, data, 0);
    }

    public void putRow(NidData nid, Data data, long time, int ctx)  throws DatabaseException
    {
        setContext(ctx);
        tree.putRow(nid, data, time, 0);
    }

    //public native DatabaseInfo getInfo(); throws DatabaseException;
    public NodeInfo getInfo(NidData nid, int ctx) throws DatabaseException
    {
        setContext(ctx);
        NodeInfo info = tree.getInfo(nid, 0);
        return info;
    }
    public void clearFlags(NidData nid, int flags) throws DatabaseException
    {
        tree.clearFlags(nid, flags);
    }
    public void setFlags(NidData nid, int flags) throws DatabaseException
    {
        tree.setFlags(nid, flags);
    }
    public int getFlags(NidData nid) throws DatabaseException
    {
        return tree.getFlags(nid);
    }
    public void setTags(NidData nid, String tags[], int ctx) throws DatabaseException
    {
        setContext(ctx);
        tree.setTags(nid, tags, 0);
    }
    public String[] getTags(NidData nid, int ctx)throws DatabaseException
    {
        setContext(ctx);
        String tags[] = tree.getTags(nid, 0);
        return tags;
    }
    public void renameNode(NidData nid, String name, int ctx)throws DatabaseException
    {
        setContext(ctx);
        tree.renameNode(nid, name, 0);
    }
    public NidData addNode(String name, int usage, int ctx) throws DatabaseException
    {
        setContext(ctx);
        NidData nid = tree.addNode(name, usage, 0);
        return nid;
    }
    public void setSubtree(NidData nid, int ctx) throws DatabaseException
    {
        setContext(ctx);
        tree.setSubtree(nid, 0);
    }
    public NidData[] startDelete(NidData nid[], int ctx)throws DatabaseException
    {
        setContext(ctx);
        NidData nids[] = tree.startDelete(nid, 0);
        return nids;
    }
    public void executeDelete(int ctx)throws DatabaseException
    {
        setContext(ctx);
        tree.executeDelete(0);
    }
    public NidData[] getSons(NidData nid, int ctx) throws DatabaseException
    {
        setContext(ctx);
        NidData [] sons = tree.getSons(nid, 0);
        return sons;
    }

    public NidData[] getMembers(NidData nid, int ctx) throws DatabaseException
    {
        setContext(ctx);
        NidData members[] = tree.getMembers(nid, 0);
        return members;
    }
    public boolean isOn(NidData nid, int ctx) throws DatabaseException
    {
        setContext(ctx);
        boolean on = tree.isOn(nid, 0);
        return on;
    }
    public void setOn(NidData nid, boolean on, int ctx) throws DatabaseException
    {
        setContext(ctx);
        tree.setOn(nid, on, 0);
    }
    public NidData resolve(PathData pad, int ctx) throws DatabaseException
    {
        setContext(ctx);
        NidData nid = tree.resolve(pad, 0);
        return nid;
    }
    public void setDefault(NidData nid, int ctx)throws DatabaseException
    {
        long lctx = ((Long)contexts.elementAt(ctx)).longValue();
        tree.restoreContext(lctx);
        tree.setDefault(nid, 0);
        lctx = tree.saveContext();
        contexts.setElementAt(new Long(lctx), ctx);
    }
    public NidData getDefault(int ctx)throws DatabaseException
    {
        setContext(ctx);
        NidData nid = tree.getDefault(0);
        return nid;
    }
    public NidData addDevice(String path, String model, int ctx)throws DatabaseException
    {
        setContext(ctx);
        NidData nid = tree.addDevice(path, model, 0);
        return nid;
    }
    public int doAction(NidData nid, int ctx) throws DatabaseException
    {
        setContext(ctx);
        return tree.doAction(nid, 0);
    }
    public void doDeviceMethod(NidData nid, String method, int ctx) throws DatabaseException
    {
        setContext(ctx);
        tree.doDeviceMethod(nid, method, 0);
    }
    public NidData [] getWild(int usage_mask, int ctx) throws DatabaseException
    {
        setContext(ctx);
        NidData nids[] = tree.getWild(usage_mask, 0);
        return nids;
    }
    public int create(int shot) throws DatabaseException
    {
        tree.create(shot);
        contexts.insertElementAt(new Long(tree.saveContext()), lastCtx);
        return lastCtx++;
    }
    public String dataToString(Data data){return data.toString();}
    public Data dataFromExpr(String expr){return Data.fromExpr(expr);}

    public int getCurrentShot() throws RemoteException
    {
        return tree.getCurrentShot();
    }
    public int getCurrentShot(String experiment)throws RemoteException
    {
        return tree.getCurrentShot(experiment);
    }

    public void setCurrentShot(int shot)throws RemoteException
    {
        tree.setCurrentShot(shot);
    }

    public void setCurrentShot(String experiment, int shot)
    {
        tree.setCurrentShot(experiment, shot);
    }
    public void setEvent(String event)throws DatabaseException
    {
        tree.setEvent(event);
    }

    public static void main(String args[])
    {
       // if (System.getSecurityManager() == null)
            System.setSecurityManager(new RMISecurityManager()
                {
                    public void checkLink(String lib){}
                });
        RemoteTree server = null;
        try {
            server = new TreeServer();
        }catch(Exception exc){
            jTraverser.stderr("Error creating TreeServer", exc);
            System.exit(0);
        }
        try {Naming.rebind("TreeServer", server);}
        catch(Exception exc) {jTraverser.stderr("Error in Naming.rebind", exc);}
    }
}

