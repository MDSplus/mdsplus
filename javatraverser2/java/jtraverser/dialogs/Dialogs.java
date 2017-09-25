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
package jtraverser.dialogs;

import java.awt.Component;
import java.awt.Point;
import java.awt.Window;
import javax.swing.JOptionPane;
import jtraverser.TreeManager;

public final class Dialogs{
    public static final void setLocation(final Window window) {
        Component parent = window.getParent();
        if(parent == null) parent = JOptionPane.getRootFrame();
        if(parent == null) return;
        final Point parloc = parent.getLocation();
        window.setLocation(parloc.x + 32, parloc.y + 32);
    }
    public final AddNodeDialog addNode;
    public final FlagsDialog   modifyFlags;

    public Dialogs(final TreeManager treeman){
        this.modifyFlags = new FlagsDialog(treeman);
        this.addNode = new AddNodeDialog();
    }

    public final void update() {
        this.modifyFlags.update();
    }
}
