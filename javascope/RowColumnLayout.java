import java.awt.*;
import java.util.Vector;

public class RowColumnLayout implements LayoutManager {

    private int vgap = 0, hgap = 0;
    private int minWidth = 0, minHeight = 0;
    private int maxWidth = 0, maxHeight = 0;
    private int preferredWidth = 0, preferredHeight = 0;
    private boolean sizeUnknown = true;
    private int column, b_comp = 0;
    private int row[];
    private float percent_height[], percent_width[];
    private int resize_mode, comp_idx, col_idx;
    private int pos_x, pos_y;
    private Container main_p;
    static final int HORIZONTAL = 1, VERTICAL = 2, NONE = 3;
    static final int MIN_SIZE = 20;
    

    public RowColumnLayout(int _column, int[] _row) {
        column = _column;
	    row = new int[column];
	    for(int i = 0; i < column; i++) {
	        row[i] = _row[i];
	        b_comp += row[i];
	    }
	    b_comp--;
    }
    
    public RowColumnLayout(int _column, int[] _row, float ph[], float pw[]) {
	    int nComps = 0;
	
        column = _column;
	    row = new int[column];
	
	    for(int i = 0; i < column; i++){
	        row[i] = _row[i];
	        nComps += row[i];
	    }
	    b_comp = nComps - 1;
	
	    if(ph != null && pw != null)
	    {
	        percent_height = new float[nComps];
	        percent_width  = new float[column];
	
	        for(int i = 0; i < nComps; i++)    
		        percent_height[i] = ph[i];
	        for(int i = 0; i < column; i++)     
		        percent_width[i] = pw[i];
	        sizeUnknown = false;
	    }
    }

    public float getPercentHeight(int i)
    {
	    if(i < percent_height.length)
	        return percent_height[i];
	    else
	        return 0;
    }

    public float getPercentWidth(int i)
    {
	    if(i < column)
	        return percent_width[i];
	    else
	        return 0;
    }


    /* Required by LayoutManager. */
    public void addLayoutComponent(String name, Component comp) {
    }

    /* Required by LayoutManager. */
    public void removeLayoutComponent(Component comp) {
    }

    private void setSizes(Container parent) {
        int nComps = parent.getComponentCount();
        Dimension d = null;
	    int tot_comp = 0, totW, totH, k, maxW[];

        //Reset preferred/minimum width and height.
        preferredWidth = 0;
        preferredHeight = 0;
        minWidth = 0;
        minHeight = 0;
	
    	if(2 * b_comp + 1 != nComps)
	    return; //errore nella widget
	    
	    percent_height = new float[b_comp + 1];
	    percent_width  = new float[column];
	
	    k = 0;
	    totW=0;
	    maxW = new int[column];
        for (int i = 0; i < column; i++) {
	        maxW[i]=0;
	        totH=0;
	        for(int j = 0; j < row[i]; j++) {
		        Component c = parent.getComponent(b_comp + k++);
    		    if (c.isVisible()) {
		            d = c.getPreferredSize();
		            if(d.height < MIN_SIZE) d.height = MIN_SIZE;
		            if(d.width < MIN_SIZE) d.width = MIN_SIZE;	
		            if (maxW[i] < d.width) 
			            maxW[i] = d.width;
		            totH += d.height;
		        }
	        }
	        if(totH > preferredHeight)
		        preferredHeight = totH;
	        totW += maxW[i];
	    }  	        		    
	    preferredWidth = totW;
	    minWidth =  preferredWidth;
	    minHeight = preferredHeight;
	  
	    resetPercentHW(parent);	    
    }


    private void resetPercentHW(Container parent)
    {
	    int k = 0;
	
	    for(int i = 0; i < column; i++) {
	        for(int j = 0; j < row[i]; j++) {
		        Component c = parent.getComponent(b_comp + k);
		        if(c.isVisible()) {
		            percent_height[k] = (float)1./row[i];
		        }
		        k++;
	        }        
 	        percent_width[i]  = (float)1./column;
	    }
   }
   
