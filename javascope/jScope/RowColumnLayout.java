package jScope;

import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Insets;
import java.awt.LayoutManager;
import java.awt.Rectangle;


/**
 * RowColumnLayout is a layout manager that dispose added component in
 * a grid defined by number of column and number of components in column.
 * Column width and component height can be modified by pointer
 * interaction. An Example explaine how to use it:
 *
 * <pre><code>
 * class RowColumnExample extends Panel {
 *
 *    protected RowColumnLayout    row_col_layout;
 *    protected int rows[] = {2, 2};
 *
 *    //Resize button class
 *    class ResizeButton extends Canvas {
 *
 * 	        ResizeButton()
 * 	        {
 * 	            setBackground(Color.lightGray);
 * 	            setCursor(new Cursor(Cursor.CROSSHAIR_CURSOR));
 * 	        }
 *
 * 	        public void paint(Graphics g)
 * 	        {
 * 	            Rectangle d = getBounds();
 * 	            g.draw3DRect(0, 0, d.width-1, d.height-1, true);
 * 	            g.dispose();
 * 	        }
 * 	        public void print(Graphics g){}
 * 	        public void printAll(Graphics g){}
 *     }
 *
 *
 *     public RowColumnExample()
 *     {
 *       	row_col_layout = new RowColumnLayout(rows);
 *       	setLayout(row_col_layout);
 *
 * 		//The number of component to add to panel are 4 (2 column each colum 2 component)
 * 		//Before add component we must add num_component - 1 canvas objet that are used
 * 		//to interact with layout manager for interactive resize of width and height of
 * 		//the grid
 *       	ResizeButton b;
 *       	for(int i = 0; i < 4 - 1; i++) {
 * 	    	    add(b = new ResizeButton());
 * 	    	    b.addMouseListener(new MouseAdapter()
 *                   {
 *   //Action used to resize column width or component height
 *                      public  void mouseReleased(MouseEvent e)
 *                      {
 *                           Component ob = e.getComponent();
 *                           if(ob instanceof ResizeButton)
 * 	                            row_col_layout.ResizeRowColumn(ob, e.getPoint().x, e.getPoint().y);
 *                      }
 *                   });
 * 	    	    b.addMouseMotionListener(new MouseMotionAdapter()
 *                   {
 *   //Action used to draw resiaze line on the component, only if component impement double buffering
 *                      public  void mouseDragged(MouseEvent e)
 *                      {
 *                          Component ob = e.getComponent();
 *                          row_col_layout.DrawResize(ob, e.getPoint().x, e.getPoint().y);
 *                      }
 *                   });
 *       	}
 *
 *       	for(int i = 0, k = 0; i < columns; i++)
 *       	{
 * 	    	    for(int j = 0; j < rows[i]; j++)
 * 	    	    {
 * 	       		    add(<component>);
 * 	       		    k++;
 * 	    	    }
 *       	}
 *
 *       	validate();
 *    }
 *  }
 *
 * </code></pre>
 *
 * @see RowColumnContainer
 */
public class RowColumnLayout implements LayoutManager {

    /**
     * Vertical component space in pixel
     */
    private int vgap = 0;
    /**
     * Horizontal componet space in pixel
     */
    private int hgap = 0;

    /**
     * Minimum width dimension
     */
    private int minWidth = 0;

    /**
     * minimum height dimension
     */
    private int minHeight = 0;

    /**
     * maximum width dimension
     */
    private int maxWidth = 0;

    /**
     * maximum height dimension
     */
    private int maxHeight = 0;

    /**
     * preferred width size
     */
    private int preferredWidth = 0;

    /**
     * preferred height size
     */
    private int preferredHeight = 0;

    /**
     * Unknow size boolean flag
     */
    private boolean sizeUnknown = true;

    /**
     * number of column
     */
    private int column;

    /**
     * number of button resize component
     */
    private int b_comp = 0;


    /**
     * Vector to define the number of component in column
     */
    private int row[];

    /**
     * Vector that defined height components
     */
    private float percent_height[];

    /**
     * Vector that defined width components
     */
    private float percent_width[];

    /**
     * Current resize mode
     */
    private int resize_mode;

    /**
     * Current componet to be resized
     */
    private int comp_idx;

    /**
     * current culomn idx to be resized
     */
    private int col_idx;

   /**
    * Current column y position
    */
   // private int pos_x;
    private int pos_y;

    /**
     * Parent component containr
     */
    private Container main_p;

