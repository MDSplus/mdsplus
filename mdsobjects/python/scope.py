if '__package__' not in globals() or __package__ is None or len(__package__)==0:
  def _mimport(name,level):
    return __import__(name,globals())
else:
  def _mimport(name,level):
    return __import__(name,globals(),{},[],level)

_data=_mimport('mdsdata',1)

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

    def plot(self,y,x=None,row=1,col=1,color="black"):
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
        @rtype: None
        """
        if x is None:
            x=_data.Data.dim_of(y)
        _data.Data.execute("JavaReplaceSignal($,$,$,$,$,$)",self.idx,y,x,row,col,color)

    def oplot(self,y,x=None,row=1,col=1,color="black"):
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
        @rtype: None
        """
        if x is None:
            x=_data.Data.dim_of(y)
        _data.Data.execute("JavaAddSignal($,$,$,$,$,$)",self.idx,y,x,row,col,color)
    
    
