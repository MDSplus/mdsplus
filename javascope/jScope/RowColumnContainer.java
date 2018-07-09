package jScope;

import java.awt.Color;
import java.awt.Component;
import java.awt.Cursor;
import java.awt.Graphics;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionAdapter;
import java.util.Vector;

import javax.swing.JComponent;

/**
 * RowColumnContainer object is a component that can contain other AWT
 * components in a grid disposition. This component create on the
 * container a RowColumnLayout manager, so no layout manager
 * can be added to this component.
 *
 * @see RowColumnLayout
 */

public class RowColumnContainer extends JComponent
{

   /**
    * RowColumnLayout
    *
    * @see RowColumnLayout
    */
   protected RowColumnLayout row_col_layout;

   /**
    * number of component in column
    */
   protected int rows[] = {1,0,0,0};

   /**
    * Normalize height of the components in column.
    *
    * @see RowColumnLayout
    */
   protected float ph[];

   /**
    * Normalized width of the columns.
    *
    * @see RowColumnLayout
    */
   protected float pw[];

   private Vector<Point> real_position = new Vector<>();

   private Point split_pos = null;

   private Component maximizeC = null;


   class Btm extends Component
   {

	    Btm()
	    {
	        setBackground(Color.lightGray);
	    }

	    public void paint(Graphics g)
	    {
	        Rectangle d = getBounds();
	        if(d.width > d.height)
	            setCursor(new Cursor(Cursor.N_RESIZE_CURSOR));
	        else
	            setCursor(new Cursor(Cursor.W_RESIZE_CURSOR));
	        g.draw3DRect(0, 0, d.width-1, d.height-1, true);
	    }
	    public void print(Graphics g){}
	    public void printAll(Graphics g){}
    }

    /**
     * Constructs a new RowColumnContainer with one row an column.
     */
     public RowColumnContainer()
     {
        setName("RowColumnContainer");
        row_col_layout = new RowColumnLayout(rows);
        setLayout(row_col_layout);
     }

    /**
     * Constructs a new RowColumnContainer with a defined number
     * of column and componet in column, and array of component
     * to add.
     *
     * @param rows an array of number of component in column
     * @param columns number of columns
     * @param c an array of componet to add
     */
    public RowColumnContainer( int rows[],
                               Component c[])
    {

      int i;
      int num_component;

      setName("RowColumnContainer");
      if(rows == null || rows.length == 0)
        throw new IllegalArgumentException("Defined null or empty row column container");

      this.rows = new int[rows.length];

      for(i = 0; i < rows.length; i++)
         this.rows[i] = rows[i];

      row_col_layout = new RowColumnLayout(rows);
      setLayout(row_col_layout);

      num_component = getComponentNumber();

      Btm b;
      for(i = 0; i < num_component - 1; i++)
      {
	     add(b = new Btm());
	     setListener(b);
      }

      if(c != null)
      {
        if(num_component != c.length)
            throw new IllegalArgumentException("Invalid componet number");
        add(c);
        validate();
      }
   }

   /**
    * Enable event capability on resize button
    *
    * @param b a resize button
    */
   private void setListener(Component b)
   {

	   b.addMouseListener(new MouseAdapter()
	   {
                public void mouseReleased(MouseEvent e)
                {
                    Component ob = e.getComponent();
                    if(ob instanceof Btm)
	                    if(!((e.getModifiersEx() & MouseEvent.BUTTON2_DOWN_MASK) == MouseEvent.BUTTON2_DOWN_MASK))
	                        row_col_layout.ResizeRowColumn(ob, e.getPoint().x, e.getPoint().y);
                }

                public void mouseClicked(MouseEvent e)
                {
                    Component ob = e.getComponent();
                    if(ob instanceof Btm)
	                    if((e.getModifiersEx() & MouseEvent.BUTTON2_DOWN_MASK) == MouseEvent.BUTTON2_DOWN_MASK)
	                        row_col_layout.ResizeRowColumn(ob);
                }
	   });
	   b.addMouseMotionListener(new MouseMotionAdapter()
	   {
                public void mouseDragged(MouseEvent e)
                {
                    Component ob = e.getComponent();
	                if(!((e.getModifiersEx() & MouseEvent.BUTTON2_DOWN_MASK) == MouseEvent.BUTTON2_DOWN_MASK))
                        row_col_layout.DrawResize(ob, e.getPoint().x, e.getPoint().y);
                }
	   });
   }

