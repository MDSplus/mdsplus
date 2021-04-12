package mds.data.descriptor_r.function;

import java.nio.ByteBuffer;
import mds.data.OPC;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Function;
import mds.data.descriptor_s.Int32;

public class X_OF extends Function
{
	public static final class DimOf extends X_OF
	{
		public DimOf(final ByteBuffer b)
		{
			super(b);
		}

		public DimOf(final Descriptor<?> arg)
		{
			super(OPC.OpcDimOf, arg);
		}

		public DimOf(final Descriptor<?> arg, final int idx)
		{
			super(OPC.OpcDimOf, arg, idx);
		}
	}

	public static final class HelpOf extends X_OF
	{
		public HelpOf(final ByteBuffer b)
		{
			super(b);
		}

		public HelpOf(final Descriptor<?> arg)
		{
			super(OPC.OpcHelpOf, arg);
		}
	}

	public static final class UnitsOf extends X_OF
	{
		public UnitsOf(final ByteBuffer b)
		{
			super(b);
		}

		public UnitsOf(final Descriptor<?> arg)
		{
			super(OPC.OpcUnitsOf, arg);
		}
	}

	public static final boolean coversOpCode(final OPC opcode)
	{
		switch (opcode)
		{
		case OpcArgOf:
		case OpcAxisOf:
		case OpcBeginOf:
		case OpcClassOf:
		case OpcCompletionMessageOf:
		case OpcCompletionOf:
		case OpcConditionOf:
		case OpcDimOf:
		case OpcDispatchOf:
		case OpcDscptrOf:
		case OpcEndOf:
		case OpcErrorlogsOf:
		case OpcErrorOf:
		case OpcHelpOf:
		case OpcIdentOf:
		case OpcImageOf:
		case OpcInterruptOf:
		case OpcKindOf:
		case OpcLanguageOf:
		case OpcMethodOf:
		case OpcModelOf:
		case OpcNameOf:
		case OpcNdescOf:
		case OpcObjectOf:
		case OpcPerformanceOf:
		case OpcPhaseOf:
		case OpcProcedureOf:
		case OpcProgramOf:
		case OpcQualifiersOf:
		case OpcRawOf:
		case OpcRoutineOf:
		case OpcSizeOf:
		case OpcSlopeOf:
		case OpcTaskOf:
		case OpcTimeoutOf:
		case OpcUnitsOf:
		case OpcValidationOf:
		case OpcValueOf:
		case OpcWhenOf:
		case OpcWindowOf:
			return true;
		default:
			return false;
		}
	}

	public static final X_OF deserialize(final ByteBuffer b)
	{
		final OPC opcode = OPC.get(b.getShort(b.getInt(Descriptor._ptrI)));
		switch (opcode)
		{
		case OpcDimOf:
			return new DimOf(b);
		case OpcHelpOf:
			return new HelpOf(b);
		case OpcUnitsOf:
			return new UnitsOf(b);
		default:
			return new X_OF(b);
		}
	}

	protected X_OF(final ByteBuffer b)
	{
		super(b);
	}

	private X_OF(final OPC opcode, final Descriptor<?> arg)
	{
		super(opcode, arg);
	}

	private X_OF(final OPC opcode, final Descriptor<?> arg, final int idx)
	{
		super(opcode, arg, new Int32(idx));
	}
}
