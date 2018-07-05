package jtraverser.editor;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.util.StringTokenizer;
import javax.swing.BorderFactory;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import mds.MdsException;
import mds.data.CTX;
import mds.data.OPC;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_a.CStringArray;
import mds.data.descriptor_r.Function;
import mds.data.descriptor_r.function.Fun;
import mds.data.descriptor_s.CString;

@SuppressWarnings("serial")
public final class PythonEditor extends Editor{
    static final short OPC_FUN = 162;

    public static final boolean checkData(final Descriptor<?> data) {
        if(data instanceof Function && ((Function)data).getOpCode() == OPC.OpcFun){
            final Descriptor<?>[] args = ((Function)data).getArguments();
            try{
                if(args != null && args.length > 2 && args[1] != null && (args[1] instanceof CString) && ((CString)args[1]).toString() != null && ((CString)args[1]).toString().toUpperCase().equals("PY")) return true;
            }catch(final Exception exc){
                MdsException.stderr("DataEditor.checkUsrData", exc);
            }
        }
        return false;
    }
    boolean    default_scroll;
    String     program;
    String     retVar;
    int        rows = 7, columns = 20;
    JTextArea  text_area;
    JTextField text_field;

    public PythonEditor(final boolean editable, final CTX ctx){
        this(null, editable, ctx);
    }

    public PythonEditor(final Function function, final boolean editable, final CTX ctx){
        super(function, editable, ctx, 0);
        JScrollPane scroll_pane;
        this.default_scroll = true;
        if(function != null){
            this.getProgram(function);
        }else{
            this.program = "";
            this.retVar = "";
        }
        this.text_area = new JTextArea(this.rows, this.columns);
        this.text_area.setEditable(editable);
        this.text_area.setText(this.program);
        this.text_field = new JTextField(10);
        this.text_field.setEditable(editable);
        this.text_field.setText(this.retVar);
        final Dimension d = this.text_area.getPreferredSize();
        d.height += 20;
        d.width += 20;
        final JPanel jp = new JPanel();
        jp.setLayout(new BorderLayout());
        final JPanel jp1 = new JPanel();
        jp1.setLayout(new BorderLayout());
        jp1.setBorder(BorderFactory.createTitledBorder("Return Variable"));
        jp1.add(this.text_field);
        jp.add(jp1, BorderLayout.NORTH);
        final JPanel jp2 = new JPanel();
        jp2.setLayout(new BorderLayout());
        jp2.setBorder(BorderFactory.createTitledBorder("Program"));
        scroll_pane = new JScrollPane(this.text_area);
        scroll_pane.setPreferredSize(d);
        jp2.add(scroll_pane);
        jp.add(jp2, BorderLayout.CENTER);
        this.setLayout(new BorderLayout());
        this.add(jp, BorderLayout.CENTER);
    }

    @Override
    public final Descriptor<?> getData() {
        final String programTxt = this.text_area.getText();
        if(programTxt == null || programTxt.equals("")) return null;
        final StringTokenizer st = new StringTokenizer(programTxt, "\n");
        final String[] lines = new String[st.countTokens()];
        int idx = 0;
        int maxLen = 0;
        while(st.hasMoreTokens()){
            lines[idx] = st.nextToken();
            if(maxLen < lines[idx].length()) maxLen = lines[idx].length();
            idx++;
        }
        for(int i = 0; i < lines.length; i++){
            final int len = lines[i].length();
            for(int j = 0; j < maxLen - len; j++)
                lines[i] += " ";
        }
        final CStringArray stArr = new CStringArray(lines);
        final String retVarTxt = this.text_field.getText();
        Descriptor<?> Args[];
        if(retVarTxt == null || retVarTxt.equals("")) Args = new Descriptor[]{null, new CString("Py"), stArr};
        else Args = new Descriptor[]{null, new CString("Py"), stArr, new CString(retVarTxt)};
        return new Fun(Args);
    }

    private final void getProgram(final Function function) {
        final Descriptor<?>[] args = function.getArguments();
        if(args.length <= 2 || args[2] == null) this.retVar = "";
        else this.retVar = args[2].toString();
        if(args.length <= 1 || args[1] == null) this.program = "";
        else if(args[1] instanceof CStringArray) this.program = ((CStringArray)args[2]).join_by("\n");
        else this.program = args[1].toString();
    }

    @Override
    public final void reset(final boolean hard) {
        this.text_area.setText(this.program);
        this.text_field = new JTextField(this.retVar);
    }
}
