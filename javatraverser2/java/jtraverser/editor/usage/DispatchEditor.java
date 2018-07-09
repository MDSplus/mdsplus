package jtraverser.editor.usage;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.Window;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import jtraverser.editor.Editor;
import jtraverser.editor.ExprEditor;
import mds.MdsException;
import mds.data.CTX;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Dispatch;

@SuppressWarnings("serial")
public class DispatchEditor extends TextEditor{
    public final class DispatchEdt extends Editor{
        private final JPanel jp;

        public DispatchEdt(final Descriptor<?> data){
            super(data, DispatchEditor.this.editable, DispatchEditor.this.ctx, 4);
            this.jp = new JPanel();
            this.jp.setLayout(new GridLayout(4, 1, 0, 0));
            this.jp.add(this.edit[0] = Editor.addLabel("Server", new ExprEditor(this.editable, this.ctx, DispatchEditor.server_tip, true, false)));
            this.jp.add(this.edit[1] = Editor.addLabel("Phase", new ExprEditor(this.editable, this.ctx, DispatchEditor.phase_tip, true, false)));
            this.jp.add(this.edit[2] = Editor.addLabel("When", new ExprEditor(this.editable, this.ctx, DispatchEditor.when_tip, false, false)));
            this.jp.add(this.edit[3] = Editor.addLabel("Completion", new ExprEditor(this.editable, this.ctx, DispatchEditor.completion_tip, true, false)));
            this.setLayout(new BorderLayout());
            this.add(this.jp, BorderLayout.NORTH);
        }

        @Override
        public final Dispatch getData() throws MdsException {
            final byte type = DispatchEditor.this.mode2dtype();
            return new Dispatch(type, this.edit[0].getData(), this.edit[1].getData(), this.edit[2].getData(), this.edit[3].getData());
        }

        @Override
        public final void setData(final Descriptor<?> data) {
            this.data = data;
            if(data instanceof Dispatch && ((Dispatch)data).getType() != Dispatch.SCHED_SEQ) JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), "The current dispatch type is currently not implemented. Use Sequential instead.", "Dispatch Type", JOptionPane.PLAIN_MESSAGE);
            this.setDescR();
        }
    }
    private static final String server_tip     = "The server part specifies the server that should execute the action. For tcpip based action servers this field should contain a string such as host:port where host is the tcpip hostname of the computer where the action server is running and the port is the port number on which the action server is listening for actions to perform. For DECNET based action servers (OpenVMS only), this should be a string such as host::object where the host is the DECNET node name and the object is the DECNET object name of the action server.";
    private static final String phase_tip      = "The phase part of a dispatch item is either the name or number corresponding to the phase of the experiment cycle. These would normally be phases such as 'store', 'init','analysis' etc. but the names and numbers of the phases can be customized by the MDSplus system administrator by modifying the TDI function phase_table()";
    private static final String when_tip       = "The when part normally contains either an integer value or an expression which evaluates to an integer value representing a sequence number. When the dispatcher (implemented by a set of mdstcl dispatch commands) builds a dispatching table, it finds all the actions defined in a tree and then sorts these actions first by phase and then by sequence number. Actions are then dispatched to servers during a phase in order of their sequence numbers (except for actions with sequence numbers less than or equal to zero which are not dispatched). There is a special case for the when part which enables you to set up dependencies on other actions. If instead of specifying a sequence number for the when part, you specify an expression which references other action nodes in the tree, this action will not be dispatched until all action nodes referenced in the expression have completed. When all the actions referenced in the expression have completed, the expression is then evaluated substituting the completion status of the referenced actions instead of the action node itself. If the result of the evaluation yields an odd number (low bit set) then this action will be dispatched. If the result is an even value then this action is not dispatched but instead assigned a failure completion status in case other actions have when expressions refering to it. Using this mechanism you can configure fairly complex conditional dispatching.";
    private static final String completion_tip = "The completion part can hold a string defining the name of an MDSplus event to be declared upon completion of this action. These events are often used to trigger updates on visualization tools such as dwscope when this action completes indicating availability of the data.";

    public DispatchEditor(final boolean editable, final CTX ctx, final Window window){
        this(null, editable, ctx, window);
    }

    public DispatchEditor(final Descriptor<?> data, final boolean editable, final CTX ctx, final Window window){
        super(data, editable, ctx, window, "Dispatch", false, "Asynchron (not implemented)", "Sequential", "Conditional (not implemented)");
    }

    @Override
    protected final boolean addExtraEditor() {
        if(this.curr_mode_idx < this.mode_idx_usr) return false;
        this.editor.add(this.data_edit = new DispatchEdt(this.data));
        return true;
    }

    @Override
    protected final boolean checkUsrData() {
        if(!(this.data instanceof Dispatch)) return false;
        final Dispatch ddata = (Dispatch)this.data;
        this.mode_idx = this.dtype2mode(ddata.getType());
        return true;
    }

    private final int dtype2mode(int dtype) {
        if(dtype > 4) dtype = 4;
        return dtype + this.mode_idx_usr - 1;
    }

    private final byte mode2dtype() {
        return (byte)(this.curr_mode_idx - this.mode_idx_usr + 1);
    }
}