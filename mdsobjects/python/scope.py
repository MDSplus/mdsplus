# 
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

_data=_mimport('mdsdata')

class Scope(object):
    """The Scope class utilizes the jScope java implementation to generate plots of MDSplus data"""

    def __init__(self,title='',x=100,y=100,width=400,height=300):
        """Initialize a Scope instance
        @param title: Title of scop window
        @type title: string
        @param x: X location on screen of scope window in pixels from left edge
        @type x: int
        @param y: Y location on screen of scope window in pixels from the top edge
        @type y: int
        @param width: Width of the scope window in pixels
        @type width: int
        @param height: Height of the scope window in pixels
        @type height: int
        @rtype: None
        """
        self.idx=_data.Data.execute("JavaNewWindow($,-1)",title)
        self.x=x
        self.y=y
        self.width=width
        self.height=height

    def show(self):
        """Show the scope window
        @rtype: None
        """
        _data.Data.execute("JavaShowWindow($,$,$,$,$)",self.idx,self.x,self.y,self.width,self.height)

    def plot(self,y,x=None,row=1,col=1,color="black", label=""):
        """Plot data in scope panel
        @param y: Y-axis of the data to plot
        @type y: Array
        @param x: X-axis of the data to plot in the panel
        @type x: Array
        @param row: Row of plot panel where row 1 is the top row
        @type row: int
        @param col: Column of plot panel where column 1 is the left column
        @type col: int
        @param color: Color of the plot line
        @type color: str
        @param label: Signal label
        @type label: str
        @rtype: None
        """
        if x is None:
            x=_data.Data.dim_of(y)
        _data.Data.execute("JavaReplaceSignal($,$,$,$,$,$,$)",self.idx,y,x,row,col,color,label)

    def oplot(self,y,x=None,row=1,col=1,color="black", label=""):
        """Overplot data in scope panel
        @param y: Y-axis of the data to plot
        @type y: Array
        @param x: X-axis of the data to plot in the panel
        @type x: Array
        @param row: Row of plot panel where row 1 is the top row
        @type row: int
        @param col: Column of plot panel where column 1 is the left column
        @type col: int
        @param color: Color of the plot line
        @type color: str
        @param label: Signal label
        @type label: str
        @rtype: None
        """
        if x is None:
            x=_data.Data.dim_of(y)
        _data.Data.execute("JavaAddSignal($,$,$,$,$,$,$)",self.idx,y,x,row,col,color,label)