    /**
     * None mode item
     */
    static final int NONE = 3;
    /**
     * Vertical mode item
     */
    static final int VERTICAL = 2;
    /**
     * Horizontal mode item
     */
    static final int HORIZONTAL = 1;

    /**
     * Minimum component height size
     */
    private int MIN_SIZE_H = 10;

    /**
     * minimum column width
     */
    private int MIN_SIZE_W = 10;

    /**
     * Canvas resize objet y position
     */
    private int by_pos = 20;

    /**
     * Canvas resize object x position
     */
    private int bx_pos = 25;

    /**
     * Boolean resize flag true if must be computed size vomponent
     */
    private boolean init_resize = true;

    /**
     * Internal variable
     */
    private int prev_col_idx = -1;

    /**
     * Costruct a RowColumnLayout with a column number defined by _column
     * argument and number of object per culomn defined by row[] vector
     *
     * @param column Number of column
     * @param row row[i] define number of component in column i
     */
    public RowColumnLayout(int[] row)
    {
        SetRowColumn(row, null, null);
    }
    /**
     * Costruct a RowColumnLayout with a column number defined by _column
     * argument and number of object per column defined by row[] vector.
     * The width size of ith column is defined by pw[i] value and height size
     * of jth component is defuned by pw[j]. The pw[i] and ph[j] value are normalize
     * value in rangeform 0 to 1.
     *
     * @param row row[i] define number of component in column i
     * @param ph Vector of normalize height of component. The sum of ph[x] of the objects in a
     * column must be 1.
     * @param pw Vector of normalize width of the culomn. The sum of pw[x] must be 1
     */

    public RowColumnLayout(int[] row, float ph[], float pw[])
    {
        SetRowColumn(row, ph, pw);
    }

    /**
     * Set normalize size of column width and component height
     *
     * @param ph Vector of normalize height of component. The sum of ph[x] for objet in a
     * column must be 1.
     * @param pw Vector of normalize width of the culomn. Sum of pw[x] must be 1
     */
    public void SetPanelSize(float ph[], float pw[])
    {
	    if(ph != null && pw != null)
	    {
            if(ph.length < b_comp + 1 || pw.length < row.length)//column)
                return; //definire exception

	        percent_height = new float[b_comp + 1];
	        percent_width  = new float[row.length];

	        for(int i = 0; i < b_comp + 1; i++)
		        percent_height[i] = ph[i];

	        for(int i = 0; i < row.length; i++)
	        {
	            /*
	            if(MaxWidth * pw[i] > currMaxWidth - MIN_SIZE_W * (column - i))
	                pw[i] = ((float)currMaxWidth - MIN_SIZE_W * (column - i)) / MaxWidth;
	            currMaxWidth -= MaxWidth * pw[i];
	            */
		        percent_width[i] = pw[i];
		    }
	        sizeUnknown = false;
	    }
    }
    /**
     * Method used to update number of column end number of componet for column
     *
     * @param row row[i] define number of component in column i
     * @param ph Vector of normalize height of component. The sum of ph[x] for objet in a
     * column must be 1.
     * @param pw Vector of normalize width of the culomn. Sum of pw[x] must be 1
     */
    public void SetRowColumn(int[] row, float ph[], float pw[])
    {
        SetRowColumn(row);
        SetPanelSize(ph, pw);
    }
    /**
     * Set number of column an number of component for column
     *
     * @param row row[i] define number of component in column i
     */
    public void SetRowColumn(int[] row)
    {
        //if(row.length < column)
        //    return; //exception
        b_comp = 0;
        column = 0;
	    this.row = new int[row.length];
	    for(int i = 0; i < row.length; i++)
	    {
	        this.row[i] = row[i];
	        b_comp += row[i];
	        if(row[i]!= 0)
	            column++;
	    }
	    b_comp--;
//        setSize(main_p);
	    sizeUnknown = true;
    }

    /**
     * Return normalize height of ith componet
     *
     * @param i index of component
     * @return Normalize Height
     */
    public float getPercentHeight(int i)
    {
	    if(i < percent_height.length)
	        return percent_height[i];
	    else
	        return 0;
    }

    /**
     * Return width of ith column
     *
     * @param i index of column
     * @return Normalize column width
     */
    public float getPercentWidth(int i)
    {
	    if(i < row.length)//column)
	        return percent_width[i];
	    else
	        return 0;
    }

