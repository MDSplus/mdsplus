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

class Balancer implements ServerListener
/**
Ensures action dispatching to servers, keeping load balancing.
*/
{
    Hashtable<String, Vector<Server>> servers = new Hashtable<String, Vector<Server>>();
    /**
    indexed by server class. Stores the vector of servers associated with the server class.
    */
    Server default_server = null;


    public synchronized void setDefaultServer(Server server)
    {
        default_server = server;
    }

    public synchronized void addServer(Server server)
    {
        Vector<Server> server_vect = servers.get(server.getServerClass().toUpperCase());
        if(server_vect == null)
        {
            server_vect = new Vector<Server>();
            servers.put(server.getServerClass().toUpperCase(), server_vect);
        }
        server_vect.addElement(server);
        server.addServerListener(this);
    }
    
    public String getActServer(String serverClass)
    {
        Vector<Server> serverV = servers.get(serverClass.toUpperCase());
        if(serverV == null || serverV.size() == 0)
            return default_server.getServerClass();
        return serverClass;
    }
    
    public boolean enqueueAction(Action action)
    {
        String server_class;
        try{
            server_class = ((DispatchData)action.getAction().getDispatch()).getIdent().getString();
        }
        catch(Exception exc)
        {
            if(default_server != null)
            {
                default_server.pushAction(action);
                return true;
            }
            return false;
        }


        Vector<Server> server_vect = new Vector<Server>();
        Vector<Server> all_server_vect = servers.get(server_class.toUpperCase());
         if(all_server_vect == null)
         {
             if(default_server != null)
             {
                 default_server.pushAction(action);
                 return true;
             }
             else
                 return false;
         } else {
              for (int i = 0; i < all_server_vect.size(); i++) {
                  Server curr_server = all_server_vect.elementAt(i);
                  if (curr_server.isReady())
                      server_vect.addElement(curr_server);
              }
              if (server_vect.size() == 0) {
                  if (default_server != null) {
                      default_server.pushAction(action);
                      return true;
                  }
                  else
                      return false;
              }
         }
     /////////////////////////////

        Enumeration<Server> server_list = server_vect.elements();
        int curr_load, min_load = 1000000;
        Server curr_server, min_server = null;
        while(server_list.hasMoreElements())
        {
            curr_server = server_list.nextElement();
            curr_load = curr_server.getQueueLength();
            if(curr_load == 0 && curr_server.isActive())
            {
                curr_server.pushAction(action);
                return true;
            }
            if(curr_load < min_load && curr_server.isActive())
            {
                min_load = curr_load;
                min_server = curr_server;
            }
        }
        if(min_server != null)
            min_server.pushAction(action);
        else if(default_server != null)//try to send to default server
            default_server.pushAction(action);
        else
        //all servers inactive (no mdsip connection): send action to either server, it will be aborted
            ((ActionServer)server_vect.elementAt(0)).pushAction(action);
        return true;
    }

    public void actionStarting(ServerEvent event){}
    public void actionAborted(ServerEvent event){}
    public synchronized void actionFinished(ServerEvent event)
    {
        String server;
        try{
            server = ((DispatchData)event.getAction().getAction().getDispatch()).getIdent().getString();
        }catch(Exception exc) {return; }
        Vector<Server> server_vect = servers.get(server.toUpperCase());
        if(server_vect == null) //it is the default server
            return;
        if(!isBalanced(server_vect))
        {
            Server min_loaded = null, max_loaded = null;
            int max_load = 0;
            Enumeration<Server> server_list = server_vect.elements();
            while (server_list.hasMoreElements())
            {
                Server curr_server = server_list.nextElement();
                int curr_len = curr_server.getQueueLength();
                if(curr_len == 0)
                    min_loaded = curr_server;
                if(curr_len > max_load)
                {
                    max_load = curr_len;
                    max_loaded = curr_server;
                }
            }
            Action action = max_loaded.popAction();
            if(action != null)
            {
                System.out.println("ACTION BALANCING: action " + action.getName()+" transferred between two equivalent servers");
                min_loaded.pushAction(action);
            }
        }
    }

    protected boolean isBalanced(Vector<Server> server_vect)
    {
        if(server_vect.size() <= 1) return true; //No load balancing if one server in the server class
        Enumeration<Server> server_list = server_vect.elements();
        int min_load = 1000000, max_load = 0;
        while(server_list.hasMoreElements())
        {
            Server curr_server = server_list.nextElement();
            if(!curr_server.isActive()) continue;
            int curr_load = curr_server.getQueueLength();
            if(curr_load < min_load) min_load = curr_load;
            if(curr_load > max_load) max_load = curr_load;
        }
        return !(min_load == 0 && max_load > 1);
    }

    public void abort()
    {
        Enumeration<Vector<Server>> server_vects = servers.elements();
        while(server_vects.hasMoreElements())
        {
            Vector<Server> server_vect = server_vects.nextElement();
            Enumeration<Server> server_list = server_vect.elements();
            while(server_list.hasMoreElements())
                server_list.nextElement().abort(true);
        }
    }

   public void abortAction(Action action)
   {
        Enumeration server_vects = servers.elements();
        while(server_vects.hasMoreElements())
        {
            Vector server_vect = (Vector)server_vects.nextElement();
            Enumeration server_list = server_vect.elements();
            while(server_list.hasMoreElements())
            {
                Server curr_server = (Server)server_list.nextElement();
                if(curr_server.abortAction(action))
                    return;
            }
        }
    }

    public void disconnected(ServerEvent event){}
    public void connected(ServerEvent event){}

   
   
}
