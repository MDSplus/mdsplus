package mds.data.descriptor_r.function;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.OPC;
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
public abstract class BUILD extends Function{
    public static final boolean coversOpCode(final short opcode) {
        switch(opcode){
            case OPC.OpcBuildPath:
            case OPC.OpcBuildEvent:
            case OPC.OpcBuildAction:
            case OPC.OpcMakeAction:
            case OPC.OpcBuildCall:
            case OPC.OpcMakeCall:
            case OPC.OpcBuildCondition:
            case OPC.OpcMakeCondition:
            case OPC.OpcBuildConglom:
            case OPC.OpcMakeConglom:
            case OPC.OpcBuildDependency:
            case OPC.OpcMakeDependency:
            case OPC.OpcBuildDim:
            case OPC.OpcMakeDim:
            case OPC.OpcBuildDispatch:
            case OPC.OpcMakeDispatch:
            case OPC.OpcBuildFunction:
            case OPC.OpcMakeFunction:
            case OPC.OpcBuildMethod:
            case OPC.OpcMakeMethod:
            case OPC.OpcBuildOpaque:
            case OPC.OpcMakeOpaque:
            case OPC.OpcBuildParam:
            case OPC.OpcMakeParam:
            case OPC.OpcBuildProcedure:
            case OPC.OpcMakeProcedure:
            case OPC.OpcBuildProgram:
            case OPC.OpcMakeProgram:
            case OPC.OpcBuildRange:
            case OPC.OpcMakeRange:
            case OPC.OpcBuildRoutine:
            case OPC.OpcMakeRoutine:
            case OPC.OpcBuildSignal:
            case OPC.OpcMakeSignal:
            case OPC.OpcBuildSlope:
            case OPC.OpcMakeSlope:
            case OPC.OpcBuildWithError:
            case OPC.OpcMakeWithError:
            case OPC.OpcBuildWithUnits:
            case OPC.OpcMakeWithUnits:
            case OPC.OpcBuildWindow:
            case OPC.OpcMakeWindow:
                return true;
        }
        return false;
    }

    protected BUILD(final ByteBuffer b){
        super(b);
    }

    @Override
    public final DATA<?> getData_() throws MdsException {
        switch(this.getOpCode()){
            case OPC.OpcBuildPath:
                return new Path(this.getArguments()).getData();
            case OPC.OpcBuildEvent:
                return new Event(this.getArguments()).getData();
            case OPC.OpcBuildAction:
            case OPC.OpcMakeAction:
                return new Action(this.getArguments()).getData();
            case OPC.OpcBuildCall:
            case OPC.OpcMakeCall:
                return new Call(this.getArguments()).getData();
            case OPC.OpcBuildCondition:
            case OPC.OpcMakeCondition:
                return new Condition(this.getArguments()).getData();
            case OPC.OpcBuildConglom:
            case OPC.OpcMakeConglom:
                return new Conglom(this.getArguments()).getData();
            case OPC.OpcBuildDependency:
            case OPC.OpcMakeDependency:
                return new Dependency(this.getArguments()).getData();
            case OPC.OpcBuildDim:
            case OPC.OpcMakeDim:
                return new Dim(this.getArguments()).getData();
            case OPC.OpcBuildDispatch:
            case OPC.OpcMakeDispatch:
                return new Dispatch(this.getArguments()).getData();
            case OPC.OpcBuildFunction:
            case OPC.OpcMakeFunction:
                return new Function(this.getArguments()).getData();
            case OPC.OpcBuildMethod:
            case OPC.OpcMakeMethod:
                return new Method(this.getArguments()).getData();
            case OPC.OpcBuildOpaque:
            case OPC.OpcMakeOpaque:
                return new Opaque(this.getArguments()).getData();
            case OPC.OpcBuildParam:
            case OPC.OpcMakeParam:
                return new Param(this.getArguments()).getData();
            case OPC.OpcBuildProcedure:
            case OPC.OpcMakeProcedure:
                return new Procedure(this.getArguments()).getData();
            case OPC.OpcBuildProgram:
            case OPC.OpcMakeProgram:
                return new Program(this.getArguments()).getData();
            case OPC.OpcBuildRange:
            case OPC.OpcMakeRange:
                return new Range(this.getArguments()).getData();
            case OPC.OpcBuildRoutine:
            case OPC.OpcMakeRoutine:
                return new Routine(this.getArguments()).getData();
            case OPC.OpcBuildSignal:
            case OPC.OpcMakeSignal:
                return new Signal(this.getArguments()).getData();
            case OPC.OpcBuildSlope:
            case OPC.OpcMakeSlope:
                return new Slope(this.getArguments()).getData();
            case OPC.OpcBuildWindow:
            case OPC.OpcMakeWindow:
                return new Window(this.getArguments()).getData();
            case OPC.OpcBuildWithError:
            case OPC.OpcMakeWithError:
                return new With_Error(this.getArguments()).getData();
            case OPC.OpcBuildWithUnits:
            case OPC.OpcMakeWithUnits:
                return new With_Units(this.getArguments()).getData();
            default:
                return Missing.NEW;
        }
    }
    // TODO: getData should return the Rdesc
    // TODO: getLocal should veryfy local of args inherited from Function
}