   private void resetPercentHW()
   {
	    int k = 0;
    	for(int i = 0; i < column; i++) {
	        for(int j = 0; j < row[i]; j++) {
		        percent_height[k] = (float)1./row[i];
		        k++;
	        }        
 	        percent_width[i]  = (float)1./column;
	    }
   }
    
    private void resetPercentH(int col)
    {
	    int k = 0;
    	for(int i = 0; i < col; i++) 
	        k += row[i];
	    
	    for(int j = 0; j < row[col]; j++) {
	        percent_height[k] = (float)1./row[col];
		    k++;
	    }        
     }
    
    private void resetPercentW()
    {
    	for(int i = 0; i < column; i++) 
 	        percent_width[i]  = (float)1./column;	
    }    

    public void resizeRowColumn(Component _b)
    {
	    int idx = 0;
			    					    						
	    for(int j = 0; j < column; j++) {
	        for(int i = 0; i < row[j]; i++) {
		        Component b = main_p.getComponent(idx);
		        if(_b == b) {
		            Component c = main_p.getComponent(b_comp + idx);
		            col_idx = j;
		            comp_idx = idx;
		            if(i == row[j] - 1)
			            resize_mode = HORIZONTAL;
		            else
			            resize_mode = VERTICAL;
		        }
		        idx++;
	        }
	    }						
									    							
    	if(resize_mode == VERTICAL)
	    {	
	        resetPercentH(col_idx);
	        resizeColumn(main_p, col_idx);	    
	    }
	
	    if(resize_mode == HORIZONTAL)
	    {
	        resetPercentW();
	        resizeColumns(main_p, col_idx);
    	}	
    }		
    	        
    public void resizeRowColumn(Component _b, int x, int y)
    {
	int k = 0, currHeight = 0, currWidth = 0, curr_y = 0, curr_x = 0;
	int currMaxHeight, currMaxWidth;
	int max_y, max_x, min_x;
	int start_comp, end_comp, inc_comp;
	int new_y, resize_y, idx = 0;
			    					    						
	    for(int j = 0; j < column; j++) {
	        for(int i = 0; i < row[j]; i++) {
		        Component b = main_p.getComponent(idx);
		        if(_b == b) {
		            Component c = main_p.getComponent(b_comp + idx);
		            Rectangle d = c.getBounds();
		            Rectangle d1 = b.getBounds();                
		            col_idx = j;
		            comp_idx = idx;
		            pos_x = d.x + d.width;
		            pos_y = d.y + d.height;
		            x += d1.x;
		            y += d1.y;
		            if(i == row[j] - 1)
			            resize_mode = HORIZONTAL;
		            else
			            resize_mode = VERTICAL;
		        }
		        idx++;
	        }
	    }						
									    	
	    for(int i = 0; i < col_idx; i++) 
	        k += row[i];
	
						
    	if(resize_mode == VERTICAL)
	    {
	        if(maxHeight < row[col_idx] * (MIN_SIZE + vgap) - vgap)
		        return;
	
	        if(pos_y < y)
	        {
		        start_comp = k;
		        end_comp = k+row[col_idx];
		        inc_comp = 1;
		        max_y = maxHeight - (row[col_idx] - (comp_idx - k) - 1) * 
							(MIN_SIZE + vgap) + vgap;		    
	        } else {
		        start_comp = k + row[col_idx] - 1;
		        end_comp = k - 1;
		        inc_comp = -1;
		        y = maxHeight - y;
		        comp_idx++;
		        max_y = maxHeight - (comp_idx-k)*(MIN_SIZE+vgap);
	        }
	    	
	        if(y > max_y)
		        resize_y = max_y;
	        else 
		        resize_y = y;
		    
	        currMaxHeight = maxHeight - (row[col_idx] - 1) * vgap;
	        for(k = start_comp, new_y = 0; k != end_comp; k += inc_comp) {
    		    currHeight = (int)(currMaxHeight * percent_height[k]);	
		    if(k == comp_idx)
		        percent_height[k] = (float)(resize_y - curr_y)/currMaxHeight;			    			   			
	        else
		        if(curr_y + currHeight - new_y < MIN_SIZE)
			        percent_height[k] = (float)(MIN_SIZE)/currMaxHeight;
		        else	
			        percent_height[k] = (float)(curr_y + currHeight - new_y)/currMaxHeight;
		    curr_y += currHeight + vgap;
		    new_y += (int)(currMaxHeight * percent_height[k]) + vgap;	    
    	}
	    resizeColumn(main_p, col_idx);	    
	}
	
	if(resize_mode == HORIZONTAL)
	{
	
	    if(maxWidth < column * (MIN_SIZE + hgap) - hgap)
		return;
	
	    currMaxWidth = (maxWidth - (column - 1) * hgap);

	    for(int i = 0; i < col_idx; i++)
		curr_x += currMaxWidth * percent_width[i] + hgap;
		
	    min_x = curr_x + MIN_SIZE;
	    max_x = (int)(curr_x + currMaxWidth * (percent_width[col_idx] + 
							percent_width[col_idx + 1])) + hgap; 	
	    if(x < min_x) x = min_x;
	    if(x > max_x - MIN_SIZE) x = max_x - MIN_SIZE;
	    
	    percent_width[col_idx] = (float)(x - curr_x)/currMaxWidth;
	    percent_width[col_idx + 1] = (float)(max_x - x - hgap)/currMaxWidth;

	    resizeColumns(main_p, col_idx);
    	}
    }
    
