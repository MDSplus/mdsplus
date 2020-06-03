import java.util.*;

class InfoServer implements Server
{
	String tree;
	int shot = -1;
	static MDSplus.Tree model_database;

	public InfoServer() {tree = null; }
	public InfoServer(final String tree) {this.tree = tree; }

	@Override
	public void pushAction(final Action action) {}
	@Override
	public Action popAction(){return null; }
	@Override
	public void addServerListener(final ServerListener listener){}
	@Override
	public String getServerClass(){return "INFO_PROVIDER"; }
	@Override
	public int getQueueLength() {return 0; }
	@Override
	public int getDoingAction() {return 0; }
	@Override
	public void abort(final boolean flush){}
	@Override
	public boolean abortAction(final Action action) {return false; }

	public static MDSplus.Tree getDatabase() {return model_database;}
	@Override
	public void setTree(final String tree)
	{
		this.tree = tree;
	}

	@Override
	public void setTree(final String tree, final int shot)
	{
		this.tree = tree;
		this.shot = shot;
	}
	@Override
	public void beginSequence(final int shot)
	{
		System.out.println("InfoServer: beginSequence...");
		try {
			model_database = new MDSplus.Tree(tree, -1);
			model_database.createPulse(shot);
		}catch(final Exception exc) {model_database = null; System.out.println("Error opening " + tree + " shot " + shot + ": " + exc);}
		System.out.println("InfoServer: beginSequence terminated");
	}
	@Override
	public void endSequence(final int shot)
	{
		if(model_database != null)
			try {
				model_database.close();
				model_database = null;
			}catch(final Exception exc) {System.out.println("Error closing " + tree + " shot " + shot + "\n" + exc);}
	}

	@Override
	public Action[] collectActions(final String rootPath)
	{
		if(model_database == null)
		{
			try {
				model_database = new MDSplus.Tree(tree, shot);
			}catch(final Exception exc){return null; }
		}
		try {
			final MDSplus.TreeNode rootNid = model_database.getNode(rootPath);
			final MDSplus.TreeNode prevDef = model_database.getDefault();
			model_database.setDefault(rootNid);
			final Action [] actions = collectActions();
			model_database.setDefault(prevDef);
			return actions;
		}catch(final Exception exc){return null; }
	}


	@Override
	public synchronized Action[] collectActions()
	{
		Action action;
		final Vector<Action> action_vect = new Vector<Action>();
		int num_actions;
		final Hashtable<Integer, MDSplus.Action> action_table = new Hashtable<Integer, MDSplus.Action>();

		if(model_database == null)
		{
			try {
				model_database = new MDSplus.Tree(tree, shot);
			}catch(final Exception exc){return null; }
		}
		MDSplus.TreeNodeArray nids = null;
		try {
			nids = model_database.getNodeWild("***", 1 << MDSplus.Tree.TreeUSAGE_ACTION);
		}catch(final Exception exc) {return null; }
		if(nids == null) return null;
		final int [] nid_array = new int[nids.size()];
		for(int i = num_actions = 0; i < nids.size(); i++)
		{
			MDSplus.TreeNode nid = null;
			try{
				nid = nids.getElementAt(i);
				if(!nid.isOn())
					continue;
				//check dispatch and task fields
				final MDSplus.Action action_data = (MDSplus.Action)nid.getData();
				if(action_data.getDispatch() == null || action_data.getTask() == null)
					continue;

				action = new Action(action_data, nid.getNid(), nid.getFullPath(),
						nid.isOn(), nid.isEssential(), null );
				action_vect.addElement(action);
				nid_array[num_actions] = nid.getNid();
				action_table.put(new Integer(nid.getNid()), action_data);
				num_actions++;
			}catch(final Exception exc)
			{
				System.out.println(nid);
				System.out.println(exc);
			}
		}
		final Action actions[] = new Action[action_vect.size()];
		action_vect.copyInto(actions);

		//Now nids and paths needs to be resolved by substituting them with ActionData
		for(int i = 0; i < num_actions; i++)
			try{
				traverseAction(actions[i].getAction().getDispatch(), action_table);
			}catch(final Exception exc){}
		//System.out.println("End collectAction()");

		return actions;
	}


	protected MDSplus.Data traverseAction(final MDSplus.Data data, final Hashtable<Integer, MDSplus.Action> action_table)
	{
		MDSplus.Action action_d;
		if(data instanceof MDSplus.TreeNode)
		{
			try {
				action_d = action_table.get(new Integer(data.getInt()));
			}catch(final Exception exc) {return data; }
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
			}catch(final Exception exc) {return data; }
			if(action_d == null)
				return data;
			else
				return action_d;
		}
		if(data instanceof MDSplus.Dispatch)
		{
			final MDSplus.Dispatch dispData = (MDSplus.Dispatch)data;
			dispData.setIdent(traverseAction(dispData.getIdent(), action_table));
			dispData.setPhase(traverseAction(dispData.getPhase(), action_table));
			if(dispData.getOpcode() != MDSplus.Dispatch.SCHED_SEQ)
				dispData.setWhen(traverseAction(dispData.getWhen(), action_table));
			dispData.setCompletion(traverseAction(dispData.getCompletion(), action_table));
			return data;
		}
		if(data instanceof MDSplus.Compound)
		{
			final MDSplus.Data [] descs =((MDSplus.Compound)data).getDescs();
			for(int i = 0; i < descs.length; i++)
				descs[i] = traverseAction(descs[i], action_table);
			return data;
		}
		return data;
	}
	@Override
	public boolean isActive() {return true; }
	@Override
	public boolean isReady() {return true; }
}

