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
interface Server
/**
Abstracts Server functionality
*/
{

    public void pushAction(Action action);
    /** Inserts a new action in the waiting queue
    */
    public Action popAction();
    /** Removes the lastly inserted action form the waiting queue.
    If no such action is found return null
    */

    public Action[] collectActions(String rootPath);
    /** Collects action information to be used by the dispatcher, starting from specified subtree
    */
    public Action[] collectActions();
    /** Collects action information to be used by the dispatcher
    */
    public void beginSequence(int shot);
    /** Handles the beginning of a sequence (e.g. for opening trees)
    */
    public void endSequence(int shot);
    /** Handles the termination of the sequence (e.g. for closing trees)
    */
    public void addServerListener(ServerListener listener);
    /** Add a new listener for actions handled by the server
    */
    public String getServerClass();
    /**
    returns the classes to which the server belongs
    */
    public int getQueueLength();
    /**
    returns the number of waiting actions
    */
    
    public int getDoingAction();
    /**
    returns the number of doing actions 
    */

    
    public void abort(boolean flush);
    /**
    aborts action currently being executed. If flush, empties waiting queue;
    */
    public boolean abortAction(Action action);
    /**
    aborts selected action
    */

   public boolean isActive();
   /**
    * true if server actively participating to dispatching
    */

   public boolean isReady();
   /**
    * true if server can participate to the next shot
    */

   public void setTree(String tree);

   public void setTree(String tree, int shot);


}

