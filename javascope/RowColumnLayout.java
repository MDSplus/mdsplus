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
    private int MIN_SIZE_H = 30, MIN_SIZE_W = 30;
    private int by_pos = 20, bx_pos = 25;
    

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
	    Dimension min_d = parent.getComponent(b_comp + 0).getMinimumSize();
	
	    MIN_SIZE_W = min_d.width;
	    MIN_SIZE_H = min_d.height;
	
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
		            if(d.height < MIN_SIZE_H) d.height = MIN_SIZE_H;
		            if(d.width < MIN_SIZE_W) d.width = MIN_SIZE_W;	
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
    
    private boolean init_resize = true;
    private int prev_col_idx = -1;
    
    public void drawResize(Component _b, int x, int y)
    {
        Component c = null, b;
        int idx = 0, k;
        boolean found;
        int pos, pos_x[];
        Rectangle r;
        int i, j , jj, num_line = 0, pos_y, n_draw, curr_height, start_pos;
       
        if(init_resize)
        {
            init_resize = false;
            found = false;
	        for( j = 0; j < column && !found; j++) {
	            for( i = 0; i < row[j]; i++) {
		            b = main_p.getComponent(idx);
		            if(_b == b) {
		                col_idx = j;
		                comp_idx = idx;
		                if(i == row[j] - 1)
			                resize_mode = HORIZONTAL;
		                else
			                resize_mode = VERTICAL;
			            found = true;
			            break;
		            }
		            idx++;
	            }
	        }
	    }
	    
	    if(resize_mode == VERTICAL)
	    {
	        k = 0;
	        for(i = 0; i < col_idx + 1; i++)
	            k += row[i];
            
            if(y < 0)
            {
                pos_y = pos = y;
                curr_height = 0;
                start_pos = _b.getBounds().y;
                k -= row[col_idx];
                
                c = main_p.getComponent(b_comp+comp_idx + 1);
                ((Waveform)c).DrawYResize(0, 0, 0);

                for( j = 0, jj = 0,  i = b_comp+comp_idx ; i > b_comp+k-1; i--, j++)
                {
                    c = main_p.getComponent(i);
                    r = c.getBounds();                    
	                curr_height += r.height; 


                    if(curr_height + pos < MIN_SIZE_H * (num_line + 1)) 
                    {
                            num_line++;
                    }
                    

                    if(start_pos + pos + (k - comp_idx - 1) * MIN_SIZE_H <= 0)
                        return;
                    
                    
                    for(n_draw = 0; jj < num_line + 1; jj++)
                    {
                        if(Math.abs(pos) + jj * MIN_SIZE_H <  curr_height)
                        {
                            n_draw++;
	                    } else
	                        break;
	                }
	                if (n_draw > 0) {
	                    pos_y = (r.height + pos) - (- curr_height + r.height + (jj - n_draw) * MIN_SIZE_H);
	                   ((Waveform)c).DrawYResize(pos_y - 2, n_draw, -MIN_SIZE_H);
	                } else {
	                    ((Waveform)c).DrawYResize(0, 0, 0);
	                }	                
                }
            }
            else 
            {
                pos_y = pos = y;
                curr_height = 0;
                start_pos = _b.getBounds().y;
                
                c = main_p.getComponent(b_comp+comp_idx);
                ((Waveform)c).DrawYResize(0, 0, 0);
                
                for( j = 0, jj = 0, i = b_comp+comp_idx+1; i < b_comp+k; i++, j++)
                {
                    c = main_p.getComponent(i);
                    r = c.getBounds();                    
	                curr_height += r.height; 


                    if(curr_height - pos < MIN_SIZE_H * (num_line + 1)) 
                    {
                            num_line++;
                    }
                    
                    if(start_pos + pos + (k - comp_idx - 1) * MIN_SIZE_H > maxHeight)
                        return;
                    
                    
                    for(n_draw = 0; jj < num_line + 1; jj++)
                    {
                        if(pos + jj * MIN_SIZE_H <  curr_height)
                        {
                            n_draw++;
	                    } else
	                        break;
	                }
	                if (n_draw > 0) {
	                    pos_y = pos - curr_height + r.height + (jj - n_draw) * MIN_SIZE_H;
	                   ((Waveform)c).DrawYResize(pos_y - 2, n_draw, MIN_SIZE_H);
	                } else {
	                    ((Waveform)c).DrawYResize(0, 0, 0);
	                }	                
                }
            }
	    }
	    
	        
	    if(resize_mode == HORIZONTAL)
	    {
	        if(x < 0) 
	            idx = col_idx;
	        else
	            idx = col_idx + 1;
	            
	        if(prev_col_idx != -1 && prev_col_idx != idx)
	        {
	            k = 0;
	            for(i = 0; i < prev_col_idx; i++)
	                k += row[i];
	            for(i = k; i < k + row[prev_col_idx]; i++) {
	                c = main_p.getComponent(b_comp + i);
	                ((Waveform)c).DrawXResize(0);	            
	            }
            }
	        prev_col_idx = idx;

            
	        k = 0;
	        for(i = 0; i < idx; i++)
	            k += row[i];
	        
	        
            c = main_p.getComponent(b_comp+k);

            r = c.getBounds();
            if(x < 0) {
               pos = r.width + x;
               if(pos < MIN_SIZE_W)
                  pos = MIN_SIZE_W;
            } else {
               pos = x;
               if(r.width - pos < MIN_SIZE_W)
                   pos = r.width - MIN_SIZE_W;
            }
	        
	        for(i = k; i < k + row[idx]; i++) {
	            c = main_p.getComponent(b_comp + i);
	            ((Waveform)c).DrawXResize(pos - 2);	            
	        }
	    }
    }

    public void resizeRowColumn(Component _b)
    {
        Component b, c;
	    int idx = 0;
	
		init_resize = true;	    					    						    
	    for(int j = 0; j < column; j++) {
	        for(int i = 0; i < row[j]; i++) {
		        b = main_p.getComponent(idx);
		        if(_b == b) {
		            c = main_p.getComponent(b_comp + idx);
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
	        resizeColumns(main_p);
    	}	
    }		
    	        
    public void resizeRowColumn(Component _b, int x, int y)
    {
	int k = 0, currHeight = 0, currWidth = 0, curr_y = 0, curr_x = 0;
	int currMaxHeight, currMaxWidth;
	int max_y, max_x, min_x;
	int start_comp, end_comp, inc_comp;
	int new_y, resize_y, idx = 0;
	Component b, c;
	Rectangle d, d1;
	
		init_resize = true;
		prev_col_idx = -1;
	    for(int j = 0; j < column; j++) {
	        for(int i = 0; i < row[j]; i++) {
		        b = main_p.getComponent(idx);
		        if(_b == b) {
		            c = main_p.getComponent(b_comp + idx);
		            d = c.getBounds();
		            d1 = b.getBounds();                
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
	        if(maxHeight < row[col_idx] * (MIN_SIZE_H + vgap) - vgap)
		        return;
	
	        if(pos_y < y)
	        {
		        start_comp = k;
		        end_comp = k+row[col_idx];
		        inc_comp = 1;
		        max_y = maxHeight - (row[col_idx] - (comp_idx - k) - 1) * 
							(MIN_SIZE_H + vgap) + vgap;		    
	        } else {
		        start_comp = k + row[col_idx] - 1;
		        end_comp = k - 1;
		        inc_comp = -1;
		        y = maxHeight - y;
		        comp_idx++;
		        max_y = maxHeight - (comp_idx-k)*(MIN_SIZE_H+vgap);
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
		        if(curr_y + currHeight - new_y < MIN_SIZE_H)
			        percent_height[k] = (float)(MIN_SIZE_H)/currMaxHeight;
		        else	
			        percent_height[k] = (float)(curr_y + currHeight - new_y)/currMaxHeight;
		    curr_y += currHeight + vgap;
		    new_y += (int)(currMaxHeight * percent_height[k]) + vgap;	    
    	}
	    resizeColumn(main_p, col_idx);	    
	}
	
	if(resize_mode == HORIZONTAL)
	{
	
	    if(maxWidth < column * (MIN_SIZE_W + hgap) - hgap)
		return;
	
	    currMaxWidth = (maxWidth - (column - 1) * hgap);

	    for(int i = 0; i < col_idx; i++)
		curr_x += currMaxWidth * percent_width[i] + hgap;
		
	    min_x = curr_x + MIN_SIZE_W;
	    max_x = (int)(curr_x + currMaxWidth * (percent_width[col_idx] + 
							percent_width[col_idx + 1])) + hgap; 	
	    if(x < min_x) x = min_x;
	    if(x > max_x - MIN_SIZE_W) x = max_x - MIN_SIZE_W;
	    
	    percent_width[col_idx] = (float)(x - curr_x)/currMaxWidth;
	    percent_width[col_idx + 1] = (float)(max_x - x - hgap)/currMaxWidth;

	    resizeColumns(main_p);
    	}
    }
        
    private void resizeColumns(Container parent) 
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
			                b.setBounds(x + d.width - hgap - 2, maxHeight - bx_pos, 4, 8);
		                else {
		                    int p = by_pos;
		                    if(d.width < by_pos + 8)
		                       p = d.width - 1;
			                b.setBounds(x + d.width - p,  c.getBounds().y + c.getBounds().height - 2, 8, 4 );
			            }    
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
		                b.setBounds(c.getBounds().x + c.getBounds().width - hgap - 2, maxHeight - bx_pos, 4, 8);		     		    
		            else {	    
		                int p = by_pos;
		                if(d.width < by_pos + 8)
		                   p = d.width - 1;
		                b.setBounds(c.getBounds().x + c.getBounds().width - p, y + d.height -2, 8, 4 );
                    }
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
        maxWidth  = parent.getSize().width  - (insets.left + insets.right);
        maxHeight = parent.getSize().height - (insets.top + insets.bottom);	
	
    	main_p = parent;    
	
	    if(maxWidth <= 0 || maxHeight <= 0)
          return;
	  	  
        // Go through the components' sizes, if neither preferredLayoutSize()
        // nor minimumLayoutSize() has been called.
        if (sizeUnknown) {
            setSizes(parent);
            sizeUnknown = false;
        }
        this.resizeColumns(parent);
  	    for(int i = 0; i < column; i++) 
            this.resizeColumn(parent, i);

    }
    
    public String toString() {
        String str = "";
        return getClass().getName() + "[vgap=" + vgap + str + "]";
    }
}
