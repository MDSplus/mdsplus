package jtraverser.editor;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.HashMap;
import java.util.Map.Entry;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSlider;
import javax.swing.JSpinner;
import javax.swing.JTable;
import javax.swing.JViewport;
import javax.swing.ScrollPaneConstants;
import javax.swing.SpinnerNumberModel;
import javax.swing.SwingConstants;
import javax.swing.UIManager;
import javax.swing.border.EmptyBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.TableModelEvent;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.DefaultTableModel;
import javax.swing.table.JTableHeader;
import javax.swing.table.TableColumn;
import jtraverser.dialogs.GraphPanel;
import mds.MdsException;
import mds.data.CTX;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_CA;
import mds.data.descriptor_a.NUMBERArray;

@SuppressWarnings("serial")
public class ArrayEditor extends Editor implements ChangeListener{
    public class RowNumberTable extends JTable implements ChangeListener, PropertyChangeListener{
        private class RowNumberRenderer extends DefaultTableCellRenderer{
            public RowNumberRenderer(){
                this.setHorizontalAlignment(SwingConstants.CENTER);
            }

            @Override
            public Component getTableCellRendererComponent(final JTable table_in, final Object value, final boolean isSelected, final boolean hasFocus, final int row, final int column) {
                if(table_in != null){
                    final JTableHeader header = table_in.getTableHeader();
                    if(header != null){
                        RowNumberTable.this.setForeground(header.getForeground());
                        RowNumberTable.this.setBackground(header.getBackground());
                        RowNumberTable.this.setFont(header.getFont());
                    }
                }
                // if(isSelected) RowNumberTable.this.setFont(RowNumberTable.this.getFont().deriveFont(Font.BOLD));
                this.setText((value == null) ? "" : value.toString());
                RowNumberTable.this.setBorder(UIManager.getBorder("TableHeader.cellBorder"));
                return this;
            }
        }
        private final JTable main;

        public RowNumberTable(final JTable table){
            this.main = table;
            this.main.addPropertyChangeListener(this);
            this.main.getModel().addTableModelListener(this);
            this.setFocusable(false);
            this.setAutoCreateColumnsFromModel(false);
            this.setSelectionModel(this.main.getSelectionModel());
            final TableColumn column = new TableColumn();
            column.setIdentifier(this);
            column.setHeaderValue("");
            this.addColumn(column);
            column.setCellRenderer(new RowNumberRenderer());
            this.getColumnModel().getColumn(0).setPreferredWidth(60);
            this.setPreferredScrollableViewportSize(this.getPreferredSize());
        }

        @Override
        public void addNotify() {
            super.addNotify();
            final Component c = this.getParent();
            if(c instanceof JViewport){
                final JViewport viewport = (JViewport)c;
                viewport.addChangeListener(this);
            }
        }

        @Override
        public int getRowCount() {
            return this.main.getRowCount();
        }

        @Override
        public int getRowHeight(final int row) {
            final int row_height = this.main.getRowHeight(row);
            if(row_height != super.getRowHeight(row)) super.setRowHeight(row, row_height);
            return row_height;
        }

        @Override
        public Object getValueAt(final int row, final int column) {
            return Integer.toString(row);
        }

        @Override
        public boolean isCellEditable(final int row, final int column) {
            return ArrayEditor.this.editable;
        }

        @Override
        public void propertyChange(final PropertyChangeEvent e) {
            if("selectionModel".equals(e.getPropertyName())) this.setSelectionModel(this.main.getSelectionModel());
            if("rowHeight".equals(e.getPropertyName())) this.repaint();
            if(!"model".equals(e.getPropertyName())) return;
            this.main.getModel().addTableModelListener(this);
            this.revalidate();
        }

        @Override
        public void setValueAt(final Object value, final int row, final int column) {/*stub*/}

        @Override
        public void stateChanged(final ChangeEvent e) {
            final JViewport viewport = (JViewport)e.getSource();
            final JScrollPane scrollPane = (JScrollPane)viewport.getParent();
            scrollPane.getVerticalScrollBar().setValue(viewport.getViewPosition().y);
        }

        @Override
        public void tableChanged(final TableModelEvent e) {
            this.revalidate();
        }
    }

