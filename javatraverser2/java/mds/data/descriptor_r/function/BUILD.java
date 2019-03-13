package mds.data.descriptor_r.function;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.OPC;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Action;
import mds.data.descriptor_r.Call;
import mds.data.descriptor_r.Condition;
import mds.data.descriptor_r.Conglom;
import mds.data.descriptor_r.Dependency;
import mds.data.descriptor_r.Dim;
import mds.data.descriptor_r.Dispatch;
import mds.data.descriptor_r.Function;
import mds.data.descriptor_r.Method;
import mds.data.descriptor_r.Opaque;
import mds.data.descriptor_r.Param;
import mds.data.descriptor_r.Procedure;
import mds.data.descriptor_r.Program;
import mds.data.descriptor_r.Range;
import mds.data.descriptor_r.Routine;
import mds.data.descriptor_r.Signal;
import mds.data.descriptor_r.Slope;
import mds.data.descriptor_r.Window;
import mds.data.descriptor_r.With_Error;
import mds.data.descriptor_r.With_Units;
import mds.data.descriptor_s.Event;
import mds.data.descriptor_s.Missing;
import mds.data.descriptor_s.Path;

@SuppressWarnings("deprecation")
public class BUILD extends Function{
    public static final boolean coversOpCode(final OPC opcode) {
        switch(opcode){
            case OpcBuildPath:
            case OpcBuildEvent:
            case OpcBuildAction:
            case OpcMakeAction:
            case OpcBuildCall:
            case OpcMakeCall:
            case OpcBuildCondition:
            case OpcMakeCondition:
            case OpcBuildConglom:
            case OpcMakeConglom:
            case OpcBuildDependency:
            case OpcMakeDependency:
            case OpcBuildDim:
            case OpcMakeDim:
            case OpcBuildDispatch:
            case OpcMakeDispatch:
            case OpcBuildFunction:
            case OpcMakeFunction:
            case OpcBuildMethod:
            case OpcMakeMethod:
            case OpcBuildOpaque:
            case OpcMakeOpaque:
            case OpcBuildParam:
            case OpcMakeParam:
            case OpcBuildProcedure:
            case OpcMakeProcedure:
            case OpcBuildProgram:
            case OpcMakeProgram:
            case OpcBuildRange:
            case OpcMakeRange:
            case OpcBuildRoutine:
            case OpcMakeRoutine:
            case OpcBuildSignal:
            case OpcMakeSignal:
            case OpcBuildSlope:
            case OpcMakeSlope:
            case OpcBuildWithError:
            case OpcMakeWithError:
            case OpcBuildWithUnits:
            case OpcMakeWithUnits:
            case OpcBuildWindow:
            case OpcMakeWindow:
                return true;
            default:
                return false;
        }
    }

    public static final BUILD deserialize(final ByteBuffer b) {
        switch(b.getShort(b.getInt(Descriptor._ptrI))){
            default:
                return new BUILD(b);
        }
    }

    protected BUILD(final ByteBuffer b){
        super(b);
    }

    @Override
    public final DATA<?> getData_() throws MdsException {
        switch(this.getOpCode()){
            case OpcBuildPath:
                return new Path(this.getArguments()).getData();
            case OpcBuildEvent:
                return new Event(this.getArguments()).getData();
            case OpcBuildAction:
            case OpcMakeAction:
                return new Action(this.getArguments()).getData();
            case OpcBuildCall:
            case OpcMakeCall:
                return new Call(this.getArguments()).getData();
            case OpcBuildCondition:
            case OpcMakeCondition:
                return new Condition(this.getArguments()).getData();
            case OpcBuildConglom:
            case OpcMakeConglom:
                return new Conglom(this.getArguments()).getData();
            case OpcBuildDependency:
            case OpcMakeDependency:
                return new Dependency(this.getArguments()).getData();
            case OpcBuildDim:
            case OpcMakeDim:
                return new Dim(this.getArguments()).getData();
            case OpcBuildDispatch:
            case OpcMakeDispatch:
                return new Dispatch(this.getArguments()).getData();
            case OpcBuildFunction:
            case OpcMakeFunction:
                return new Function(this.getArguments()).getData();
            case OpcBuildMethod:
            case OpcMakeMethod:
                return new Method(this.getArguments()).getData();
            case OpcBuildOpaque:
            case OpcMakeOpaque:
                return new Opaque(this.getArguments()).getData();
            case OpcBuildParam:
            case OpcMakeParam:
                return new Param(this.getArguments()).getData();
            case OpcBuildProcedure:
            case OpcMakeProcedure:
                return new Procedure(this.getArguments()).getData();
            case OpcBuildProgram:
            case OpcMakeProgram:
                return new Program(this.getArguments()).getData();
            case OpcBuildRange:
            case OpcMakeRange:
                return new Range(this.getArguments()).getData();
            case OpcBuildRoutine:
            case OpcMakeRoutine:
                return new Routine(this.getArguments()).getData();
            case OpcBuildSignal:
            case OpcMakeSignal:
                return new Signal(this.getArguments()).getData();
            case OpcBuildSlope:
            case OpcMakeSlope:
                return new Slope(this.getArguments()).getData();
            case OpcBuildWindow:
            case OpcMakeWindow:
                return new Window(this.getArguments()).getData();
            case OpcBuildWithError:
            case OpcMakeWithError:
                return new With_Error(this.getArguments()).getData();
            case OpcBuildWithUnits:
            case OpcMakeWithUnits:
                return new With_Units(this.getArguments()).getData();
            default:
                return Missing.NEW;
        }
    }
    // TODO: getData should return the Rdesc
    // TODO: getLocal should veryfy local of args inherited from Function
}