   /**
    * Add componets to the container. The array componet length must
    * be equals to the componet needed on the row column grid.
    *
    * @param c an array of component
    */

   public void add(Component c[])
   {
      int i, j, k;

      if(c.length != getComponentNumber())
        throw new IllegalArgumentException("Invalid component number");

      for(i = 0, k = 0; i < rows.length; i++)
      {
	    for(j = 0; j < rows[i]; j++)
	    {
	       super.add(c[k]);
	       k++;
	    }
      }
   }



   /*

    private void upadateRealPosition(Point p)
    {
       for(int j = 1; j < real_position.size(); j+=2)
       {
            Point real_pos = (Point)real_position.elementAt(j);
            if(rrow <= real_pos.y && rcol == real_pos.x)
            {
                real_position.setElementAt(new Point(real_pos.x, real_pos.y++), j);
            }
       }
    }
*/

    private boolean positionOverwrite(Point pos)
    {
        for(int i = 1; i < real_position.size(); i+=2)
        {
            Point real_pos = (Point)real_position.elementAt(i);
            if(pos.x == real_pos.x && pos.y == real_pos.y)
            {
               // real_pos.y++;
               // Point in_pos = (Point)real_position.elementAt(i - 1);
               // if(in_pos.x == real_pos.x && in_pos.y == real_pos.y)
               // {
               //     real_position.removeElementAt(i);
               //     real_position.removeElementAt(i-1);
               // }
                return true;
            }
        }
        return false;
    }


    private void setRealPosition(Point in_pos, Point real_pos)
    {
        if(in_pos == null || real_pos == null) return;
        if(in_pos.x != real_pos.x || in_pos.y != real_pos.y)
        {
            real_position.addElement(in_pos);    //added position i
            real_position.addElement(real_pos);  //real position  i+1
        }
    }

    private Point getRealPosition(Point in_pos)
    {
        for(int i = 0; i < real_position.size(); i+=2)
        {
            Point p = (Point)real_position.elementAt(i);
            if(in_pos.x == p.x && in_pos.y == p.y)
                return (Point)real_position.elementAt(i+1);
        }
        return null;
    }



   /**
    * Adds a specific component in row column position, if row, col position
    * position is not present in row column grid, the grid is modified
    * to add new componet.
    *
    * @param c the component to be added
    * @param row component position in column
    * @param col column position of the component
    */

   public void add(Component c, int row, int col)
   {
        int new_rows[], cmp_idx = 0, i;
        Btm b;
        int rrow = row, rcol = col;

        if(getGridComponent(row, col) != null)
            throw new IllegalArgumentException("Component already added in this position");

        if(col > rows.length)
        {
            if(row != 1)
                rrow = 1;

            new_rows = new int[col];
            for(i = 0; i < rows.length; i++)
                new_rows[i] = rows[i];
            new_rows[col - 1] = 1;

            rows = new_rows;
            cmp_idx = -1;
        } else {

            rrow = rows[col-1] + 1;
            cmp_idx = getComponentIndex(rrow - 1, col) + 1;


            for(i = 0; i < real_position.size(); i+=2)
            {
                Point in_pos = (Point)real_position.elementAt(i);
                if(row < in_pos.y && col == in_pos.x)
                {
                    cmp_idx--;
                    rrow--;
                }
            }

            for(int j = 0; j < real_position.size(); j+=2)
            {
                Point in_p = (Point)real_position.elementAt(j);
                if(row < in_p.y && col == in_p.x)
                {
                    Point real_p = (Point)real_position.elementAt(j+1);
                    real_p.y++;
                    if(in_p.x == real_p.x && in_p.y == real_p.y)
                    {
                        real_position.removeElementAt(j);
                        real_position.removeElementAt(j);
                        j-=2;
                        continue;
                    }
                    if(rrow <= real_p.y && rcol == real_p.x)
                    {
                        real_position.setElementAt(new Point(real_p.x, real_p.y), j + 1);
                    }
                }
            }
            rows[col-1]++;
        }
        if(cmp_idx >= 0)
	        super.add(c, cmp_idx);
	    else
	        super.add(c);
	    super.add(b = new Btm(), 0);
	    setListener(b);

        if(rrow != row || rcol != col)
          setRealPosition(new Point(col, row), new Point(rcol, rrow));
   }