    public static final boolean checkData(final Descriptor<?> data) {
        return(data instanceof NUMBERArray || data instanceof Descriptor_CA);
    }
    private final JTable             table;
    private final JTable             rows;
    private Thread                   updater;
    private HashMap<Integer, Number> changes;
    private JSlider                  slider;
    private JSpinner[]               coord_edit;
    private JPanel                   dimcontrol;

    public ArrayEditor(final boolean editable, final CTX ctx){
        this(null, editable, ctx);
    }

    public ArrayEditor(final Descriptor<?> data, final boolean editable, final CTX ctx){
        super(data, editable, ctx, 0);
        this.table = new JTable();
        this.table.setAutoResizeMode(JTable.AUTO_RESIZE_ALL_COLUMNS);
        this.table.setDoubleBuffered(true);
        this.rows = new RowNumberTable(this.table);
        final JScrollPane scroll_panel = new JScrollPane(this.table);
        this.setLayout(new BorderLayout());
        this.add(scroll_panel, BorderLayout.CENTER);
        scroll_panel.setRowHeaderView(this.rows);
        scroll_panel.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
        scroll_panel.setCorner(ScrollPaneConstants.UPPER_LEFT_CORNER, this.rows.getTableHeader());
        this.setPreferredSize(new Dimension(240, 640));
        if(!Editor.isNoData(this.data)) this.setData(data);
        final JButton plot = new JButton("Plot");
        plot.addActionListener(new ActionListener(){
            @Override
            public void actionPerformed(final ActionEvent arg0) {
                final DefaultTableModel model = (DefaultTableModel)ArrayEditor.this.table.getModel();
                final float[] dat = new float[model.getRowCount()];
                for(int i = 0; i < dat.length; i++)
                    dat[i] = ((Number)model.getValueAt(i, 0)).floatValue();
                GraphPanel.newPlot(dat, null, JOptionPane.getRootFrame(), ArrayEditor.this.getTitle());
            }
        });
        this.add(plot, BorderLayout.SOUTH);
    }

    @Override
    protected void finalize() throws Throwable {
        this.interrupt();
        super.finalize();
    }

    @Override
    public Descriptor<?> getData() throws MdsException {
        if(!(this.data instanceof NUMBERArray)) return this.data;
        @SuppressWarnings("unchecked")
        final NUMBERArray<Number> new_data = (NUMBERArray<Number>)this.data;
        for(final Entry<Integer, Number> entry : this.changes.entrySet())
            new_data.setAtomic(entry.getKey().intValue(), entry.getValue());
        return this.data = new_data;
    }

    private final String getTitle() {
        if(this.dimcontrol == null) return "Array";
        final StringBuilder sb = new StringBuilder(256).append("Slice( ");
        for(int i = 0; i < this.coord_edit.length; i++){
            if(i > 0) sb.append(" , ");
            if(this.slider.getValue() == i) sb.append(':');
            else sb.append(this.coord_edit[i].getValue());
        }
        return sb.append(" )").toString();
    }

    public final boolean hasChanged() {
        return !this.changes.isEmpty();
    }

    @Override
    public final void interrupt() {
        if(this.updater != null && this.updater.isAlive()) this.updater.interrupt();
    }

