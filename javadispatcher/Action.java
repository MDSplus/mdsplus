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

class Action
{
    ActionData action;
    String server_address="DUMMY";
    int nid;
    String name;
    boolean on;
    int dispatch_status;
    int status;
    int timestamp;
    boolean manual = false;
    boolean essential = false;
    static final int NOT_DISPATCHED = 1;
    static final int DISPATCHED = 2;
    static final int DOING = 3;
    static final int DONE = 4;
    static final int ABORTED = 5;

    static final int ServerNOT_DISPATCHED = 0xfe18008;
    static final int ServerINVALID_DEPENDENCY = 0xfe18012;
    static final int ServerCANT_HAPPEN = 0xfe1801a;



    public Action(ActionData action, int nid, String name, boolean on, boolean essental, String server_address)
    {
        this.action = action;
        this.nid = nid;
        this.name = name;
        this.on = on;
        dispatch_status = NOT_DISPATCHED;
        status = 0;
        this.server_address = server_address;
        this.essential = essential;
    }

  //public int getTimestamp() {return timestamp; }
    public synchronized void setTimestamp(int timestamp) {this.timestamp = timestamp; }
    public synchronized ActionData getAction() {return action; }
    public synchronized int getNid() {return nid; }
    public synchronized String getName() {return name; }
    public synchronized boolean isOn() {return on; }
    public synchronized int getDispatchStatus() {return dispatch_status; }
    public synchronized int getStatus() {return status; }
    public synchronized boolean isManual() {return manual; }
    public synchronized void setManual(boolean manual) {this.manual = manual; }
    public synchronized boolean isEssential() { return essential;}
    synchronized void setStatus(int status) {this.status = status; }

    public synchronized String getServerAddress(){return server_address;}
    public synchronized void setServerAddress(String server_address){this.server_address = server_address;}
    
    synchronized void setStatus(int dispatch_status, int status, boolean verbose)
    {
        String server;
        this.status = status;
        this.dispatch_status = dispatch_status;

        if(verbose)
        {
            try {
                server = ((DispatchData)action.getDispatch()).getIdent().getString();
            } catch(Exception e) {server = ""; }
            switch(dispatch_status) {
                case DISPATCHED : System.out.println(""+ new Date() + " Dispatching node " +
                    name + "(" + nid+")"  +  " to " + server); break;
                 case DOING : System.out.println(""+new Date() + " " +server +
                        " is beginning action " + name); break;
                 case DONE :
                        if((status & 1) != 0)
                            System.out.println(""+new Date() + " Action " +name + " completed  ");
                        else
                            System.out.println(""+new Date() + " Action " +name +
                                " failed  " + MdsHelper.getErrorString(status));
                        break;
                 case ABORTED: System.out.println(""+new Date() + " Action " +name + " aborted"); break;
            }
        }
    }
    public synchronized String toString() {return name; }
}
