/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
import java.util.*;

class InfoServer implements Server
{
    String tree;
    int shot = -1;
    static Database model_database;

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

    public static Database getDatabase() {return model_database;}
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
            model_database = new Database(tree, -1);
            model_database.open();
            model_database.create(shot);
        }catch(Exception exc) {model_database = null; System.out.println("Error opening " + tree + " shot " + shot + ": " + exc);}
        System.out.println("InfoServer: beginSequence terminated");
      }
    public void endSequence(int shot)
    {
        if(model_database != null)
        try {
            model_database.close(0);
            model_database = null;
        }catch(Exception exc) {System.out.println("Error closing " + tree + " shot " + shot + "\n" + exc);}
    }

    public Action[] collectActions(String rootPath)
    {
        if(model_database == null)
        {
            try {
                model_database = new Database(tree, shot);
                model_database.open();
            }catch(Exception exc){return null; }
        }
        try {
            NidData rootNid = model_database.resolve(new PathData(rootPath), 0);
            NidData prevDef = model_database.getDefault(0);
            model_database.setDefault(rootNid, 0);
            Action [] actions = collectActions();
            model_database.setDefault(prevDef, 0);
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
        Hashtable<Integer, ActionData> action_table = new Hashtable<Integer, ActionData>();

        if(model_database == null)
        {
            try {
                model_database = new Database(tree, shot);
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
                    model_database.isOn(nids[i], 0), info.isEssential(), null );
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


    protected Data traverseAction(Data data, Hashtable<Integer, ActionData> action_table)
    {
        ActionData action_d;
        if(data instanceof NidData)
        {
            try {
                action_d = action_table.get(new Integer(data.getInt()));
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
                action_d = action_table.get(new Integer(nid.getInt()));
            }catch(Exception exc) {return data; }
            if(action_d == null)
                return data;
            else
                return action_d;
        }
        if(data instanceof DispatchData)
        {
            DispatchData dispData = (DispatchData)data;
            dispData.descs[0] = traverseAction(dispData.descs[0], action_table);
            dispData.descs[1] = traverseAction(dispData.descs[1], action_table);
            if(dispData.getType() != DispatchData.SCHED_SEQ)
                dispData.descs[2] = traverseAction(dispData.descs[2], action_table);
            dispData.descs[3] = traverseAction(dispData.descs[3], action_table);
            return data;
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
    public boolean isReady() {return true; }
}

