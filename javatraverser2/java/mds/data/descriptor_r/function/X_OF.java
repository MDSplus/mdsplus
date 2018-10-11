package mds.data.descriptor_r.function;

import java.nio.ByteBuffer;
import mds.data.OPC;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Function;
import mds.data.descriptor_s.Int32;

public class X_OF extends Function{
    public static final class DimOf extends X_OF{
        public DimOf(final ByteBuffer b){
            super(b);
        }

        public DimOf(final Descriptor<?> arg){
            super(OPC.OpcDimOf, arg);
        }

        public DimOf(final Descriptor<?> arg, final int idx){
            super(OPC.OpcDimOf, arg, idx);
        }
    }
    public static final class HelpOf extends X_OF{
        public HelpOf(final ByteBuffer b){
            super(b);
        }

        public HelpOf(final Descriptor<?> arg){
            super(OPC.OpcHelpOf, arg);
        }
    }
    public static final class UnitsOf extends X_OF{
        public UnitsOf(final ByteBuffer b){
            super(b);
        }

        public UnitsOf(final Descriptor<?> arg){
            super(OPC.OpcUnitsOf, arg);
        }
    }

    public static final boolean coversOpCode(final short opcode) {
        switch(opcode){
            case OPC.OpcArgOf:
            case OPC.OpcAxisOf:
            case OPC.OpcBeginOf:
            case OPC.OpcClassOf:
            case OPC.OpcCompletionMessageOf:
            case OPC.OpcCompletionOf:
            case OPC.OpcConditionOf:
            case OPC.OpcDimOf:
            case OPC.OpcDispatchOf:
            case OPC.OpcDscptrOf:
            case OPC.OpcEndOf:
            case OPC.OpcErrorlogsOf:
            case OPC.OpcErrorOf:
            case OPC.OpcHelpOf:
            case OPC.OpcIdentOf:
            case OPC.OpcImageOf:
            case OPC.OpcInterruptOf:
            case OPC.OpcKindOf:
            case OPC.OpcLanguageOf:
            case OPC.OpcMethodOf:
            case OPC.OpcModelOf:
            case OPC.OpcNameOf:
            case OPC.OpcNdescOf:
            case OPC.OpcObjectOf:
            case OPC.OpcPerformanceOf:
            case OPC.OpcPhaseOf:
            case OPC.OpcProcedureOf:
            case OPC.OpcProgramOf:
            case OPC.OpcQualifiersOf:
            case OPC.OpcRawOf:
            case OPC.OpcRoutineOf:
            case OPC.OpcSizeOf:
            case OPC.OpcSlopeOf:
            case OPC.OpcTaskOf:
            case OPC.OpcTimeoutOf:
            case OPC.OpcUnitsOf:
            case OPC.OpcValidationOf:
            case OPC.OpcValueOf:
            case OPC.OpcWhenOf:
            case OPC.OpcWindowOf:
                return true;
        }
        return false;
    }

    public static final X_OF deserialize(final ByteBuffer b) {
        switch(b.getShort(b.getInt(Descriptor._ptrI))){
            case OPC.OpcDimOf:
                return new DimOf(b);
            case OPC.OpcHelpOf:
                return new HelpOf(b);
            case OPC.OpcUnitsOf:
                return new UnitsOf(b);
            default:
                return new X_OF(b);
        }
    }

    protected X_OF(final ByteBuffer b){
        super(b);
    }

    private X_OF(final short opcode, final Descriptor<?> arg){
        super(opcode, arg);
    }

    private X_OF(final short opcode, final Descriptor<?> arg, final int idx){
        super(opcode, arg, new Int32(idx));
    }
}
