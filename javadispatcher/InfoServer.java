import java.util.*;

class InfoServer implements Server 
{
    String tree;
    Database model_database;

    public InfoServer() {tree = null; }
    public InfoServer(String tree) {this.tree = tree; }
    
    public void pushAction(Action action) {}
    public Action popAction(){return null; }
    public void addServerListener(ServerListener listener){}
    public String getServerClass(){return "INFO_PROVIDER"; }
    public int getQueueLength() {return 0; }
    public void abort(boolean flush){}
    public boolean abortAction(Action action) {return false; }
    
    
    public void setTree(String tree) {this.tree = tree; }
    public void beginSequence(int shot)
    {
        try {
            model_database = new Database(tree, -1);
            model_database.open();
            //model_database.create(shot);
        }catch(Exception exc) {model_database = null; System.out.println("Error opening " + tree + " shot " + shot);}
    }
    public void endSequence(int shot)
    {
        if(model_database != null)
        try {
            model_database.close(0);
        }catch(Exception exc) {System.out.println("Error opening " + tree + " shot " + shot);}
    }
    
    public synchronized Action[] collectActions()
    {
        Action action;
        Vector action_vect = new Vector();
        int nid_int, num_actions;
        String name;
        Boolean on;
        Hashtable action_table = new Hashtable();
        
        if(model_database == null)
        {
            try {
                model_database = new Database(tree, -1);
                model_database.open();
            }catch(Exception exc){return null; }
        }
        NidData[]nids = null;
        try {
            nids = model_database.getWild(NodeInfo.USAGE_ACTION, 0);
        }catch(Exception exc) {return null; } 
        if(nids == null) return null;
        int [] nid_array = new int[nids.length];
        for(int i = num_actions = 0; i < nids.length; i++)
        {
            try{
                if(!model_database.isOn(nids[i], 0))
                    continue;
                NodeInfo info = model_database.getInfo(nids[i], 0);
                //check dispatch and task fields
                ActionData action_data = (ActionData)model_database.getData(nids[i], 0);
                if(action_data.getDispatch() == null || action_data.getTask() == null)
                    continue;
                action = new Action(action_data, nids[i].getInt(), info.getFullPath(), 
                    model_database.isOn(nids[i], 0));
                action_vect.addElement(action);
                nid_array[num_actions] = nids[i].getInt();
                action_table.put(new Integer(nids[i].getInt()), action_data);
                num_actions++;
            }catch(Exception exc){}
        }
        Action actions[] = new Action[action_vect.size()];
        action_vect.copyInto(actions);
        
//Now nids and paths needs to be resolved by substituting them with ActionData
        for(int i = 0; i < num_actions; i++)
            try{
                traverseAction((DispatchData)actions[i].getAction().getDispatch(), action_table); 
            }catch(Exception exc){}
//System.out.println("End collectAction()");
        
        return actions;
    }
    
    
    protected Data traverseAction(Data data, Hashtable action_table)
    {
        ActionData action_d;
        if(data instanceof NidData)
        {
            try {
                action_d = (ActionData)action_table.get(new Integer(data.getInt()));
            }catch(Exception exc) {return data; }
            if(action_d == null)
                return data;
            else
                return action_d;
        }
        if(data instanceof PathData)
        {
            NidData nid = null;
            try {
                nid = model_database.resolve((PathData)data, 0);
                action_d = (ActionData)action_table.get(new Integer(nid.getInt()));
            }catch(Exception exc) {return data; }
            if(action_d == null)
                return data;
            else
                return action_d;
        }
        if(data instanceof CompoundData)
        {
            Data [] descs =((CompoundData)data).getDescs();
            for(int i = 0; i < descs.length; i++)
                descs[i] = traverseAction(descs[i], action_table);
            return data;
        }
        return data;
    }
    public boolean isActive() {return true; }
}
        
            