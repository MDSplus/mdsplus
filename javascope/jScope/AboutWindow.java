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
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.IOException;


public class AboutWindow extends JLabel  {
    static String javaVersion = System.getProperty("java.version");
    ImageIcon io = null;

    public AboutWindow() {
        try {
            String icon_file = jScopeFacade.findFileInClassPath("jdocs/about_jscope.jpg");
            if (icon_file != null)
            io = new ImageIcon(icon_file);
            else
            io = new ImageIcon(getClass().getClassLoader().getResource("jdocs/about_jscope.jpg"));

            if (jScopeFacade.is_debug)
            System.out.println("about_jscope image path " + icon_file + io);
            if(io != null) setIcon(io);
        }catch(Exception exc){}
    }

    public void update(Graphics g) {
	paint(g);
	}

    public void paint(Graphics gReal) {
	if(io == null)
	return;

	//Double buffering
	Image imageBuffer = createImage( getWidth(), getHeight() );
	Graphics g = imageBuffer.getGraphics();

	Image image = io.getImage();

	g.drawImage( image, 1, 1, null );
	Toolkit.getDefaultToolkit().sync();

	int start  = 32+2;
	int top    = 102+1;
	int botton = 268+1;
	int delta  = 14;

	g.setColor( new Color( 128, 128, 128 ) );
	g.drawRect( -1, -1, getWidth(), getHeight() ); // makes a bevel border likeness

	g.drawString( jScopeFacade.VERSION, start, botton - 3*delta);
	g.drawString( "http://www.igi.cnr.it",  start, botton - 2*delta );
	g.drawString( "JVM used :" +  javaVersion, start, botton -  delta );

	gReal.drawImage( imageBuffer, 0, 0, this );
	}

    }
