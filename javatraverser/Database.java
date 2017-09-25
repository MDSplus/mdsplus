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
//package jTraverser;

public class Database implements RemoteTree{
    String name;
    int shot;
    boolean is_open = false;
    boolean is_readonly = false;
    boolean is_editable = false;
    boolean is_realtime = false;
    static {
        try {
 //           System.loadLibrary("MdsShr");
 //           System.loadLibrary("MdsIpShr");
 //           System.loadLibrary("TreeShr");
 //           System.loadLibrary("TdiShr");
            System.loadLibrary("JavaMds");
	    }
            catch(Exception exc)
            {
              jTraverser.stderr("Cannot load library ", exc);
              exc.printStackTrace();
            }
    }
    public Database() {super();}
    public Database(String name, int shot)
    {
	this.name = name.toUpperCase();
	this.shot = shot;
    }
    public void setTree(String name, int shot)
    {
        this.name = name.toUpperCase();
        this.shot = shot;
    }

    public void setEditable(boolean editable)
    {
        this.is_editable = editable;
    }

    public void setRealtime(boolean realtime)
    {
        this.is_realtime = realtime;
    }

    public void setReadonly(boolean readonly)
    {
        this.is_readonly = readonly;
    }
    public Data evaluateData(Data data, int ctx) throws DatabaseException
    {
        return evaluateSimpleData(data, ctx);
    }
    final public String getName() {return name; }
    final public int getShot() {return shot;}
    public boolean isOpen() {return is_open; }
    public boolean isEditable() {return is_editable; }
    public boolean isReadonly() {return is_readonly;}
    public boolean isRealtime() {return is_realtime;}
   /* Low level MDS database management routines, will be  masked by the Node class*/
    public native int open() throws DatabaseException;
    public native int openNew() throws DatabaseException;
    public native void write(int ctx) throws DatabaseException;
    public native void close(int ctx)throws DatabaseException;
    public native void quit(int ctx)throws DatabaseException;
    public native Data getData(NidData nid, int ctx) throws DatabaseException;
    public native Data evaluateData(NidData nid, int ctx) throws DatabaseException;
    public native Data evaluateSimpleData(Data data, int ctx) throws DatabaseException;
    public native void putData(NidData nid, Data data, int ctx) throws DatabaseException;
    public native void putRow(NidData nid, Data data, long time, int ctx) throws DatabaseException;
    public native NodeInfo getInfo(NidData nid, int ctx) throws DatabaseException;
    public native void setTags(NidData nid, String tags[], int ctx) throws DatabaseException;
    public native String[] getTags(NidData nid, int ctx);
    public native void renameNode(NidData nid, String name, int ctx) throws DatabaseException;
    public native NidData addNode(String name, int usage, int ctx) throws DatabaseException;
    public native void setSubtree(NidData nid, int ctx) throws DatabaseException;
    public native NidData[] startDelete(NidData nid[], int ctx) throws DatabaseException;
    public native void executeDelete(int ctx) throws DatabaseException;
    public native NidData[] getSons(NidData nid, int ctx) throws DatabaseException;
    public native NidData[] getMembers(NidData nid, int ctx) throws DatabaseException;
    public native boolean isOn(NidData nid, int ctx) throws DatabaseException;
    public native void setOn(NidData nid, boolean on, int ctx) throws DatabaseException;
    public native NidData resolve(PathData pad, int ctx) throws DatabaseException;
    public native void setDefault(NidData nid, int ctx) throws DatabaseException;
    public native NidData getDefault(int ctx) throws DatabaseException;
    public native NidData addDevice(String path, String model, int ctx) throws DatabaseException;
    public native int doAction(NidData nid, int ctx) throws DatabaseException;
    public native void doDeviceMethod(NidData nid, String method, int ctx) throws DatabaseException;
    public native NidData [] getWild(int usage_mask, int ctx) throws DatabaseException;
    public native int create(int shot) throws DatabaseException;
    public String dataToString(Data data){return data.toString();}
    public Data dataFromExpr(String expr){return Data.fromExpr(expr);}
    public native long saveContext();
    public native void restoreContext(long context);
    public int getCurrentShot() { return getCurrentShot(name);}
    public native int getCurrentShot(String experiment);
    public void setCurrentShot(int shot) {setCurrentShot(name, shot);}
    public native void setCurrentShot(String experiment, int shot);
    public native String getOriginalPartName(NidData nid) throws DatabaseException;
    public native void clearFlags(NidData nid, int flags) throws DatabaseException;
    public native void setFlags(NidData nid, int flags) throws DatabaseException;
    public native int getFlags(NidData nid) throws DatabaseException;
    public native void setEvent(String event)throws DatabaseException;
    public native String getMdsMessage(int status);
  }