   /**
    * Update RowColumnLayout.
    *
    * @see RowColumnLayout
    */
   public void update()
   {
      row_col_layout.SetRowColumn(rows, ph, pw);
      invalidate();
      validate();
   }

   /**
    * Update RowColumnLayout with defined row component height and
    * column width.
    *
    * @param ph Vector of normalize height of component. The sum of ph[x] of the objects in a
    * column must be 1.
    * @param pw Vector of normalize width of the culomn. The sum of pw[x] must be 1
    *
    * @see RowColumnLayout
    */
    public void update(float ph[], float pw[])
    {
        this.ph = ph;
        this.pw = pw;
        update();
    }

    public Component getMaximizeComponent()
    {
      return maximizeC;
    }

    public void resetMaximizeComponent()
    {
        maximizeC = null;
    }

    
    public boolean isMaximize()
    {
      return ( maximizeC != null);
    }


    public void maximizeComponent(Component c)
    {
        maximizeC = c;
        if(c == null)
        {
            update();
            return;
        }

        int n_com = getGridComponentCount() ;

        if (n_com == 1) return;

        int i, j, k;
        float m_ph[] = new float[n_com];
        float m_pw[] = new float[rows.length];
        ph = new float[n_com];
        pw = new float[rows.length];


        Point p = this.getComponentPosition(c);

        for(i = 0, k = 0; i < rows.length; i++)
        {
            if(rows[i] == 0) continue;

            pw[i] = row_col_layout.getPercentWidth(i);

            if(i == p.x-1)
                m_pw[i] = 1;
            else
                m_pw[i] = 0;


            for(j = 0; j < rows[i]; j++)
            {
                if(i == p.x-1 && j == p.y-1)
                   m_ph[k] = 1;
                else
                   m_ph[k] = 0;
                ph[k] = row_col_layout.getPercentHeight(k);
                k++;
            }
        }
        row_col_layout.SetRowColumn(rows, m_ph, m_pw);
        invalidate();
        validate();
    }


   /**
    * Set new grid configuration.
    *
    * @param rows an array of number of component in column
    */
   public void setRowColumn(int rows[])
   {
       this.rows = rows;
       row_col_layout.SetRowColumn(rows);
   }


    /**
     * Gets the number of components, added by user, in this panel.
     * @return    the number of components in this panel.
     */
   public int getGridComponentCount()
   {
      //NOTE: return only the number of user added componet and not
      //internal resize button component.
      return (super.getComponentCount() + 1)/2;
   }

    /**
     * Gets the nth user added component in this container.
     *
     * @param n the index of the component to get.
     * @return the n<sup>th</sup> component in this container.
     * @exception ArrayIndexOutOfBoundsException if the n<sup>th</sup> value does not exist.
     * Need not be called from AWT thread. */

    /*
     Component cmp_xxx;
     int cmp_xxx_idx;
     public Component getGridComponent(int n)
    {
	cmp_xxx_idx = n;
	try {
        SwingUtilities.invokeAndWait(new Runnable() {
                public void run() {
	           System.out.println( " get grid component e in dispatcher thread "+SwingUtilities.isEventDispatchThread());
                    cmp_xxx = getComponent(getGridComponentCount() - 1 + cmp_xxx_idx);
                }
            });
	} catch(InterruptedException e){}
	  catch(InvocationTargetException  e){}
          return cmp_xxx;
     }
    */
     public Component getGridComponent(int n)
     {
 //       System.out.println( " get grid component e in dispatcher thread "+SwingUtilities.isEventDispatchThread());
        return getComponent(getGridComponentCount() - 1 + n);
     }

