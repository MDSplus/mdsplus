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

public class WaveContainerEvent extends AWTEvent 
{
    static final int START_UPDATE   = AWTEvent.RESERVED_ID_MAX + 4;
    static final int END_UPDATE     = AWTEvent.RESERVED_ID_MAX + 5;
    static final int KILL_UPDATE    = AWTEvent.RESERVED_ID_MAX + 6;
    static final int WAVEFORM_EVENT = AWTEvent.RESERVED_ID_MAX + 7;
    String info;
    AWTEvent we;

    public WaveContainerEvent (Object source, int event_id, String info) 
    {
        super(source, event_id);
        this.info = info;
    }

    public WaveContainerEvent (Object source, AWTEvent we) 
    {
        super(source, WAVEFORM_EVENT);
        this.we = we;
    }

}
