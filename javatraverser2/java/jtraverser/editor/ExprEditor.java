package jtraverser.editor;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Insets;
import java.awt.Toolkit;
import java.awt.datatransfer.StringSelection;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import javax.swing.JButton;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.text.JTextComponent;
import mds.MdsException;
import mds.data.CTX;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.CString;

@SuppressWarnings("serial")
public class ExprEditor extends Editor{
    protected final class EvalPopupMenu extends JPopupMenu implements MouseListener{
        private class EvalActionListener implements ActionListener{
            private final String evalexpr;

            EvalActionListener(final String evalexpr){
                this.evalexpr = evalexpr;
            }

            @Override
            public void actionPerformed(final ActionEvent e) {
                ((JPopupMenu)((Component)e.getSource()).getParent()).setVisible(false);
                String aeeval;
                try{
                    final Descriptor<?> aedata = ExprEditor.this.getMds().getDescriptor(ExprEditor.this.ctx, this.evalexpr, new CString(EvalPopupMenu.this.text.getText()));
                    if(aedata == null) aeeval = "no data";
                    else aeeval = aedata.toString();
                }catch(final MdsException de){
                    aeeval = de.getMessage();
                }
                Toolkit.getDefaultToolkit().getSystemClipboard().setContents(new StringSelection(aeeval), null);
                JOptionPane.showMessageDialog(ExprEditor.this, "<html><body><p style='width: 360px;'>" + aeeval + "</p></body></html>", "Evaluated Data", JOptionPane.PLAIN_MESSAGE);
            }
        }
        protected final JTextComponent text;

        public EvalPopupMenu(final JTextComponent text){
            this.text = text;
            JMenuItem item;
            this.add(item = new JMenuItem("DATA($)"));
            item.addActionListener(new EvalActionListener("DATA(COMPILE($))"));
            this.add(item = new JMenuItem("RAW_OF($)"));
            item.addActionListener(new EvalActionListener("RAW_OF(COMPILE($))"));
            this.add(item = new JMenuItem("DIM_OF($)"));
            item.addActionListener(new EvalActionListener("DIM_OF(COMPILE($))"));
        }

        @Override
        public void mouseClicked(final MouseEvent arg0) {/*stub*/}

        @Override
        public void mouseEntered(final MouseEvent e) {/*stub*/}

        @Override
        public void mouseExited(final MouseEvent e) {/*stub*/}

        @Override
        public void mousePressed(final MouseEvent e) {
            if(e.isPopupTrigger()) EvalPopupMenu.this.show(e.getComponent(), e.getX(), e.getY());
        }

        @Override
        public void mouseReleased(final MouseEvent e) {
            this.mousePressed(e);
        }
    }
    private final boolean        default_to_string;
    private final JButton        left, right;
    private final JTextComponent text_edit;
    private final JScrollPane    scroll_pane;
    private String               expr;
    private boolean              quotes_added;

    public ExprEditor(final boolean editable, final CTX ctx, final boolean default_to_string, final boolean asField){
        this((Descriptor<?>)null, editable, ctx, null, default_to_string, asField);
    }

    public ExprEditor(final boolean editable, final CTX ctx, final String tooltip, final boolean default_to_string, final boolean asField){
        this(null, editable, ctx, tooltip, default_to_string, asField);
    }

    public ExprEditor(final Descriptor<?> data, final boolean editable, final CTX ctx){
        this(data, editable, ctx, null, (data != null && data instanceof CString), true);
    }

    public ExprEditor(final Descriptor<?> data, final boolean editable, final CTX ctx, final String tooltip, final boolean default_to_string, final boolean isField){
        super(data, editable, ctx, 0);
        this.default_to_string = default_to_string;
        this.setLayout(new BorderLayout());
        if(isField){// default_scroll
            this.text_edit = new JTextArea(7, 24);
            this.text_edit.setEditable(this.editable);
            this.text_edit.addMouseListener(new EvalPopupMenu(this.text_edit));
            ((JTextArea)this.text_edit).setTabSize(2);
            ((JTextArea)this.text_edit).setLineWrap(true);
            final Dimension d = this.text_edit.getPreferredSize();
            d.height += 20;
            d.width += 20;
            this.scroll_pane = new JScrollPane(this.text_edit);
            this.scroll_pane.setPreferredSize(d);
        }else{
            this.scroll_pane = null;
            this.text_edit = new JTextField(24);
            this.text_edit.setEditable(this.editable);
            this.text_edit.addMouseListener(new EvalPopupMenu(this.text_edit));
        }
        this.left = new JButton("\"");
        this.right = new JButton("\"");
        final Insets insets = new Insets(0, 0, 0, 0);
        this.left.setMargin(insets);
        this.right.setMargin(insets);
        final ActionListener leftright = new ActionListener(){
            @Override
            public void actionPerformed(final ActionEvent e) {
                if(!ExprEditor.this.editable) return;
                ExprEditor.this.remove(ExprEditor.this.left);
                ExprEditor.this.remove(ExprEditor.this.right);
                ExprEditor.this.quotes_added = false;
                ExprEditor.this.expr = ExprEditor.this.text_edit.getText();
                ExprEditor.this.expr = "\"" + ExprEditor.this.expr + "\"";
                ExprEditor.this.text_edit.setText(ExprEditor.this.expr);
                ExprEditor.this.validate();
                ExprEditor.this.repaint();
            }
        };
        this.left.addActionListener(leftright);
        this.right.addActionListener(leftright);
        if(tooltip != null){
            this.setToolTipText(tooltip);
            this.text_edit.setToolTipText(tooltip);
        }
        this.setData(data);
    }

    @Override
    public final Descriptor<?> getData() throws MdsException {
        this.expr = this.text_edit.getText().trim();
        if(this.expr.isEmpty()) return null;
        if(this.quotes_added) return new CString(this.expr);
        return this.getMds().getDescriptor(this.ctx, "COMPILE($)", new CString(this.expr));
    }

    @Override
    public final void reset(final boolean hard) {
        this.removeAll();
        this.quotes_added = false;
        if(this.data instanceof CString){
            this.expr = this.data.toString();
            this.quotes_added = true;
        }else if(Editor.isNoData(this.data)){
            this.expr = null;
            this.quotes_added = this.default_to_string;
        }else this.expr = this.data.getSize() > 0x80000 ? this.data.toString() : this.data.decompileX();
        this.text_edit.setText(this.expr);
        if(this.scroll_pane == null) this.add(this.text_edit, BorderLayout.CENTER);
        else this.add(this.scroll_pane, BorderLayout.CENTER);
        if(this.quotes_added){
            this.add(this.left, BorderLayout.LINE_START);
            this.add(this.right, BorderLayout.LINE_END);
        }
        this.validate();
        this.repaint();
    }

    @Override
    public final void setData(final Descriptor<?> data) {
        this.data = data;
        this.reset(false);
    }
}
