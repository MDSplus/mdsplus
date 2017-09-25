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
import java.rmi.*;
import java.rmi.RemoteException.*;

public interface RemoteTree extends Remote {
    public void setTree(String experiment, int shot) throws RemoteException;
    public String getName() throws RemoteException;
    public int getShot() throws RemoteException;
    boolean isOpen() throws RemoteException;
    boolean isEditable() throws RemoteException;
    boolean isReadonly() throws RemoteException;
    boolean isRealtime() throws RemoteException;
    void setEditable(boolean editable)throws RemoteException;
    void setReadonly(boolean readonly)throws RemoteException;
    void setRealtime(boolean realtime)throws RemoteException;
   /* Low level MDS database management routines, will be  masked by the Node class*/
    public int open() throws RemoteException, DatabaseException;
    public void write(int ctx) throws RemoteException, DatabaseException;
    public void close(int ctx) throws RemoteException, DatabaseException;
    public void quit(int ctx) throws RemoteException, DatabaseException;
    public Data getData(NidData nid, int ctx)  throws RemoteException, DatabaseException;
    public Data evaluateData(NidData nid, int ctx)  throws RemoteException, DatabaseException;
    public Data evaluateData(Data data, int ctx)  throws RemoteException, DatabaseException;
    public void putData(NidData nid, Data data, int ctx)  throws RemoteException, DatabaseException;
    public void putRow(NidData nid, Data data, long time, int ctx)  throws RemoteException, DatabaseException;
    //public native DatabaseInfo getInfo(); throws DatabaseException;
    public NodeInfo getInfo(NidData nid, int ctx)  throws RemoteException, DatabaseException;
    public void clearFlags(NidData nid, int flags)  throws RemoteException, DatabaseException;
    public void setFlags(NidData nid, int flags)  throws RemoteException, DatabaseException;
    public int getFlags(NidData nid)  throws RemoteException, DatabaseException;
    public void setTags(NidData nid, String tags[], int ctx) throws RemoteException, DatabaseException;
    public String[] getTags(NidData nid, int ctx) throws RemoteException, DatabaseException;
    public void renameNode(NidData nid, String name, int ctx) throws RemoteException, DatabaseException;
    public void setSubtree(NidData nid,  int ctx) throws RemoteException, DatabaseException;
    public NidData addNode(String name, int usage, int ctx) throws RemoteException, DatabaseException;
    public NidData[] startDelete(NidData nid[], int ctx) throws RemoteException, DatabaseException;
    public void executeDelete(int ctx) throws RemoteException, DatabaseException;
    public NidData[] getSons(NidData nid, int ctx) throws RemoteException, DatabaseException;
    public NidData[] getMembers(NidData nid, int ctx) throws RemoteException, DatabaseException;
    public boolean isOn(NidData nid, int ctx) throws RemoteException, DatabaseException;
    public void setOn(NidData nid, boolean on, int ctx) throws RemoteException, DatabaseException;
    public NidData resolve(PathData pad, int ctx) throws RemoteException, DatabaseException;
    public void setDefault(NidData nid, int ctx) throws RemoteException, DatabaseException;
    public NidData getDefault(int ctx) throws RemoteException, DatabaseException;
    public NidData addDevice(String path, String model, int ctx) throws RemoteException, DatabaseException;
    public int doAction(NidData nid, int ctx) throws RemoteException, DatabaseException;
    public void doDeviceMethod(NidData nid, String method, int ctx) throws RemoteException, DatabaseException;
    public NidData [] getWild(int usage_mask, int ctx) throws RemoteException, DatabaseException;
    public int create(int shot) throws RemoteException, DatabaseException;
    public String dataToString(Data data)throws RemoteException;
    public Data dataFromExpr(String expr)throws RemoteException;
    public int getCurrentShot() throws RemoteException;
    public int getCurrentShot(String experiment)throws RemoteException;
    public void setCurrentShot(int shot)throws RemoteException;
    public void setCurrentShot(String experiment, int shot)throws RemoteException;
    public void setEvent(String event)throws RemoteException, DatabaseException;
}