    /**
     * Gets the (row, col) component in this container.
     * @param row component index in the column
     * @param col column index of the component
     * @return     the (row, col) component in this container,
     *              or null value if does not exist.
     */
   public Component getGridComponent(int row, int col)
   {
      int idx;
      Point curr_pos;
      Point p = getRealPosition(curr_pos = new Point(col,row));

      if(p != null)
      {
         col = p.x;
         row = p.y;
      } else
          if(positionOverwrite(curr_pos))
             return null;

      idx = getComponentIndex(row, col);

      if(idx < 0)
        return null;

      return getComponent(idx);

   }


    /**
     * Gets the (row, col) component index in this container.
     * @param row component index in the column
     * @param col column index of the component
     * @return     the index component in this container,
     *              -1 if the (row, col) value does not exist.
     *                 .
     */
   public int getComponentIndex(int row, int col)
   {
      int cmp_idx = 0;

      if(col > rows.length || row > rows[col-1]) return -1;

      for(int i = 0; i < col - 1; i++)
            cmp_idx += rows[i];


      return (cmp_idx + row + getGridComponentCount() - 2);
   }

    /**
     * Gets the index of the component in this container.
     *
     * @param c component
     * @return index of the component in the container or -1 if not presentt
     */

    public int getComponentIndex(Component c)
    {
	    int idx;
	    for(idx = 0; idx < getGridComponentCount() &&  getGridComponent(idx) != c; idx++);
	    if(idx < getGridComponentCount())
	        return idx + 1;
	    else
	        return -1;
    }

    /**
     * Get (row , col) position of the component in the container
     *
     * @param c componet
     * @return (row, col) position of the component
     * or null if component not presente in the container
     */
    public Point getComponentPosition(Component c)
    {
	    int col = 0, row = 0;

	    if(c == null) return null;

	    int idx = getComponentIndex(c);

	    for(col = 0; col < rows.length; col++)
	    {
	        for(row = 0; row < rows[col] && idx != 0; row++)
		        idx--;
	        if(idx == 0) break;
	    }
	    Point p = new Point(col + 1, row);
	    return p;
    }




    /**
     * Update container with new vector of values of components in
     * columns. Column number is equal to the number of the
     * non zero element in array rows.
     *
     * @param rows an array of number of component in column
     * @param c an array of new componet to add
     */
    public void update(int rows[], Component c[])
    {

        if(rows == null || rows.length == 0)
            throw new IllegalArgumentException("Defined null or empty row column container");

        int curr_rows[] = this.rows;//row_col_layout.GetRows();
        int col;
        int idx_w = getGridComponentCount() - 1;
        Btm b;
        int idx = 0;

        if(curr_rows.length > rows.length)
            col = curr_rows.length;
        else
            col = rows.length;

        for(int i = 0; i < col; i++)
        {
            if(i > rows.length)
            {
                for(int k = 0; k < curr_rows[i]; k++)
                {
                    remove(idx_w);
                    remove(0);
                    idx_w--;
                }
            } else {
                if(i > curr_rows.length  - 1)
                {
                    for(int k = 0; k < rows[i]; k++)
                    {
	                    add(b = new Btm(), 0);
	                    setListener(b);
	                    add(c[idx++]);
                    }
                } else {
                    if(curr_rows[i] > rows[i])
                    {
                        idx_w += rows[i];
                        for(int k = rows[i]; k < curr_rows[i]; k++)
                        {
                            if(idx_w > 0)
                                remove(idx_w);
                            remove(0);
                            idx_w--;
                        }
                    } else {
                        idx_w += curr_rows[i];
                        for(int k = curr_rows[i]; k < rows[i]; k++)
                        {
	                        add(c[idx++], idx_w);
	                        add(b = new Btm(), 0);
	                        setListener(b);
	                        idx_w++;
                        }
                    }
                }
            }
        }

        if(!rows.equals(this.rows))
        {
            this.rows = new int[rows.length];
            for(int i = 0; i < rows.length; i++)
                this.rows[i] = rows[i];
        }

        row_col_layout.SetRowColumn(rows);
        invalidate();
        validate();
    }