    @Override
    public final void reset(final boolean hard) {
        if(this.updater != null && this.updater.isAlive()) this.updater.interrupt();
        if(Editor.isNoData(this.data)) return;
        if(hard){
            if(this.dimcontrol != null) this.remove(this.dimcontrol);
            final int[] dims = this.data.getShape();
            if(dims.length > 1){
                final int dim = 0;
                this.dimcontrol = new JPanel(new BorderLayout());
                this.dimcontrol.add(this.slider = new JSlider(0, dims.length - 1, dim), BorderLayout.WEST);
                this.slider.setMajorTickSpacing(1);
                this.slider.setPaintTicks(true);
                this.slider.setPaintLabels(true);
                this.slider.setOrientation(SwingConstants.VERTICAL);
                this.slider.setToolTipText("Slice array along this dimension. Time is the last dimension.");
                this.slider.addChangeListener(this);
                this.slider.setBorder(new EmptyBorder(3, 0, 3, 0));
                this.slider.setPreferredSize(new Dimension(60, 20 * dims.length - 6));
                this.slider.setInverted(true);
                final JPanel coord = new JPanel(new GridLayout(dims.length, 2));
                coord.setMinimumSize(new Dimension(180, 0));
                this.dimcontrol.add(coord, BorderLayout.CENTER);
                this.coord_edit = new JSpinner[dims.length];
                for(int i = 0; i < dims.length; i++){
                    final JSpinner ce = this.coord_edit[i] = new JSpinner(new SpinnerNumberModel(0, 0, dims[i] - 1, 1));
                    ce.addChangeListener(this);
                    coord.add(ce);
                    final JLabel label = new JLabel(String.valueOf(dims[i]));
                    label.setHorizontalAlignment(SwingConstants.CENTER);
                    coord.add(label);
                }
                this.coord_edit[dim].setEnabled(false);
                this.add(this.dimcontrol, BorderLayout.NORTH);
            }else{
                this.dimcontrol = null;
                this.slider = null;
                this.coord_edit = null;
            }
            this.validate();
            this.repaint();
        }
        if(!(this.data instanceof NUMBERArray)) return;
        final NUMBERArray<?> narray = (NUMBERArray<?>)this.data;
        final int[] dims = narray.getShape();
        this.changes = new HashMap<Integer, Number>();
        final TableColumn column = ArrayEditor.this.rows.getColumn(ArrayEditor.this.rows);
        final JTableHeader header = ArrayEditor.this.rows.getTableHeader();
        final DefaultTableModel model = new DefaultTableModel(){
            @Override
            public boolean isCellEditable(final int row, final int col) {
                return ArrayEditor.this.editable;
            }

            @Override
            public final void setValueAt(final Object value, final int row, final int col) {
                if(col > 0) return;
                try{
                    final Number number = narray.parse(value.toString());
                    ArrayEditor.this.changes.put(Integer.valueOf(row), number);
                    super.setValueAt(number, row, col);
                }catch(final Exception e){
                    System.err.println(e + ": " + e.getMessage());
                    ArrayEditor.this.setToolTipText(e.getMessage());
                }
            }
        };
        model.addColumn(this.data.getDTypeName());
        this.table.setModel(model);
        this.table.putClientProperty("terminateEditOnFocusLost", Boolean.TRUE);
        final int offset, inc, dim;
        if(this.slider == null){
            dim = offset = 0;
            inc = 1;
        }else{
            dim = this.slider.getValue();
            final int[] coord = new int[dims.length];
            for(int i = 0; i < dims.length; i++)
                coord[i] = ((Integer)this.coord_edit[i].getValue()).intValue();
            int toffset = 0, tinc = 1;
            for(int i = 0, j = 1; i < dims.length; j *= dims[i++]){
                if(dim == i) tinc = j;
                else toffset += j * coord[i];
            }
            offset = toffset;
            inc = tinc;
        }
        (this.updater = new Thread("ArrayEditor.updater"){
            {
                this.setDaemon(true);
                this.setPriority(Thread.MIN_PRIORITY);
            }

            @Override
            public final void run() {
                final int n = dims.length > 0 ? dims[dim] : 1;
                for(int i = 0; i < n; i++){
                    if(i % 10000 == 0 && n > 10000){
                        column.setHeaderValue(new StringBuilder(4).append(100 * i / n).append('%').toString());
                        header.repaint();
                        synchronized(this){
                            if(this.isInterrupted()) return;
                        }
                    }
                    model.addRow(new Number[]{narray.get(i * inc + offset)});
                }
                column.setHeaderValue("");
                header.repaint();
            }
        }).start();
    }

    @Override
    public final void setData(Descriptor<?> data) {
        if(data instanceof Descriptor_CA) data = ((Descriptor_CA)data).unpack();
        this.data = data;
        this.reset(true);
    }

    @Override
    public final void stateChanged(final ChangeEvent e) {
        if(this.slider != null){
            final int dim = this.slider.getValue();
            for(int i = 0; i < this.coord_edit.length; i++)
                this.coord_edit[i].setEnabled(i != dim);
        }
        this.reset(false);
    }
}