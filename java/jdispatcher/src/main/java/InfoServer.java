import java.util.*;

class InfoServer implements Server
{
    String tree;
    int shot = -1;
    static MDSplus.Tree model_database;

    public InfoServer() {tree = null; }
    public InfoServer(String tree) {this.tree = tree; }

    public void pushAction(Action action) {}
    public Action popAction(){return null; }
    public void addServerListener(ServerListener listener){}
    public String getServerClass(){return "INFO_PROVIDER"; }
    public int getQueueLength() {return 0; }
    public int getDoingAction() {return 0; }
    public void abort(boolean flush){}
    public boolean abortAction(Action action) {return false; }

    public static MDSplus.Tree getDatabase() {return model_database;}
    public void setTree(String tree)
    {
	this.tree = tree;
    }

    public void setTree(String tree, int shot)
    {
	this.tree = tree;
	this.shot = shot;
    }
    public void beginSequence(int shot)
    {
      System.out.println("InfoServer: beginSequence...");
	try {
	    model_database = new MDSplus.Tree(tree, -1);
	    model_database.createPulse(shot);
	}catch(Exception exc) {model_database = null; System.out.println("Error opening " + tree + " shot " + shot + ": " + exc);}
	System.out.println("InfoServer: beginSequence terminated");
      }
    public void endSequence(int shot)
    {
	if(model_database != null)
	try {
	    model_database.close();
	    model_database = null;
	}catch(Exception exc) {System.out.println("Error closing " + tree + " shot " + shot + "\n" + exc);}
    }

    public Action[] collectActions(String rootPath)
    {
	if(model_database == null)
	{
	    try {
	        model_database = new MDSplus.Tree(tree, shot);
	    }catch(Exception exc){return null; }
	}
	try {
	    MDSplus.TreeNode rootNid = model_database.getNode(rootPath);
	    MDSplus.TreeNode prevDef = model_database.getDefault();
	    model_database.setDefault(rootNid);
	    Action [] actions = collectActions();
	    model_database.setDefault(prevDef);
	    return actions;
	}catch(Exception exc){return null; }
    }


    public synchronized Action[] collectActions()
    {
	Action action;
	Vector<Action> action_vect = new Vector<Action>();
	int nid_int, num_actions;
	String name;
	Boolean on;
	Hashtable<Integer, MDSplus.Action> action_table = new Hashtable<Integer, MDSplus.Action>();

	if(model_database == null)
	{
	    try {
	        model_database = new MDSplus.Tree(tree, shot);
	    }catch(Exception exc){return null; }
	}
	MDSplus.TreeNodeArray nids = null;
	try {
	    nids = model_database.getNodeWild("***", 1 << MDSplus.Tree.TreeUSAGE_ACTION);
	}catch(Exception exc) {return null; }
	if(nids == null) return null;
	int [] nid_array = new int[nids.size()];
	for(int i = num_actions = 0; i < nids.size(); i++)
	{
            MDSplus.TreeNode nid = null;
	    try{
                nid = nids.getElementAt(i);
	        if(!nid.isOn())
	            continue;
	        //check dispatch and task fields
	        MDSplus.Action action_data = (MDSplus.Action)nid.getData();
	        if(action_data.getDispatch() == null || action_data.getTask() == null)
	            continue;

	        action = new Action(action_data, nid.getNid(), nid.getFullPath(),
	            nid.isOn(), nid.isEssential(), null );
	        action_vect.addElement(action);
	        nid_array[num_actions] = nid.getNid();
	        action_table.put(new Integer(nid.getNid()), action_data);
	        num_actions++;
	    }catch(Exception exc)
            {
                System.out.println(nid);
                System.out.println(exc);
            }
	}
	Action actions[] = new Action[action_vect.size()];
	action_vect.copyInto(actions);

//Now nids and paths needs to be resolved by substituting them with ActionData
	for(int i = 0; i < num_actions; i++)
	    try{
	        traverseAction((MDSplus.Dispatch)actions[i].getAction().getDispatch(), action_table);
	    }catch(Exception exc){}
//System.out.println("End collectAction()");

	return actions;
    }


    protected MDSplus.Data traverseAction(MDSplus.Data data, Hashtable<Integer, MDSplus.Action> action_table)
    {
	MDSplus.Action action_d;
	if(data instanceof MDSplus.TreeNode)
	{
	    try {
	        action_d = action_table.get(new Integer(data.getInt()));
	    }catch(Exception exc) {return data; }
	    if(action_d == null)
	        return data;
	    else
	        return action_d;
	}
	if(data instanceof MDSplus.TreePath)
	{
	    MDSplus.TreeNode nid = null;
	    try {
	        nid = model_database.getNode((MDSplus.TreePath)data);
	        action_d = action_table.get(new Integer(nid.getInt()));
	    }catch(Exception exc) {return data; }
	    if(action_d == null)
	        return data;
	    else
	        return action_d;
	}
	if(data instanceof MDSplus.Dispatch)
	{
	    MDSplus.Dispatch dispData = (MDSplus.Dispatch)data;
	    dispData.setIdent(traverseAction(dispData.getIdent(), action_table));
	    dispData.setPhase(traverseAction(dispData.getPhase(), action_table));
	    if(dispData.getOpcode() != MDSplus.Dispatch.SCHED_SEQ)
	        dispData.setWhen(traverseAction(dispData.getWhen(), action_table));
	    dispData.setCompletion(traverseAction(dispData.getCompletion(), action_table));
	    return data;
	}
	if(data instanceof MDSplus.Compound)
	{
	    MDSplus.Data [] descs =((MDSplus.Compound)data).getDescs();
	    for(int i = 0; i < descs.length; i++)
	        descs[i] = traverseAction(descs[i], action_table);
	    return data;
	}
	return data;
    }
    public boolean isActive() {return true; }
    public boolean isReady() {return true; }
}