   /**
    * Removes component in (row, col) position from this container.
    *
     * @param row component index in the column
     * @param col column index of the component
    */
   public void removeComponent(int row, int col)
   {

        int idx = getComponentIndex(row, col);

        int b_idx = idx - getGridComponentCount();
        //remove component
        remove(idx);

        //remove resize button
        if(b_idx >= 0)
            this.remove(b_idx);
        else
            this.remove(b_idx + 1);

        int size = real_position.size();

        for(int j = 0; j < size; j+=2)
        {
            Point real_p = (Point)real_position.elementAt(j+1);
            if(row == real_p.y && col == real_p.x)
            {
                real_position.removeElementAt(j);
                real_position.removeElementAt(j);
                break;
            }
        }

        size = real_position.size();
        boolean found;

        for(int i = row+1; i <= rows[col - 1]; i++)
        {
            found = false;
            for(int j = 0; j < size; j+=2)
            {
                Point real_p = (Point)real_position.elementAt(j+1);
                if(i == real_p.y && col == real_p.x)
                {
                    found = true;
                    real_p.y--;
                    real_position.setElementAt(new Point(real_p.x, real_p.y), j + 1);
                    break;
                }
            }
            if(!found)
               setRealPosition(new Point(col, i+1), new Point(col, i));
        }
        rows[col-1]--;
        this.ph = this.pw = null;
        update();
   }

    /**
     * Removes the specified component from this container.
     *
     * @param c the component to be removed
     */
    public void removeComponent(Component c)
    {
        if(c == null) return;
        Point p = getComponentPosition(c);
        if(p == null) return;
        removeComponent(p.y, p.x);
    }

    /**
     * Returns the number of componet to add to this container.
     *
     * @return number of componet to add to this container
     */
    public int getComponentNumber()
    {
	    int num = 0;
	    for(int i = 0; i < rows.length && rows[i] != 0; i++)
	        num += rows[i];
	    return num;
    }

    /**
     * Get number of columns.
     *
     * @return number of columns
     */

    public int getColumns()
    {
        int col = 0;
        for(int c = 0; c < rows.length ; c++)
          if(rows[c] != 0)
            col++;
        return col;
    }

    /**
     * Get defined number of component in column col.
     *
     * @param col  number of column
     * @return number of component in column ith.
     */

    public int getComponentsInColumn(int col){return rows[col];}

    /**
     * Get defined number of component in columns
     *
     * @return an integer array ith element is number of component in columns ith
     */
    public int[] getComponentsInColumns(){return rows;}

    /**
     * Add component to the container. (row, col) position is automatic
     * evaluated.
     *
     * @param c component to add
     * @return index of the added component
     */
    public int splitContainer(Component c)
    {
        int i, j, idx = 1, col = 0, row = 0;
        boolean not_add = true;

        for(j = rows.length ; j <= rows.length * 4 && not_add; j++)
        {
            for(i = 0, idx = 0; i < rows.length; i++)
            {
                if (rows[i] < j && not_add) {
                    row = rows[i] + 1;
                    col = i+1;
                    not_add = false;
                }
                idx += rows[i];
            }
        }
        add(c, row, col);

        split_pos = new Point(col, row);
        update();
        return idx-1;
    }

    public Point getSplitPosition()
    {
        return split_pos;
    }

    public void resetSplitPosition()
    {
        split_pos = null;
    }

    public float[] getNormalizedHeight()
    {
      if(isMaximize())
        return ph;
      return row_col_layout.getPercentHeight();
    }

    public float[] getNormalizedWidth()
    {
      if(isMaximize())
        return ph;
      return row_col_layout.getPercentWidth();
    }

    public int[] getComponetNumInColumns()
    {
      return rows;
    }

    /**
     * Repaint all added component.
     */
    public void repaintAll()
    {
        for(int i = 0; i < getGridComponentCount(); i++)
	        getGridComponent(i).repaint();
    }

}