    /**
     * Return number of column
     *
     * @return Integer number of column
     */
    public int GetColumns()
    {
        return column;
    }

    /**
     * Return integer vector of number of component in each column
     *
     * @return Integer vector where ith value is number of component in ith column
     */
    public int[] GetRows()
    {
        return row;
    }

    /**
     * Do nothing
     * Required by LayoutManager.
     * @param name Component name
     * @param comp Component
     */
    public void addLayoutComponent(String name, Component comp) {
//        System.out.println("Add cmp");
    }

    /**
     * Do nothing
     * Required by LayoutManager.
     * @param comp component
     */
    public void removeLayoutComponent(Component comp) {
//        System.out.println("remove cmp");
    }

    /**
     * Initialize component size
     *
     * @param parent parent component
     */
    private void setSizes(Container parent) {
        int nComps = parent.getComponentCount();
        Dimension d = null;
	    int totW, totH, k, maxW[];

        //Reset preferred/minimum width and height.
        preferredWidth = 0;
        preferredHeight = 0;
        minWidth = 0;
        minHeight = 0;

    	if(2 * b_comp + 1 != nComps) {
            throw new IllegalArgumentException("Invalid number of component in RowColumnLayout");
	    }
	    percent_height = new float[b_comp + 1];
	    percent_width  = new float[row.length];//column];
	    Dimension min_d = parent.getComponent(b_comp + 0).getMinimumSize();

	    MIN_SIZE_W = min_d.width;
	    MIN_SIZE_H = min_d.height;

	    k = 0;
	    totW=0;
	    maxW = new int[row.length];//column];
        for (int i = 0; i < row.length; i++) {//column; i++) {
            if(row[i] == 0) continue;
	        maxW[i]=0;
	        totH=0;
	        for(int j = 0; j < row[i]; j++) {
		        Component c = parent.getComponent(b_comp + k++);

//		        if(!(c instanceof RowColumnComponent))
//		            return; //definire exception

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

	    k = 0;
	    for(int i = 0; i < row.length; i++) {// column; i++) {
	        if(row[i] == 0) continue;
	        for(int j = 0; j < row[i]; j++) {
		        Component c = parent.getComponent(b_comp + k);
		        if(c.isVisible()) {
		            percent_height[k] = (float)1./row[i];
		        }
		        k++;
	        }
 	        percent_width[i]  = (float)1./column;
	    }


//	    resetPercentHW(parent);
    }

/**
 * Set componet to a columns all with the same height, componet portion
 * of the parent width
 *
 * @param col Integer index of column
 */
    private void ResetHeight(int col)
    {
        if(row != null && col < row.length)
        {
	        int k = 0;
    	    for(int i = 0; i < col; i++)
	            k += row[i];

    	    if(percent_height != null && k + row[col] <= percent_height.length)
    	    {
	            for(int j = 0; j < row[col]; j++) {
	                percent_height[k] = (float)1./row[col];
		            k++;
	            }
	        }
	    }
     }

    /**
     * Set to all column to the same width, column portion of the parent width
     */
    private void ResetWidth()
    {
         if(row != null && percent_width != null && percent_width.length >= row.length)
         {
   	        for(int i = 0; i < row.length; i++) {//column; i++)
    	        if(row[i] == 0) continue;
 	            percent_width[i]  = (float)1./column;
 	        }
 	     }
    }


    /**
     * Method to draw orizontal or vertical line during resize componet or
     * column. Line/s are drow on the component only if double buffering
     * is supported on component. RowColumnLayout manager check double buffering
     * component capability by isDoubleBuffered() method of component.
     *
     * @param _b Canvas object used to interact with mouse drag
     * @param x Canvas x position
     * @param y Canvas y position
     * @see RowColumnComponet
     */
    public void DrawResize(Component _b, int x, int y)
    {
        Component c = null, b;
        int idx = 0, k;
        boolean found;
        int pos;
        Rectangle r;
        int i, j , jj, num_line = 0, pos_y, n_draw, curr_height, start_pos;

        if(init_resize)
        {
            init_resize = false;
            found = false;
	        for( j = 0; j < row.length && !found; j++) {//column && !found; j++) {
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
                //((RowColumnComponent)c).DrawYResize(0, 0, 0);
                  DrawYResize(c, 0, 0, 0);
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
	                   //((RowColumnComponent)c).DrawYResize(c, pos_y - 2, n_draw, -MIN_SIZE_H);
                            DrawYResize(c, pos_y - 2, n_draw, -MIN_SIZE_H);
                        } else {
	                   // ((RowColumnComponent)c).DrawYResize(0, 0, 0);
	                        DrawYResize(c, 0, 0, 0);
	                 }
                }
            }
            else
            {
                pos_y = pos = y;
                curr_height = 0;
                start_pos = _b.getBounds().y;

                c = main_p.getComponent(b_comp+comp_idx);
                //((RowColumnComponent)c).DrawYResize(0, 0, 0);
                DrawYResize(c, 0, 0, 0);

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
//	                   ((RowColumnComponent)c).DrawYResize(pos_y - 2, n_draw, MIN_SIZE_H);
	                   DrawYResize(c, pos_y - 2, n_draw, MIN_SIZE_H);
	                } else {
//	                    ((RowColumnComponent)c).DrawYResize(0, 0, 0);
	                    DrawYResize(c, 0, 0, 0);
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
	                //((RowColumnComponent)c).DrawXResize(0);
	                DrawXResize(c, 0);
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
	            //((RowColumnComponent)c).DrawXResize(pos - 2);
                    DrawXResize(c, pos - 2);
                }
	    }
    }

