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
package mds;

/* $Id$ */
import java.awt.AWTEvent;
import java.sql.Connection;

/**
 * ConnectionEvent instances describe the current status of the data transfer and are passed by the DataProvider
 * implementation to jScope by means of ConnectionListener.processConnectionEvent method.
 * ConnectionEvent instances can also signal a connection lost. In this case field id (inherited by AWTEvent) is set
 * to ConnectionEvent.LOST_CONNECTION, otherwise field id should be set to 0.
 *
 * @see Connection Listener
 */
@SuppressWarnings("serial")
public class MdsEvent extends AWTEvent{
    public static final int IDLE         = AWTEvent.RESERVED_ID_MAX;
    public static final int TRANSFER     = AWTEvent.RESERVED_ID_MAX + 1;
    public static final int HAVE_CONTEXT = AWTEvent.RESERVED_ID_MAX + 2;
    public static final int LOST_CONTEXT = AWTEvent.RESERVED_ID_MAX + 3;
    /**
     * Number of bytes transferred so far.
     */
    public final int        current_size;
    /**
     * Additional string information, shown in the status bar of jScope.
     */
    String                  info;
    /**
     * Total size of the data to be transferred.
     */
    public final int        total_size;

    public MdsEvent(final Object source){
        super(source, MdsEvent.IDLE);
        this.info = null;
        this.current_size = this.total_size = 0;
    }

    public MdsEvent(final Object source, final int total_size, final int current_size){
        super(source, MdsEvent.TRANSFER);
        this.total_size = total_size;
        this.current_size = current_size;
        this.info = null;
    }

    public MdsEvent(final Object source, final int event_id, final String info){
        super(source, event_id);
        this.info = new String(info);
        this.current_size = this.total_size = 0;
    }

    public MdsEvent(final Object source, final String info){
        super(source, 0);
        this.info = new String(info);
        this.current_size = this.total_size = 0;
    }

    public MdsEvent(final Object source, final String info, final int total_size, final int current_size){
        super(source, MdsEvent.TRANSFER);
        this.total_size = total_size;
        this.current_size = current_size;
        this.info = info;
    }

    public String getInfo() {
        return this.info;
    }
}
