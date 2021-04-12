package mds.data;

public enum DTYPE
{
	Z("$Missing"), // 0
	V, // 1 aligned bit string
	BU("Byte_Unsigned", "BU"), // 2
	WU("Word_Unsigned", "WU"), // 3
	LU("Long_Unsigned", "LU"), // 4
	QU("Quadword_Unsigned", "QU"), // 5
	B("Byte", "B"), // 6
	W("Word", "W"), // 7
	L("Long", "L"), // 8
	Q("Quadword", "Q"), // 9
	F("F_Float", "F"), // 10 VMS
	D("D_Float", "V"), // 11 VMS
	FC("F_Complex", "F"), // 12 VMS
	DC("D_Complex", "V"), // 13 VMS
	T("Text"), // 14
	NU, // 15 numeric string, unsigned
	NL, // 16 numeric string, left separate sign
	NLO, // 17 numeric string, left overpunched sign
	NR, // 18 numeric string, right separate sign
	NRO, // 19 numeric string, right overpunched sign
	NZ, // 20 numeric string, zoned sign
	P, // 21 packed decimal string
	ZI, // 22 sequence of instructions
	ZEM, // 23 procedure entry mask
	DSC("Dsc"), // 24
	OU("Octaword_Unsigned", "OU"), // 25
	O("Octaword", "O"), // 26
	G("G_Float", "G"), // 27
	H("H_Float", "H"), // 28
	GC("G_Complex", "G"), // 29 IEEE
	HC("H_Complex", "H"), // 30 128bit
	CIT, // 31 COBOL Intermediate Temporary
	BPV, // 32 bound procedure value
	BLV, // 33 bound label value
	DVU, // 34 unaligned bit string
	ADT, // 35 absolute date and time
	d36, //
	VT, // 37 varying character string; 16-bit count, followed by a string
	d38, d39, //
	d40, d41, d42, d43, d44, d45, d46, d47, d48, d49, //
	d50, //
	POINTER("Pointer"), // 51
	FS("FS_Float", "S"), // 52 IEEE
	FT("FT_Float", "D"), // 53 IEEE
	FSC("FS_Complex", "S"), // 54 IEEE
	FTC("FT_Complex", "D"), // 55 IEEE
	d56, d57, d58, d59, //
	d60, d61, d62, d63, d64, d65, d66, d67, d68, d69, //
	d70, d71, d72, d73, d74, d75, d76, d77, d78, d79, //
	d80, d81, d82, d83, d84, d85, d86, d87, d88, d89, //
	d90, d91, d92, d93, d94, d95, d96, d97, d98, d99, //
	d100, d101, d102, d103, d104, d105, d106, d107, d108, d109, //
	d110, d111, d112, d113, d114, d115, d116, d117, d118, d119, //
	d120, d121, d122, d123, d124, d125, d126, d127, d128, d129, //
	d130, d131, d132, d133, d134, d135, d136, d137, d138, d139, //
	d140, d141, d142, d143, d144, d145, d146, d147, d148, d149, //
	d150, d151, d152, d153, d154, d155, d156, d157, d158, d159, //
	d160, d161, d162, d163, d164, d165, d166, d167, d168, d169, //
	d170, d171, d172, d173, d174, d175, d176, d177, d178, d179, //
	d180, d181, d182, d183, d184, d185, d186, d187, d188, d189, //
	d190, //
	IDENT("Ident"), // 191
	NID("Nid"), // 192
	PATH("Path"), // 193
	PARAM("Param"), // 194
	SIGNAL("Signal"), // 195
	DIMENSION("Dim"), // 196
	WINDOW("Window"), // 197
	SLOPE("Slope"), // 198
	FUNCTION("Function"), // 199
	CONGLOM("Conglom"), // 200
	RANGE("Range"), // 201
	ACTION("Action"), // 202
	DISPATCH("Dispatch"), // 203
	PROGRAM("Program"), // 204
	ROUTINE("Routine"), // 205
	PROCEDURE("Procedure"), // 206
	METHOD("Method"), // 207
	DEPENDENCY("Dependency"), // 208
	CONDITION("Condition"), // 209
	EVENT("Event"), // 210
	WITH_UNITS("With_Units"), // 211
	CALL("Call"), // 212
	WITH_ERROR("With_Error"), // 213
	LIST("List"), // 214
	TUPLE("Tuple"), // 215
	DICTIONARY("Dictionary"), // 216
	OPAQUE("Opaque"), // 217
	d218, d219, //
	d220, d221, d222, d223, d224, d225, d226, d227, d228, d229, //
	d230, d231, d232, d233, d234, d235, d236, d237, d238, d239, //
	d240, d241, d242, d243, d244, d245, d246, d247, d248, d249, //
	d250, d251, d252, d253, d254, d255, //
	;
	public static final DTYPE FLOAT = DTYPE.FS;
	public static final DTYPE DOUBLE = DTYPE.FT;
	public static final DTYPE COMPLEX_FLOAT = DTYPE.FSC;
	public static final DTYPE COMPLEX_DOUBLE = DTYPE.FTC;

	public static DTYPE get(final byte c)
	{
		return DTYPE.values()[c & 0xFF];
	}

	public static final String getName(final byte dtype)
	{
		final String ans = DTYPE.values()[dtype & 0xFF].label;
		if (ans != null)
			return ans;
		return new StringBuffer(9).append("DTYPE_").append(dtype & 0xFF).toString();
	}

	public static final String getSuffix(final byte dtype)
	{
		return DTYPE.values()[dtype & 0xFF].suffix;
	}

	public final String suffix;
	public final String label;

	private DTYPE()
	{
		this.label = null;
		this.suffix = null;
	}

	private DTYPE(final String label)
	{
		this.label = label;
		this.suffix = null;
	}

	private DTYPE(final String label, final String suffix)
	{
		this.label = label;
		this.suffix = suffix;
	}

	public final byte toByte()
	{
		return (byte) this.ordinal();
	}
}