    /**
     * Calculate equal space of column and equal width of component in
     * column
     *
     * @param _b Canvas object used to interact with mouse if _b is column resize
     * canvas equal width culomn are performed if _b is componet column resize
     * all component in column are resized to equal height.
     */
    public void ResizeRowColumn(Component _b)
    {
        Component b;
	    int idx = 0;

		init_resize = true;
	    for(int j = 0; j < row.length; j++) { //column; j++) {
	        for(int i = 0; i < row[j]; i++)
	        {
		        b = main_p.getComponent(idx);
		        if(_b == b) {
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
	        ResetHeight(col_idx);
	        ResizeColumn(main_p, col_idx);
	    }

	    if(resize_mode == HORIZONTAL)
	    {
	        ResetWidth();
	        ResizeColumns(main_p);
    	}
    }

    private int nextColumn(int idx)
    {
        for(int i = idx + 1; i < row.length; i++)
            if(row[i] != 0) return i;
        return -1;
    }

    /**
     * Culomn width and component height resize on x or y value.
     *
     * @param _b Canvas object used to interact with mouse if _b is column resize
     * canvas columns width are resize on x value if _b is componet column resize
     * component height are resize on y value.
     * @param x x position
     * @param y y position
     */
    public void ResizeRowColumn(Component _b, int x, int y)
    {
	int k = 0, currHeight = 0, curr_y = 0, curr_x = 0;
	int currMaxHeight, currMaxWidth;
	int max_y, max_x, min_x;
	int start_comp, end_comp, inc_comp;
	int new_y, resize_y, idx = 0;
	Component b, c;
	Rectangle d, d1;


		init_resize = true;
		prev_col_idx = -1;
	    for(int j = 0; j < row.length; j++) { //column; j++) {
	        for(int i = 0; i < row[j]; i++) {
		        b = main_p.getComponent(idx);
		        if(_b == b) {
		            c = main_p.getComponent(b_comp + idx);
		            d = c.getBounds();
		            d1 = b.getBounds();
		            col_idx = j;
		            comp_idx = idx;
		            //pos_x = d.x + d.width;
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
	    ResizeColumn(main_p, col_idx);
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
							percent_width[nextColumn(col_idx)])) + hgap;
							//percent_width[col_idx + 1])) + hgap;
	    if(x < min_x) x = min_x;
	    if(x > max_x - MIN_SIZE_W) x = max_x - MIN_SIZE_W;

	    percent_width[col_idx] = (float)(x - curr_x)/currMaxWidth;
//	    percent_width[col_idx + 1] = (float)(max_x - x - hgap)/currMaxWidth;
	    percent_width[nextColumn(col_idx)] = (float)(max_x - x - hgap)/currMaxWidth;

	    ResizeColumns(main_p);
    	}
    }

    /**
     * Resize component width in colums
     *
     * @param parent component container
     */
    private void ResizeColumns(Container parent)
    {
	    int k = 0, x = 0;
	    int previousWidth = 0, currMaxWidth, currWidth;

	    currMaxWidth = maxWidth - (column - 1) * hgap;
	    int curr_col = 0;
	    for(int i = 0; i < row.length; i++)//column; i++)
	    {
	        if(row[i] == 0) continue;
	        currWidth = (int)(currMaxWidth * percent_width[i]);
	        for(int j = 0; j < row[i]; j++) {
		        Component c = parent.getComponent(b_comp + k);
		        if (c.isVisible()) {
		            Dimension d = c.getSize();//c.getPreferredSize();
		            x = previousWidth;
		            d.width = currWidth;
		            //if(i != column - 1)
		            if(curr_col != column - 1)

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
			            b.invalidate();
		            }
		        }
		        k++;
	        }
	        previousWidth += currWidth + hgap;
	        curr_col++;
    	 }
    }

    /**
     * Resize column height component
     *
     * @param parent component container
     * @param col_idx column index
     */
    private void ResizeColumn(Container parent, int col_idx)
    {
	int k = 0, y = 0;
	int previousHeight = 0, currMaxHeight;

	    for(int i = 0; i <  col_idx; i++)
	        k += row[i];

	    currMaxHeight = maxHeight - (row[col_idx] - 1) * vgap;
	    for(int j = 0; j < row[col_idx]; j++) {
	        Component c = parent.getComponent(b_comp + k);
            if (c.isVisible()) {
                Dimension d = c.getSize();//c.getPreferredSize();
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

    /**
     * Required by LayoutManager
     *
     * @param parent component container
     * @return component container dimension
     */
    public Dimension preferredLayoutSize(Container parent) {
        Dimension dim = new Dimension(0, 0);

        setSizes(parent);

        //Always add the container's insets!
        Insets insets = parent.getInsets();
        dim.width = preferredWidth + insets.left + insets.right;
        dim.height = preferredHeight + insets.top + insets.bottom;

        sizeUnknown = false;

        return dim;
    }

    /**
     * Required by LayoutManager
     *
     * @param parent component container
     * @return component container dimension
     */
    public Dimension minimumLayoutSize(Container parent) {
        Dimension dim = new Dimension(0, 0);

        //Always add the container's insets!
        Insets insets = parent.getInsets();
        dim.width = minWidth + insets.left + insets.right;
        dim.height = minHeight + insets.top + insets.bottom;

        sizeUnknown = false;

        return dim;
    }

    /**
     * Required by LayoutManager.
     * This is called when the panel is first displayed,
     * and every time its size changes.
     * Note: You CAN'T assume preferredLayoutSize() or minimumLayoutSize()
     * will be called -- in the case of applets, at least, they probably
     * won't be.
     *
     * @param parent component container
     */
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
        this.ResizeColumns(parent);
  	    for(int i = 0; i < row.length; i++)//column; i++)
            this.ResizeColumn(parent, i);

    }


    /**
     * Draw vertical line at x position on component
     *
     * @param x vertical line position
     * @param c Component on which draw line/s
     */
    private void DrawXResize(Component c, int x)
    {

   //     if(System.getProperty("java.version").indexOf("1.2") != -1 && !c.isDoubleBuffered())
   //         return;
   //     else
            if(!(c instanceof Waveform || c instanceof MultiWaveform))
                return;
        Dimension d = c.getSize();
        Graphics g = c.getGraphics();
        c.paint(g);
		g.drawLine(x, 0, x, d.height);
		g.dispose();
    }

    /**
     * Draw horizontal n_lines lines firt at y position next at y+ith*space on component
     * where ith came form 1 to n_lines. Multi horizontal line must be draw when more
     * than 1 component is resized.
     *
     * @param y start y horizontal lines
     * @param n_line number of line to draw
     * @param space number of pixel between line
     * @param c component on which draw line/s
     */
    private void DrawYResize(Component c, int y, int n_line, int space)
    {
 //       if(System.getProperty("java.version").indexOf("1.2") != -1 && !c.isDoubleBuffered())
 //           return;
 //       else
            if(!(c instanceof Waveform || c instanceof MultiWaveform))
                return;
        Dimension d = c.getSize();
        Graphics g = c.getGraphics();
        c.paint(g);
        for(int i = 0; i < n_line; i++)
		{
		    g.drawLine(0, y+i*space, d.width, y+i*space);
		}
        g.dispose();

    }

    /**
     * Returns a string representation of this RowColumnLayout and its values.
     *
     * @return a string representation of this layout.
     */
    public String toString() {
        String str = "";
        return getClass().getName() + "[vgap=" + vgap + str + "]";
    }

    public float[] getPercentHeight()
    {
      return percent_height;
    }

    public float[] getPercentWidth()
    {
      return percent_width;
    }

}
