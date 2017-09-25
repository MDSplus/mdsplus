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
package jScope;

/* $Id$ */
import java.awt.AWTEvent;
import java.awt.Event;

/**
 * ConnectionEvent instances describe the current status of the data transfer and are passed by the DataProvider
 * implementation to jScope by means of ConnectionListener.processConnectionEvent method.
 * ConnectionEvent instances can also signal a connection lost. In this case field id (inherited by AWTEvent) is set 
 * to ConnectionEvent.LOST_CONNECTION, otherwise field id should be set to 0.
 * 
 * @see Connection Listener
 * @see DataProvider
 */
public class ConnectionEvent extends AWTEvent 
{
    public static final int LOST_CONNECTION = AWTEvent.RESERVED_ID_MAX + 1;

    /**
     * Total size of the data to be transferred.
     */
    int    total_size;

    /**
     * Number ofbytes transferred so far.
     */
    int    current_size;

    /**
     * Additional string information, shown in the status bar of jScope.
     */
    String info;

    public ConnectionEvent(Object source, String info, int total_size, int current_size) 
    {
        super(source, 0);
        this.total_size   = total_size;
        this.current_size = current_size;
        this.info = info;
    }
    public String getInfo() { return info;}
    public ConnectionEvent(Object source, int total_size, int current_size) 
    {
        super(source, 0);
        this.total_size   = total_size;
        this.current_size = current_size;
        this.info = null;
    }

    public ConnectionEvent(Object source, String info) 
    {
        super(source, 0);
        this.info = new String(info);
    }

    public ConnectionEvent(Object source, int event_id, String info) 
    {
        super(source, event_id);
        this.info = new String(info);
    }
}
