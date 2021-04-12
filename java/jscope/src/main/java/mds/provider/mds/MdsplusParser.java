package mds.provider.mds;

import java.util.StringTokenizer;

public class MdsplusParser
{
	static String keywords[] =
	{ "ARG_OF", "ARRAY", "ASIN", "ASIND", "AS_IS", "ATAN", "ATAN2", "ATAN2D", "ATAND", "ATANH", "AXIS_OF", "BREAK",
			"BSEARCH", "BTEST", "BUILD_ACTION", "BUILD_CALL", "BUILD_CONDITION", "BUILD_CONGLOM", "BUILD_DEPENDENCY",
			"BUILD_DIM", "BUILD_DISPATCH", "BUILD_EVENT", "BUILD_FUNCTION", "BUILD_METHOD", "BUILD_PROCEDURE",
			"BUILD_PROGRAM", "BUILD_RANGE", "BUILD_ROUTINE", "BUILD_SIGNAL", "BUILD_SLOPE", "BUILD_WINDOW",
			"BUILD_WITH_UNITS", "BUILTIN_OPCODE", "BYTE", "BYTE_UNSIGNED", "CALL", "CASE", "CEILING", "CHAR", "CLASS",
			"CLASS_OF", "CMPLX", "COMMA", "COMPILE", "COMPILE_DEPENDENCY", "COMPLETION_OF", "COMPLETION_MESSAGE_OF",
			"CONCAT", "CONDITIONAL", "CONDITION_OF", "CONJG", "CONTINUE", "COS", "COSD", "COSH", "COUNT", "DATA",
			"DATA_WITH_UNITS", "DATE_TIME", "DBLE", "DEALLOCATE", "DEBUG", "DECOMPILE", "DECOMPILE_DEPENDENCY",
			"DECOMPRESS", "DIGITS", "DIM", "DIM_OF", "DISPATCH_OF", "DIVIDE", "DO", "DOT_PRODUCT", "DPROD", "DSCPTR",
			"DSCPTR_OF", "DTYPE_RANGE", "D_COMPLEX", "D_FLOAT", "ELBOUND", "ELEMENT", "ELSE", "END_OF", "EQUALS_FIRST",
			"EQV", "ERRORLOGS_OF", "ESHAPE", "ESIZE ", "EUBOUND", "EVALUATE", "EXECUTE", "EXP", "EXPONENT", "EXTEND",
			"EXTRACT", "EXT_FUNCTION ", "FINITE", "FIRSTLOC", "FIX_ROPRAND", "FLOAT", "FLOOR", "FOPEN", "FOR",
			"FRACTION", "FSEEK", "FTELL", "FUN", "F_COMPLEX", "F_FLOAT", "GE", "GETDBI", "GETNCI", "GOTO", "GT",
			"G_COMPLEX", "G_FLOAT", "HELP_OF", "HUGE", "H_COMPLEX", "H_FLOAT", "IACHAR", "IAND", "IAND_NOT", "IBCLR",
			"IBSET", "ICHAR", "IDENT_OF", "IEOR", "IEOR_NOT", "IF", "IF_ERROR", "IMAGE_OF", "INAND", "NAND_NOT",
			"INDEX", "INOR", "INOR_NOT", "INOT", "INT", "INTERRUPT_OF ", "INT_UNSIGNED", "IOR", "IOR_NOT", "ISHFT",
			"I_TO_X", "KIND", "KIND_OF", "LABEL", "LANGUAGE_OF", "LASTLOC", "LBOUND", "LE", "LEN", "LEN_TRIM", "LGE",
			"LGT ", "LLE", "LLT", "LOG", "LOG10", "LOG2", "LOGICAL", "LONG", "LONG_UNSIGNED", "LT", "MAKE_ACTION",
			"MAKE_CALL", "MAKE_CONDITION", "MAKE_CONGLOM", "MAKE_DEPENDENCY", "MAKE_DIM", "MAKE_DISPATCH ",
			"MAKE_FUNCTION", "MAKE_METHOD", "MAKE_PARAM", "MAKE_PROCEDURE", "MAKE_PROGRAM", "MAKE_RANGE",
			"MAKE_ROUTINE", "MAKE_SIGNAL", "MAKE_SLOPE", "MAKE_WINDOW", "MAKE_WITH_UNITS", "MAP", "MAX", "MAXEXPONENT",
			"MAXLOC", "MAXVAL", "MEAN", "MIN", "MINEXPONENT", "MINLOC", "MINVAL", "MOD", "MODEL_OF", "MULTIPLY",
			"NAME_OF", "NAND", "NAND_NOT", "NDESC", "NDESC_OF", "NE", "NOT", "OBJECT_OF", "OCTAWORD",
			"OCTAWORD_UNSIGNED", "OPCODE_BUILTIN", "OPCODE_STRING", "OR", "OR_NOT", "PACK", "PERFORMANCE_OF",
			"PHASE_OF", "POST_DEC", "POST_INC", "POWER", "PRECISION", "PRESENT", "PRE_DEC", "PRE_INC", "PRIVATE",
			"PROCEDURE_OF", "PRODUCT", "PROGRAM_OF", "PUBLIC", "QUADWORD", "QUADWORD_UNSIGNED", "QUALIFIERS_OF",
			"RADIX", "RAMP", "RANGE", "RANK", "RAW_OF", "REAL", "REM", "REPEAT", "REPLICATE", "RESET_PRIVATE",
			"RESET_PUBLIC", "RETURN", "ROUTINE_OF", "RRSPACING", "SCALE", "SCAN", "SELECTED_INT_KIND",
			"SELECTED_REAL_KIND", "SET_EXPONENT", "SET_RANGE", "SHAPE", "SHIFT_LEFT", "SHIFT_RIGHT", "SHOW_PRIVATE",
			"SIZE", "SIZEOF", "SLOPE_OF", "SORT", "SORTVAL", "SPACING", "SPAWN", "SPREAD", "SQRT", "SQUARE",
			"STATEMENT", "STRING_OPCODE", "SUBSCRIPT", "SUBTRACT", "SUM", "SWITCH", "TAN", "TAND", "TANH", "TASK_OF",
			"TEXT", "TIME_OUT_OF", "TINY", "TRANSLATE", "TRIM", "UBOUND", "UNION", "UNITS", "UNITS_OF", "UNARY_MINUS",
			"UNARY_PLUS", "UNSIGNED", "UPCASE", "USING", "VALIDATION", "VALIDATION_OF", "VALUE_OF", "VAR", "VECTOR",
			"VERIFY", "WAIT", "WHEN_OF", "WHILE", "WINDOW_OF", "WORD", "WORD_UNSIGNED", "WRITE", "X_TO_I", "ZERO",
			"LONG", "FS_FLOAT" };

	static boolean isKeyword(String str)
	{
		final String upStr = str.toUpperCase();
		for (final String keyword : keywords)
			if (upStr.equals(keyword))
				return true;
		if (str.startsWith("_")) // TDI variable
			return true;
		return false;
	}

	// Parses a TDI expression substituting each occurrence of name (which is not a
	// keyword)
	// with <funName>(<name>)
	public static String parseFun(String expr, String rightPart, String leftPart)
	{
		final StringTokenizer st = new StringTokenizer(expr + " ", " ()[]{}*+/-!|&=?^#,.;:><", true);
		String outExpr = "";
		while (st.hasMoreTokens())
		{
			final String currToken = st.nextToken();
			if (isKeyword(currToken))
				outExpr += currToken;
			else
			{
				final char firstChar = currToken.charAt(0);
				if ((firstChar >= 'a' && firstChar <= 'z') || (firstChar >= 'A' && firstChar <= 'Z'))
					outExpr += rightPart + currToken + leftPart;
				else
					outExpr += currToken;
			}
		}
		// System.out.println(outExpr);
		return outExpr;
	}
}