    public void DisableResize(Component comp)
    {
	    resize_mode = NONE;
    }
    
    private void resizeColumns(Container parent, int col_idx) 
    {
	    int k = 0, x = 0;
	    int previousWidth = 0, currMaxWidth, currWidth;
		
	    currMaxWidth = maxWidth - (column - 1) * hgap;
	    for(int i = 0; i < column; i++)
	    {    
	        currWidth = (int)(currMaxWidth * percent_width[i]);
	        for(int j = 0; j < row[i]; j++) {
		        Component c = parent.getComponent(b_comp + k);
		        if (c.isVisible()) {
		            Dimension d = c.getPreferredSize();                
		            x = previousWidth;
		            d.width = currWidth;
		            if(i != column - 1)
			            d.width  = (int)(currMaxWidth  * percent_width[i]);
		            else
			            d.width = maxWidth - x;
		            c.setBounds(x, c.getBounds().y, d.width, c.getBounds().height);
		            if(k < b_comp)
		            {
		                Component b = parent.getComponent(k);		
		                if(j == row[i] - 1) 	    
			                b.setBounds(x + d.width - hgap - 2, maxHeight - 20, 4, 8);
		                else 		    
			                b.setBounds(x + d.width - 25,  c.getBounds().y + c.getBounds().height - 2, 8, 4 );
		            }
		        }
		        k++;
	        }
	        previousWidth += currWidth + hgap;
    	 }       	
    }

    private void resizeColumn(Container parent, int col_idx)
    {
	int k = 0, x, y = 0;
	int previousHeight = 0, currMaxHeight;

	    for(int i = 0; i <  col_idx; i++)
	        k += row[i];
	
	    currMaxHeight = maxHeight - (row[col_idx] - 1) * vgap; 
	    for(int j = 0; j < row[col_idx]; j++) {
	        Component c = parent.getComponent(b_comp + k);
            if (c.isVisible()) {
                Dimension d = c.getPreferredSize();                
		        y += previousHeight;
		        d.height = (int)(currMaxHeight * percent_height[k]);
		        if(j != row[col_idx] - 1) 	     
		            d.height = (int)(currMaxHeight * percent_height[k]);
		        else
		            d.height = maxHeight - y;
		        c.setBounds(c.getBounds().x, y, c.getBounds().width, d.height);
                previousHeight = d.height + vgap;
		        if(k < b_comp)
		        {
		            Component b = parent.getComponent(k);		
		            if(j == row[col_idx] - 1)
		                b.setBounds(c.getBounds().x + c.getBounds().width - hgap - 2, maxHeight - 20, 4, 8);		     		    
		            else 	    
		                b.setBounds(c.getBounds().x +  c.getBounds().width - 25, y + d.height -2, 8, 4 );
		        }
	        }
	        k++;
    	 }       	
    }

    /* Required by LayoutManager. */
    public Dimension preferredLayoutSize(Container parent) {
        Dimension dim = new Dimension(0, 0);
        int nComps = parent.getComponentCount();

        setSizes(parent);

        //Always add the container's insets!
        Insets insets = parent.getInsets();
        dim.width = preferredWidth + insets.left + insets.right;
        dim.height = preferredHeight + insets.top + insets.bottom;

        sizeUnknown = false;

        return dim;
    }

    /* Required by LayoutManager. */
    public Dimension minimumLayoutSize(Container parent) {
        Dimension dim = new Dimension(0, 0);
        int nComps = parent.getComponentCount();

        //Always add the container's insets!
        Insets insets = parent.getInsets();
        dim.width = minWidth + insets.left + insets.right;
        dim.height = minHeight + insets.top + insets.bottom;
	
        sizeUnknown = false;

        return dim;
    }

    /* Required by LayoutManager. */
    /* This is called when the panel is first displayed, 
     * and every time its size changes. 
     * Note: You CAN'T assume preferredLayoutSize() or minimumLayoutSize()
     * will be called -- in the case of applets, at least, they probably
     * won't be. */
    public void layoutContainer(Container parent) {
        Insets insets = parent.getInsets();
        int nComps = parent.getComponentCount();
        int previousWidth = 0, previousHeight = 0;
	    int currMaxWidth = 0, currMaxHeight = 0;
        int k = 0, x = 0, y = insets.top;
        int rowh = 0, start = 0;
        int xFudge = 0, yFudge = 0;
        boolean oneColumn = false;
	
        maxWidth  = parent.getSize().width  - (insets.left + insets.right);
        maxHeight = parent.getSize().height - (insets.top + insets.bottom);	
	
    	main_p = parent;    
	
	    if(maxWidth <= 0 || maxHeight <= 0)
          return;
	  	  
        // Go through the components' sizes, if neither preferredLayoutSize()
        // nor minimumLayoutSize() has been called.
        if (sizeUnknown) {
            setSizes(parent);
        }
            
        if (maxWidth <= minWidth) {
            oneColumn = true;
        }

	    currMaxWidth = maxWidth - column * hgap;


    	k = 0;
  	    for(int i = 0; i < column; i++) {
	        previousHeight = 0;
	        y = 0;
	        currMaxHeight = maxHeight - (row[i] - 1) * vgap; 
	        for(int j = 0; j < row[i]; j++) {
                Component c = parent.getComponent(b_comp + k);
                if (c.isVisible()) {
		            y += previousHeight;
                    Dimension d = c.getPreferredSize();                
		            if(i != column - 1)
		                d.width  = (int)(currMaxWidth  * percent_width[i]);
		            else
		                d.width = maxWidth - x;
		            if(j != row[i] - 1) 	     
		                d.height = (int)(currMaxHeight * percent_height[k]);
		            else
		                d.height = maxHeight - y;
		            c.setBounds(x, y, d.width, d.height);
                    previousWidth  = d.width;
                    previousHeight = d.height + vgap;
		            if(k < b_comp)
		            {
		                Component b = parent.getComponent(k);		
		                if(j == row[i] - 1) 	    
		                    b.setBounds(x + d.width - hgap - 2, maxHeight - 20, 4, 8);		     
		                else 		    
		                    b.setBounds(x + previousWidth - 25, y + d.height - 2, 8, 4);
		            }
	            }
	            k++;
    	    }
	        x += previousWidth + hgap;
	    }
    }
    
    public String toString() {
        String str = "";
        return getClass().getName() + "[vgap=" + vgap + str + "]";
    }
}
