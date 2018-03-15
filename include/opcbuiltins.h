COM/*	<opcbuiltins.h>
	.macro	OpcBUILTINS				;*/
COM/*	<WARNING: This code is used by both MACRO and C.>*/
COM/*	<Check TDISHR$SHARE.MAR and TDI$$DEF_FUNCTION.C	>*/
COM/*	<Included by opcopcodes.h			>*/
COM/*	<For the OPC macro to avoid argument mismatch:	>*/
COM/*	<MACRO needs white space between it and (".	>*/
COM/*	<CC requires a / *, MACRO requires a semicolon.	>*/

COM/*	<Tokens are used by LEX and DECOMPILE_R.	>*/
COM/*	<+I=immediate (for all VMS arguments, evaluate. Machine-indep types, _OF.)	>*/
COM/*	<	Especially VECTOR and SET_RANGE to make arrays and BUILD_ for class-R.	>*/
COM/*	<+N=named (is operator or keyword in the language.)				>*/
COM/*	<+S=symbolic (decompiles to symbolic form.)					>*/
COM/*	<+U=unusual (defer evaluation: system calls and array generators.)		>*/
COM/*	<CC omissions: % (casts) declarations initializers auto const static struct * &	>*/
COM/*	<F90 omissions:	!-comments defined-operators defined-types common declarations	>*/
COM/*	<types%components identify much-I/O modules entry contains named-arguments	>*/
COM/*	<F90==CC: select case==switch do==for dowhile==while exit==break cycle==continue>*/
COM/*	<CC==TDI: case x:==case(x)			default:==case default		>*/
COM/*	<CC==TDI: do s while(x)==do {s}while(x)		return==return()		>*/
COM/*	<IDL omissions: a#b TOTAL							>*/
COM/*	<name,		f1,	f2,	f3,		i1,i2,	o1,o2,	m1,m2,	token	Language, args>*/

OPC (	dollar,	$ ,		Else,	undef,	undef,		XX,YY,	XX,YY,	0,0,	ARG	)/*;	argument for put*/
OPC (	A0,	$A0 ,		Constant, undef,	A0,		F,HC,	F,HC,	0,0,	CONST	)/*;	Bohr radius	*/
OPC (	Alpha,	$ALPHA ,		Constant, undef,	Alpha,		F,HC,	F,HC,	0,0,	CONST	)/*;	1/137		*/
OPC (	Amu,	$AMU ,		Constant, undef,	Amu,		F,HC,	F,HC,	0,0,	CONST	)/*;	atomic mass unit*/
OPC (	C,	$C ,		Constant, undef,	C,		F,HC,	F,HC,	0,0,	CONST	)/*;	light speed	*/
OPC (	Cal,	$CAL ,		Constant, undef,	Cal,		F,HC,	F,HC,	0,0,	CONST	)/*;	calorie to J	*/
OPC (	Degree,	$DEGREE ,	Constant, undef,	Degree,	F,HC,	F,HC,	0,0,	CONST	)/*;	degree to rad	*/
OPC (	Ev,	$EV ,		Constant, undef,	Ev,		F,HC,	F,HC,	0,0,	CONST	)/*;	electron volt	*/
OPC (	False,	$FALSE ,		Constant, undef,	False,		VV,VV,	VV,VV,	0,0,	CONST	)/*;	0bu		*/
OPC (	Faraday,	$FARADAY ,	Constant, undef,	Faraday,	F,HC,	F,HC,	0,0,	CONST	)/*;	Faraday constant*/
OPC (	G,	$G ,		Constant, undef,	G,		F,HC,	F,HC,	0,0,	CONST	)/*;	gravity		*/
OPC (	Gas,	$GAS ,		Constant, undef,	Gas,		F,HC,	F,HC,	0,0,	CONST	)/*;	gas		*/
OPC (	H,	$H ,		Constant, undef,	H,		F,HC,	F,HC,	0,0,	CONST	)/*;	Planck		*/
OPC (	Hbar,	$HBAR ,		Constant, undef,	Hbar,		F,HC,	F,HC,	0,0,	CONST	)/*;			*/
OPC (	I,	$I ,		Constant, undef,	I,		F,HC,	FC,HC,	0,0,	CONST	)/*;	imaginary	*/
OPC (	K,	$K ,		Constant, undef,	K,		F,HC,	F,HC,	0,0,	CONST	)/*;	Boltzmann	*/
OPC (	Me,	$ME ,		Constant, undef,	Me,		F,HC,	F,HC,	0,0,	CONST	)/*;	mass electron	*/
OPC (	Missing,	$MISSING ,	Constant, undef,	Missing,	XX,YY,	XX,YY,	0,0,	CONST+I	)/*;	missing arg	*/
OPC (	Mp,	$MP ,		Constant, undef,	Mp,		F,HC,	F,HC,	0,0,	CONST	)/*;	mass proton	*/
OPC (	N0,	$N0 ,		Constant, undef,	N0,		F,HC,	F,HC,	0,0,	CONST	)/*;	Loschmidt's numb*/
OPC (	Na,	$NA ,		Constant, undef,	Na,		F,HC,	F,HC,	0,0,	CONST	)/*;	Avogadro number	*/
OPC (	P0,	$P0 ,		Constant, undef,	P0,		F,HC,	F,HC,	0,0,	CONST	)/*;	atmospheric pres*/
OPC (	Pi,	$PI ,		Constant, undef,	Pi,		F,HC,	F,HC,	0,0,	CONST	)/*;			*/
OPC (	Qe,	$QE ,		Constant, undef,	Qe,		F,HC,	F,HC,	0,0,	CONST	)/*;	charge electron	*/
OPC (	Re,	$RE ,		Constant, undef,	Re,		F,HC,	F,HC,	0,0,	CONST	)/*;	classical el.rad*/
OPC (	Roprand,	$ROPRAND ,	Constant, undef,	Roprand,	F,HC,	F,HC,	0,0,	CONST+I	)/*;vax	bad floating	*/
OPC (	Rydberg,	$RYDBERG ,	Constant, undef,	Rydberg,	F,HC,	F,HC,	0,0,	CONST	)/*;	Rydberg constant*/
OPC (	T0,	$T0 ,		Constant, undef,	T0,		F,HC,	F,HC,	0,0,	CONST	)/*;	standard temp.	*/
OPC (	Torr,	$TORR ,		Constant, undef,	Torr,		F,HC,	F,HC,	0,0,	CONST	)/*;	1mm Hg pressure	*/
OPC (	True,	$TRUE ,		Constant, undef,	True,		VV,VV,	VV,VV,	0,0,	CONST	)/*;	1bu		*/
OPC (	Value,	$VALUE ,		Value,	undef,	        undef,		XX,YY,	XX,YY,	0,0,	CONST	)/*;	raw value in sig*/
OPC (	Abort,	ABORT ,		Abort,	undef,		undef,		XX,YY,	XX,YY,	0,255,	OK+U	)/*;	() 		*/
OPC (	Abs,	ABS ,		Same,	Abs,		Abs,		BU,HC,	BU,H,	1,1,	OK	)/*;f9	(a)		*/
OPC (	Abs1,	ABS1 ,		Same,	Abs,		Abs1,		BU,HC,	BU,H,	1,1,	OK	)/*;	(a) 1-norm	*/
OPC (	AbsSq,	ABSSQ ,		Same,	Square,		AbsSq,		BU,HC,	BU,H,	1,1,	OK	)/*;	(a) abs(a)**2	*/
OPC (	Achar,	ACHAR ,		Same,	Char,		Char,		BU,O,	T,T,	1,2,	OK	)/*;f9	(i,[dummy_kind])*/
OPC (	Acos,	ACOS ,		Same,	None,		Acos,		F,HC,	F,HC,	1,1,	OK	)/*;f9	(x)		*/
OPC (	Acosd,	ACOSD ,		Same,	None,		Acosd,		F,HC,	F,HC,	1,1,	OK	)/*;vax	(x)		*/
OPC (	Add,	ADD ,		Same,	Add,		Add,		BU,HC,	BU,HC,	2,2,	ADD+S	)/*;%	a+b		*/
OPC (	Adjustl,	ADJUSTL ,	Same,	Adjust,		Adjustl,	BU,T,	T,T,	1,1,	OK	)/*;f9	(string)	*/
OPC (	Adjustr,	ADJUSTR ,	Same,	Adjust,		Adjustr,	BU,T,	T,T,	1,1,	OK	)/*;f9	(string)	*/
OPC (	Aimag,	AIMAG ,		Same,	Keep,		Aimag,		BU,HC,	BU,H,	1,1,	OK	)/*;f9	(z)		*/
OPC (	Aint,	AINT ,		Same,	Aint,		Aint,		F,HC,	F,HC,	1,2,	OK	)/*;f9	(a,[kind])	*/
OPC (	All,	ALL ,		Trans,	Mask1,		All,		BU,O,	VV,VV,	1,2,	OK	)/*;f9	(mask,[dim])	*/
OPC (	Allocated,	ALLOCATED ,	Allocated, undef,    	undef,		XX,YY,	VV,VV,	1,1,	OK+U	)/*;f9	(array)		*/
OPC (	And,	AND ,		Same,	Land,		And,		BU,O,	VV,VV,	2,2,	LAND+N+S )/*;%	v&&w	v.AND.w	*/
OPC (	AndNot,	AND_NOT ,	Same,	Land,		AndNot,	BU,O,	VV,VV,	2,2,	LAND+N	)/*;%	(v,w)	v.AND..NOT.w	*/
OPC (	Anint,	ANINT ,		Same,	Aint,		Anint,		F,HC,	F,HC,	1,2,	OK	)/*;f9	(a,kind)	*/
OPC (	Any,	ANY ,		Trans,	Mask1,		Any,		BU,O,	VV,VV,	1,2,	OK	)/*;f9	(mask,[dim])	*/
OPC (	Arg,	ARG ,		Same,	None,		Arg,		FC,HC,	F,H,	1,1,	OK	)/*;	(z) atan2(imag,real)	*/
OPC (	Argd,	ARGD ,		Same,	None,		Argd,		FC,HC,	F,H,	1,1,	OK	)/*;	(z) atan2d(imag,real)	*/
OPC (	ArgOf,	ARG_OF ,		ArgOf,	undef,		undef, FUNCTION,CONDITION,XX,YY,1,2,	OK+I	)/*;mds	(ext_fun other ,[n])	*/
OPC (	Array,	ARRAY ,		Array,	undef,		Zero,		XX,YY,	FLOAT,F,	0,2,	OK+U	)/*;	([size],[kind])		*/
OPC (	Asin,	ASIN ,		Same,	None,		Asin,		F,HC,	F,HC,	1,1,	OK	)/*;f9	(x)			*/
OPC (	Asind,	ASIND ,		Same,	None,		Asind,		F,HC,	F,HC,	1,1,	OK	)/*;vax	(x)			*/
OPC (	AsIs,	AS_IS ,		AsIs,	undef,		undef,		XX,YY,	XX,YY,	1,1,	MODIF+N	)/*;mds	(any) unevaluated	*/
OPC (	Atan,	ATAN ,		Same,	None,		Atan,		F,HC,	F,HC,	1,1,	OK	)/*;f9	(x)			*/
OPC (	Atan2,	ATAN2 ,		Same,	Atan2,		Atan2,		F,HC,	F,HC,	2,2,	OK	)/*;f9	(y,x)			*/
OPC (	Atan2d,	ATAN2D ,		Same,	Atan2,		Atan2d,		F,HC,	F,HC,	2,2,	OK	)/*;vax	(y,x)			*/
OPC (	Atand,	ATAND ,		Same,	None,		Atand,		F,HC,	F,HC,	1,1,	OK	)/*;vax	(x)			*/
OPC (	Atanh,	ATANH ,		Same,	None,		Atanh,		F,HC,	F,HC,	1,1,	OK	)/*;vax	(x)			*/
OPC (	AxisOf,	AXIS_OF ,	AxisOf, undef,		undef, DIMENSION,DIMENSION,SLOPE,SLOPE,1,1,	OK+I	)/*;mds	(dimension)	*/
OPC (	Backspace,	BACKSPACE ,	Backspace, undef,	undef,		L,L,	L,L,	1,1,	OK+U	)/*;f9	(unit)			*/
OPC (	Ibclr,	IBCLR ,		Same,	Long2,		Ibclr,		XX,YY,	XX,YY,	2,2,	OK	)/*;f9	(i,pos)			*/
OPC (	BeginOf,	BEGIN_OF ,	BeginOf, undef,		undef, WINDOW,RANGE,		XX,YY,	1,2,	OK+I	)/*;mds	(window or range)*/
OPC (	Ibits,	IBITS ,		Same,	Long2,		Ibits,		BU,L,	BU,L,	3,3,	OK	)/*;f9	(i,pos,len)		*/
OPC (	Break,	BREAK ,		Break,	undef,		undef,		XX,YY,	XX,YY,	0,0,	BREAK+N+U )/*;%cc BREAK;for,switch,while*/
OPC (	Bsearch,	BSEARCH ,	Bsearch, undef,		undef,		T,HC,	T,HC,	2,4,	OK	)/*;	(a,table,[mode],[upcas])*/
OPC (	Ibset,	IBSET ,		Same,	Long2,		Ibset,		XX,YY,	XX,YY,	2,2,	OK	)/*;f9	(i,pos)			*/
OPC (	Btest,	BTEST ,		Same,	Btest,		Btest,		XX,YY,	VV,VV,	2,2,	OK	)/*;f9	(i,pos)			*/
OPC (	BuildAction,	BUILD_ACTION ,	Build,	undef,		undef, XX,YY,ACTION,ACTION,	2,5,	OK+I	)/*;mds	(dis,task,err,comp,perf)*/
OPC (	BuildCondition,	BUILD_CONDITION ,Build,	undef,		undef, XX,YY,CONDITION,WU,	2,2,	OK+I	)/*;mds	(opcode,condition)	*/
OPC (	BuildConglom,	BUILD_CONGLOM ,	Build,	undef,		undef, XX,YY,CONGLOM,CONGLOM,	4,4,	OK+I	)/*;mds	(image,model,name,qual)	*/
OPC (	BuildDependency,	BUILD_DEPENDENCY ,Build, undef,		undef, XX,YY,DEPENDENCY,WU,	3,3,	OK+I	)/*;mds	(opcode,arg1,arg2)	*/
OPC (	BuildDim,	BUILD_DIM ,	Build,	undef,		undef, XX,YY,DIMENSION,DIMENSION,2,2,	OK+I	)/*;mds	(window,axis)		*/
OPC (	BuildDispatch,	BUILD_DISPATCH ,	Build,	undef,		undef, XX,YY,DISPATCH,BU,	5,5,	OK+I	)/*;mds	(type,id,phase,when,com)*/
OPC (	BuildEvent,	BUILD_EVENT ,	BuildPath, undef,	undef, T,T,EVENT,EVENT,		1,1,	OK+I	)/*;mds	(string)		*/
OPC (	BuildFunction,	BUILD_FUNCTION ,	Build,	undef,		undef, XX,YY,FUNCTION,WU,	1,254,	OK+I	)/*;mds	(opcode,[arguments...])	*/
OPC (	BuildMethod,	BUILD_METHOD ,	Build,	undef,		undef, XX,YY,METHOD,METHOD,	3,254,	OK+I	)/*;mds	(timeout,method,obj,...)*/
OPC (	BuildParam,	BUILD_PARAM ,	Build,	undef,		undef, XX,YY,PARAM,PARAM,	3,3,	OK+I	)/*;mds	(value,help,validation)	*/
OPC (	BuildPath,	BUILD_PATH ,	BuildPath, undef,	undef, T,T,PATH,PATH,		1,1,	OK+I	)/*;mds	(string)		*/
OPC (	BuildProcedure,	BUILD_PROCEDURE ,Build,	undef,		undef, XX,YY,PROCEDURE,PROCEDURE,3,254,	OK+I	)/*;mds	(timeout,lang,proc,...)	*/
OPC (	BuildProgram,	BUILD_PROGRAM ,	Build,	undef,		undef, XX,YY,PROGRAM,PROGRAM,	2,2,	OK+I	)/*;mds	(timeout,program)	*/
OPC (	BuildRange,	BUILD_RANGE ,	Build,	undef,		undef, XX,YY,RANGE,RANGE,	2,3,	OK+I	)/*;mds	([low],[high],[step])	*/
OPC (	BuildRoutine,	BUILD_ROUTINE ,	Build,	undef,		undef, XX,YY,ROUTINE,ROUTINE,	3,254,	OK+I	)/*;mds	(timeout,image,rout,..)	*/
OPC (	BuildSignal,	BUILD_SIGNAL ,	Build,	undef,		undef, XX,YY,SIGNAL,SIGNAL,	2,2+MAXDIM,OK+I	)/*;mds	(data,raw,[dim...])	*/
OPC (	BuildSlope,	BUILD_SLOPE ,	Build,	undef,		undef, XX,YY,SLOPE,SLOPE,	1,254,	OK+I	)/*;mds	(slope,[begin,end]...)	*/
OPC (	BuildWindow,	BUILD_WINDOW ,	Build,	undef,		undef, XX,YY,WINDOW,WINDOW,	3,3,	OK+I	)/*;mds	(istart,iend,x_at_0)	*/
OPC (	BuildWithUnits,	BUILD_WITH_UNITS ,Build, undef,		undef, XX,YY,WITH_UNITS,WITH_UNITS,2,2,	OK+I	)/*;mds	(data,units)		*/
OPC (	BuiltinOpcode,	BUILTIN_OPCODE ,	Same,	None,		StringOpcode,	T,T,	W,W,	1,1,	OK	)/*;mds	(string)		*/
OPC (	Byte,	BYTE ,		Same,	Keep,		undef,		B,B,	B,B,	1,1,	CAST+N+I )/*;%cc (a) (signed char)	*/
OPC (	ByteUnsigned,	BYTE_UNSIGNED ,	Same,	Keep,		undef,		BU,BU,	BU,BU,	1,1,	CAST+N+I )/*;%cc (a) (unsigned char)	*/
OPC (	Case,	CASE ,		Case,	undef,		undef,		XX,YY,	XX,YY,	2,254,	CASE+N+U )/*;%cc CASE(n)stmt switch_opt	*/
OPC (	Ceiling,	CEILING ,	Same,	Aint,		Ceiling,	BU,HC,	BU,HC,	1,1,	OK	)/*;f9	(a)			*/
OPC (	Char,	CHAR ,		Same,	Char,		Char,		BU,O,	T,T,	1,2,	OK	)/*;f9	(i,[kind])		*/
OPC (	Class,	CLASS ,		Class,	undef,		undef,		XX,YY,	BU,BU,	1,1,	OK	)/*;vms	(any)			*/
OPC (	Fclose,	FCLOSE ,		Fclose,	undef,		undef,		L,L,	L,L,	1,1,	OK+U	)/*;cc	(unit)			*/
OPC (	Cmplx,	CMPLX ,		Same,	Cmplx,		Cmplx,		FC,HC,	FC,HC,	1,3,	CAST+N+I )/*;f9 (x,[y],[kind])		*/
OPC (	Comma,	COMMA ,		Comma,	undef,		undef,		XX,YY,	XX,YY,	2,254,	OK+S	)/*;%cc	a,b.. eval&keep last	*/
OPC (	Compile,	COMPILE ,	Compile, undef,		undef, T,T,	FUNCTION,FUNCTION,1,254,OK	)/*;mds	(string,[arguments...])	*/
OPC (	CompletionOf,	COMPLETION_OF ,	CompletionOf, undef,	undef, ACTION,DISPATCH,	XX,YY,	1,1,	OK+I	)/*;mds	(dispatch)		*/
OPC (	Concat,	CONCAT ,		MinMax,Concat,		Concat,		T,T,	T,T,	2,254,	CONCAT+I )/*;%f9 a//b...		*/
OPC (	Conditional,	CONDITIONAL ,	Conditional,Merge,	Merge,	XX,YY,	XX,YY,	3,3,	COND+S	)/*;%cc	(t,f,c) c ? t : f	*/
OPC (	Conjg,	CONJG ,		Same,	Keep,		Conjg,		BU,HC,	BU,HC,	1,1,	OK	)/*;f9	(z)			*/
OPC (	Continue,	CONTINUE ,	Continue, undef,		undef,		XX,YY,	XX,YY,	0,0,	BREAK+N+U )/*;%cc CONTINUE; for,while	*/
OPC (	Convolve,	CONVOLVE ,	Convolve, undef,		undef,		BU,HC,	BU,HC,	2,2,	OK	)/*;	(z,z)			*/
OPC (	Cos,	COS ,		Same,	NoHc,		Cos,		F,HC,	F,HC,	1,1,	OK	)/*;f9	(z)			*/
OPC (	Cosd,	COSD ,		Same,	None,		Cosd,		F,HC,	F,HC,	1,1,	OK	)/*;vax	(x)			*/
OPC (	Cosh,	COSH ,		Same,	None,		Cosh,		F,HC,	F,HC,	1,1,	OK	)/*;f9	(x)			*/
OPC (	Count,	COUNT ,		Trans,	Mask1,		Count,		BU,O,	L,L,	1,2,	OK	)/*;f9	(mask,[dim])		*/
OPC (	Cshift,	CSHIFT ,		Shift,	undef,		Cshift,		XX,YY,	XX,YY,	2,3,	OK	)/*;f9	(array,shift,dim)	*/
OPC (	Cvt,	CVT ,		Same,	Cvt,		undef,		XX,YY,	XX,YY,	2,2,	OK+I	)/*;	(a,mold)		*/
OPC (	Data,	DATA ,		Data,	undef,		undef,		XX,YY,	XX,YY,	1,1,	OK+I	)/*;mds	(any)			*/
OPC (	DateAndTime,	DATE_AND_TIME ,	DateAndTime, undef,	undef,		T,T,	T,L,	0,1,	OK+U	)/*;f9variant ([string])	*/
OPC (	DateTime,	DATE_TIME ,	DateTime, undef,	undef,		XX,YY,	T,T,	0,1,	OK+U	)/*;	([quad])		*/
OPC (	Dble,	DBLE ,		Same,	Dble,		undef,		BU,HC,	WU,HC,	1,1,	OK	)/*;f9variant (a)		*/
OPC (	Deallocate,	DEALLOCATE ,	Deallocate, undef,	undef,		XX,YY,	L,L,	0,254,	OK+U	)/*;	(a...)			*/
OPC (	Debug,	DEBUG ,		Debug,	undef,		undef,		L,L,	L,L,	0,1,	OK+U	)/*;	(option)		*/
OPC (	Decode,	DECODE ,		Decode,	undef,		undef,		T,T,	XX,YY,	1,2,	OK	)/*;f?	(fmt|*,text)		*/
OPC (	Decompile,	DECOMPILE ,	Decompile, undef,	undef,		XX,YY,	T,T,	1,2,	OK	)/*;mds	(expression,[max_array])*/
OPC (	Decompress,	DECOMPRESS ,	Decompress, undef,	undef,		XX,YY,	XX,YY,	4,4,	OK	)/*;mds	([ima],[rou],shape,dat)	*/
OPC (	Default,	DEFAULT ,	Default, undef,		undef,		XX,YY,	XX,YY,	1,254,	DEFAULT+N+U )/*;%cc CASE DEFAULT stmt	*/
OPC (	Derivative,	DERIVATIVE ,	Trans,	Sign,		Derivative,	XX,YY,	XX,YY,	2,3,	OK	)/*;	(z,dim,[dt]) axis diff	*/
OPC (	Descr,	DESCR ,		Else,	undef,		undef,		XX,YY,	XX,YY,	1,1,	OK	)/*;vax	(any)			*/
OPC (	Diagonal,	DIAGONAL ,	Diagonal, undef,		undef,		XX,YY,	XX,YY,	1,2,	OK	)/*;	(vector,[fill])		*/
OPC (	Digits,	DIGITS ,		Scalar,Any,		Digits,		BU,HC,	L,L,	1,1,	OK	)/*;f9	(model)			*/
OPC (	Dim,	DIM ,		Same,	Add,		Dim,		BU,HC,	BU,HC,	2,2,	OK	)/*;f9	(x,y)			*/
OPC (	DimOf,	DIM_OF ,		DimOf,	undef,		undef, XX,YY,DIMENSION,DIMENSION,1,2,	OK	)/*;mds	(signal or VMS,[num])	*/
OPC (	DispatchOf,	DISPATCH_OF ,	DispatchOf, undef,	undef, ACTION,DISPATCH,DISPATCH,DISPATCH,1,1,OK+I)/*;mds (action)		*/
OPC (	Divide,	DIVIDE ,		Same,	Divide,		Divide,		BU,HC,	BU,HC,	2,2,	MUL+S	)/*;%	a/b			*/
OPC (	Lbound,	LBOUND ,		Bound,	undef,		Lbound,		XX,YY,	L,L,	1,2,	OK	)/*;f9	(array,[dim])		*/
OPC (	Do,	DO ,		Do,	undef,		undef,		XX,YY,	XX,YY,	2,254,	DO+N	)/*;%cc	DO stmt WHILE(expr)	*/
OPC (	DotProduct,	DOT_PRODUCT ,	Scalar,Multiply,	DotProduct,	BU,HC,	BU,HC,	2,2,	OK	)/*;f9variant (vec_a,vec_b)	*/
OPC (	Dprod,	DPROD ,		Same,	Dprod,		Multiply,	BU,HC,	WU,HC,	2,2,	OK	)/*;f9variant (x,y)		*/
OPC (	Dscptr,	DSCPTR ,		Dscptr,	undef,		undef,		XX,YY,	XX,YY,	1,2,	OK+I	)/*;mds	(any_class_R,[num])	*/
OPC (	Shape,	SHAPE ,		Bound,	undef,		Shape,		XX,YY,	L,L,	1,2,	OK	)/*;f9	(array,[dim])		*/
OPC (	Size,	SIZE ,		Bound,	undef,		Size,		XX,YY,	L,L,	1,2,	OK	)/*;f9	(array,[dim])		*/
OPC (	Kind,	KIND ,		Kind,	undef,		undef,		XX,YY,	BU,BU,	1,1,	OK	)/*;f9	(any)			*/
OPC (	Ubound,	UBOUND ,		Bound,	undef,		Ubound,		XX,YY,	L,L,	1,2,	OK	)/*;f9	(array,[dim])		*/
OPC (	DComplex,	D_COMPLEX ,	Same,	Cmplx,		Complex,	D,D,	DC,DC,	1,2,	CAST+N+I )/*;%	(x,y)			*/
OPC (	DFloat,	D_FLOAT ,	Same,	Keep,		undef,		D,D,	D,D,	1,1,	CAST+N+I )/*;%	(a)			*/
OPC (	Range,	RANGE ,		Scalar,Any,		Range,		F,HC,	L,L,	1,1,	OK	)/*;f9	(model)			*/
OPC (	Precision,	PRECISION ,	Scalar,Any,		Precision,	F,HC,	L,L,	1,1,	OK	)/*;f9	(model)			*/
OPC (	Elbound,	ELBOUND ,	Ebound, undef,		Elbound,	XX,YY,	BU,H,	1,2,	OK	)/*;	(array,[dim])		*/
OPC (	Else,	ELSE ,		Else,	undef,		undef,		XX,YY,	XX,YY,	0,0,	ELSE+N+I )/*;%cc placeholder IF		*/
OPC (	Elsewhere,	ELSEWHERE ,	Else,	undef,		undef,		XX,YY,	XX,YY,	0,0,	ELSEW+N+I )/*;%f9 placeholder WHERE	*/
OPC (	Encode,	ENCODE ,		Encode,	undef,		undef,		XX,YY,	T,T,	1,254,	OK	)/*;%	(fmt|*,arg...)		*/
OPC (	Endfile,	ENDFILE ,	Endfile, undef,		undef,		L,L,	L,L,	1,1,	OK+U	)/*;f9	(unit)			*/
OPC (	EndOf,	END_OF ,		EndOf,	undef,		undef, WINDOW,RANGE,	XX,YY,	1,2,	OK+I	)/*;mds	(window slope,[n] range)*/
OPC (	Eoshift,	EOSHIFT ,	Shift,	undef,		Eoshift,	XX,YY,	XX,YY,	3,4,	OK	)/*;f9	(arr,dim,shift,[bndry])	*/
OPC (	Epsilon,	EPSILON ,	Scalar, Keep,		Epsilon,	F,HC,	F,HC,	1,1,	OK	)/*;f9	(model)			*/
OPC (	Eq,	EQ ,		Same,	Eq,		Eq,		XX,YY,	VV,VV,	2,2,	LEQ+N+S	)/*;%	a==b	a.EQ.b		*/
OPC (	Equals,	EQUALS ,		Equals,	undef,		undef,		XX,YY,	XX,YY,	2,2,	OK+S+U	)/*;%	a = b			*/
OPC (	EqualsFirst,	EQUALS_FIRST ,	EqualsFirst, undef,	undef,		XX,YY,	XX,YY,	1,1,	OK+S+U	)/*;%	a binop = b		*/
OPC (	Eqv,	EQV ,		Same,	Land,		Eqv,		BU,O,	VV,VV,	2,2,	LEQV+N	)/*;%	(v,w)	v.EQV.w		*/
OPC (	Eshape,	ESHAPE ,		Ebound, undef,		Eshape,		XX,YY,	BU,H,	1,2,	OK	)/*;	(array,[dim])		*/
OPC (	Esize,	ESIZE ,		Ebound, undef,		Esize,		XX,YY,	BU,H,	1,2,	OK	)/*;	(array,[dim])		*/
OPC (	Eubound,	EUBOUND ,	Ebound, undef,		Eubound,	XX,YY,	BU,H,	1,2,	OK	)/*;	(array,[dim])		*/
OPC (	Evaluate,	EVALUATE ,	Evaluate, undef,	undef,		XX,YY,	XX,YY,	1,1,	OK	)/*;	(reeval_expression)	*/
OPC (	Execute,	EXECUTE ,	Execute, undef,		undef,		T,T,	XX,YY,	1,254,	OK	)/*;	(string,[arguments...])	*/
OPC (	Exp,	EXP ,		Same,	NoHc,		Exp,		F,HC,	F,HC,	1,1,	OK	)/*;f9	(z)			*/
OPC (	Exponent,	EXPONENT ,	Same,	Keep,		Exponent,	F,HC,	L,L,	1,1,	OK	)/*;f9	(x)			*/
OPC (	ExtFunction,	EXT_FUNCTION ,	ExtFunction, undef,	undef,		XX,YY,	XX,YY,	2,254,	OK+S+U	)/*;mds (image,routine,a...)	*/
OPC (	Fft,	FFT ,		Fft,	undef,		undef,		FC,HC,	FC,HC,	2,2,	OK	)/*;	(array,direction)	*/
OPC (	FirstLoc,	FIRSTLOC ,	Trans,	Mask1,		FirstLoc,	BU,O,	VV,VV,	1,2,	OK	)/*;	(mask,[dim])		*/
OPC (	Fit,	FIT ,		Fit,	undef,		undef,		F,HC,	F,HC,	2,2,	OK	)/*;	(X_guess,residuals(X))	*/
OPC (	FixRoprand,	FIX_ROPRAND ,	Same,	Fix,		FixRoprand,	BU,HC,	BU,HC,	2,2,	OK	)/*;	(a,replacement)		*/
OPC (	Float,	FLOAT ,		Same,	Real,		undef,		F,H,	F,H,	1,2,	CAST+N+I )/*;	(a,[kind])		*/
OPC (	Floor,	FLOOR ,		Same,	Aint,		Floor,		F,HC,	F,HC,	1,1,	OK	)/*;f9	(x)			*/
OPC (	For,	FOR ,		For,	undef,		undef,		XX,YY,	XX,YY,	4,254,	FOR+N	)/*;%cc	FOR(expr;expr;expr)stmt	*/
OPC (	Fraction,	FRACTION ,	Same,	Keep,		Fraction,	F,HC,	F,HC,	1,1,	OK	)/*;f9	(x)			*/
OPC (	Fun,	FUN ,		Fun,	undef,		undef,		XX,YY,	YY,YY,	2,254,	FUN+N+U	)/*;[public] fun name(a...)stmt	*/
OPC (	FComplex,	F_COMPLEX ,	Same,	Cmplx,		Complex,	F,F,	FC,FC,	1,2,	CAST+N+I )/*;%	(x,y)			*/
OPC (	FFloat,	F_FLOAT ,	Same,	Keep,		undef,		F,F,	F,F,	1,1,	CAST+N+I )/*;%	(a)			*/
OPC (	Ge,	GE ,		Same,	Eq,		Ge,		XX,YY,	VV,VV,	2,2,	LGE+N+S	)/*;%	a>=b	a.GE.b		*/
OPC (	GetNci,	GETNCI ,		GetNci,	undef,		undef,		NID,NID,XX,YY,	2,3,	OK+U	)/*;mds	(nid,item_str,[usage])	*/
OPC (	Goto,	GOTO ,		Goto,	undef,		undef,		XX,YY,	XX,YY,	1,1,	GOTO+N+U )/*;%cc GOTO label;		*/
OPC (	Gt,	GT ,		Same,	Eq,		Gt,		XX,YY,	VV,VV,	2,2,	LGE+N+S	)/*;%	a>b	a.GT.b		*/
OPC (	GComplex,	G_COMPLEX ,	Same,	Cmplx,		Complex,	G,G,	GC,GC,	1,2,	CAST+N+I )/*;%	(x,y)			*/
OPC (	GFloat,	G_FLOAT ,	Same,	Keep,		undef,		G,G,	G,G,	1,1,	CAST+N+I )/*;%	(a)			*/
OPC (	HelpOf,	HELP_OF ,	HelpOf, undef,		undef,	PARAM,PARAM,		T,T,	1,1,	OK+I	)/*;mds	(param)		*/
OPC (	Huge,	HUGE ,		Scalar, Keep,		Huge,		BU,HC,	BU,H,	1,1,	OK	)/*;f9	(model)			*/
OPC (	HComplex,	H_COMPLEX ,	Same,	Cmplx,		Complex,	HC,HC,	HC,HC,	1,2,	CAST+N+I )/*;%	(x,y)			*/
OPC (	HFloat,	H_FLOAT ,	Same,	Keep,		undef,		H,H,	H,H,	1,1,	CAST+N+I )/*;%	(a)			*/
OPC (	Iachar,	IACHAR ,		Same,	Keep,		Ichar,		T,T,	BU,BU,	1,1,	OK	)/*;f9	(character)		*/
OPC (	Iand,	IAND ,		Same,	Iand,		Iand,		BU,O,	BU,OU,	2,2,	IAND+S	)/*;f9	i&j iand(i,j)		*/
OPC (	IandNot,	IAND_NOT ,	Same,	Iand,		IandNot,	BU,O,	BU,OU,	2,2,	IAND	)/*;	(i,j)			*/
OPC (	Ichar,	ICHAR ,		Same,	Keep,		Ichar,		T,T,	BU,BU,	1,1,	OK	)/*;f9	(character)		*/
OPC (	IdentOf,	IDENT_OF ,	IdentOf, undef,	undef, ACTION,DISPATCH,	T,T,	1,1,	OK+I	)/*;mds	(dispatch)		*/
OPC (	If,	IF ,		If,	undef,		undef,		XX,YY,	XX,YY,	2,3,	IF+N	)/*;%cc	IF(expr)stmt		*/
OPC (	IfError,	IF_ERROR ,	IfError, undef,		undef,		XX,YY,	XX,YY,	1,254,	OK	)/*;	(a..) a, if (error) b..	*/
OPC (	ImageOf,	IMAGE_OF ,	ImageOf, undef,		undef, FUNCTION,ROUTINE,T,T,	1,1,	OK+I	)/*;mds	(ext_f conglom routine)	*/
OPC (	In,	IN ,		Else,	undef,		undef,		XX,YY,	XX,YY,	1,1,	MODIF+N	)/*;	IN a	read only	*/
OPC (	Inand,	INAND ,		Same,	Iand,		Inand,		BU,O,	BU,OU,	2,2,	IAND	)/*;	(i,j)			*/
OPC (	InandNot,	INAND_NOT ,	Same,	Iand,		InandNot,	BU,O,	BU,OU,	2,2,	IAND	)/*;	(i,j)			*/
OPC (	Index,	INDEX ,		Same,	Ttb,		Index,	T,T,SUBSCRIPT,SUBSCRIPT,2,3,	OK	)/*;f9	(string,substr,[back])	*/
OPC (	Inor,	INOR ,		Same,	Iand,		Inor,		BU,O,	BU,OU,	2,2,	IOR	)/*;	(i,j)			*/
OPC (	InorNot,	INOR_NOT ,	Same,	Iand,		InorNot,	BU,O,	BU,OU,	2,2,	IOR	)/*;	(i,j)			*/
OPC (	Inot,	INOT ,		Same,	Keep,		Inot,		BU,O,	BU,OU,	1,1,	UNARY+S	)/*;%cc	~a	\\not(a)\\	*/
OPC (	InOut,	INOUT ,		Else,	undef,		undef,		XX,YY,	XX,YY,	1,1,	MODIF+N	)/*;	INOUT a	read/write	*/
OPC (	Inquire,	INQUIRE ,	Inquire, undef,		undef,		XX,YY,	XX,YY,	2,2,	OK+U	)/*;%f9	(unit|file,select)	*/
OPC (	Int,	INT ,		Same,	Real,		undef,		B,O,	B,O,	1,2,	CAST+N+I )/*;f9	(a,[kind])		*/
OPC (	Integral,	INTEGRAL ,	Trans,	Sign,		Integral,	BU,HC,	BU,HC,	2,3,	OK	)/*;	(a,dim,[dt]) axis sum	*/
OPC (	Interpol,	INTERPOL ,	Interpol, undef,		undef,		BU,HC,	BU,HC,	2,3,	OK	)/*;	(a,n) or (a,xin,xout)	*/
OPC (	Intersect,	INTERSECT ,	Intersect, undef,	undef,		XX,YY,	XX,YY,	0,254,	OK	)/*;	(a,...)			*/
OPC (	IntUnsigned,	INT_UNSIGNED ,	Same,	Keep,		undef,		BU,OU,	BU,OU,	1,1,	CAST+N+I )/*;%	(a) (unsigned int) zext	*/
OPC (	Inverse,	INVERSE ,	Same,	Inverse,	Inverse,	F,HC,	F,HC,	1,1,	OK	)/*;	(matrix)		*/
OPC (	Ior,	IOR ,		Same,	Iand,		Ior,		BU,O,	BU,OU,	2,2,	IOR+S	)/*;f9	i|j ior(i,j)		*/
OPC (	IorNot,	IOR_NOT ,	Same,	Iand,		IorNot,	BU,O,	BU,OU,	2,2,	IOR	)/*;	(i,j)			*/
OPC (	IsIn,	IS_IN ,		IsIn,	undef,		undef,		T,HC,	VV,VV,	2,3,	IN+N	)/*;%	a IS_IN b rng/lst upcase*/
OPC (	Ieor,	IEOR ,		Same,	Iand,		Ieor,		BU,O,	BU,OU,	2,2,	IXOR	)/*;	(i,j)			*/
OPC (	IeorNot,	IEOR_NOT ,	Same,	Iand,		IeorNot,	BU,O,	BU,OU,	2,2,	IXOR	)/*;	(i,j)			*/
OPC (	Label,	LABEL ,		Label,	undef,		undef,		XX,YY,	XX,YY,	2,254,	LABEL+N+U )/*;% LABEL label: stmt	*/
OPC (	Laminate,	LAMINATE ,	Laminate, undef,		undef,		XX,YY,	XX,YY,	2,254,	OK+S	)/*;%	{{a1,a2},...{z1,z2}}	*/
OPC (	LanguageOf,	LANGUAGE_OF ,	LanguageOf, undef,	undef, PROCEDURE,PROCEDURE,	XX,YY,	1,1,	OK+I	)/*;mds	(procedure)	*/
OPC (	LastLoc,	LASTLOC ,	Trans,	Mask1,		LastLoc,	BU,O,	VV,VV,	1,2,	OK	)/*;	(mask,[dim])		*/
OPC (	Le,	LE ,		Same,	Eq,		Le,		XX,YY,	VV,VV,	2,2,	LGE+N+S	)/*;%	a<=b	a.LE.b		*/
OPC (	Len,	LEN ,		Scalar, Any,		Len,		XX,YY,	L,L,	1,1,	OK	)/*;f9+	(string or other)	*/
OPC (	LenTrim,	LEN_TRIM ,	Same,	Any,		LenTrim,	BU,T,	L,L,	1,1,	OK	)/*;f9	(string)		*/
OPC (	Lge,	LGE ,		Same,	Eq,		Ge,		T,T,	VV,VV,	2,2,	OK	)/*;f9	(string_a,string_b)	*/
OPC (	Lgt,	LGT ,		Same,	Eq,		Gt,		T,T,	VV,VV,	2,2,	OK	)/*;f9	(string_a,string_b)	*/
OPC (	Lle,	LLE ,		Same,	Eq,		Le,		T,T,	VV,VV,	2,2,	OK	)/*;f9	(string_a,string_b)	*/
OPC (	Llt,	LLT ,		Same,	Eq,		Lt,		T,T,	VV,VV,	2,2,	OK	)/*;f9	(string_a,string_b)	*/
OPC (	Log,	LOG ,		Same,	NoHc,		Log,		F,HC,	F,HC,	1,1,	OK	)/*;f9	(z)			*/
OPC (	Log10,	LOG10 ,		Same,	None,		Log10,		F,HC,	F,HC,	1,1,	OK	)/*;f9	(x)			*/
OPC (	Log2,	LOG2 ,		Same,	None,		Log2,		F,HC,	F,HC,	1,1,	OK	)/*;vax	(x)			*/
OPC (	Logical,	LOGICAL ,	Same,	Not,		Logical,	BU,O,	VV,VV,	1,2,	OK	)/*;f9	(a,[kind])		*/
OPC (	Long,	LONG ,		Same,	Keep,		undef,		L,L,	L,L,	1,1,	CAST+N+I )/*;%	(a) (signed long)	*/
OPC (	LongUnsigned,	LONG_UNSIGNED ,	Same,	Keep,		undef,		LU,LU,	LU,LU,	1,1,	CAST+N+I )/*;%	(a) (unsigned long)	*/
OPC (	Lt,	LT ,		Same,	Eq,		Lt,		XX,YY,	VV,VV,	2,2,	LGE+N+S	)/*;%	a<b	a.LT.b		*/
OPC (	Matmul,	MATMUL ,		Matmul,	undef,		undef,		BU,HC,	BU,HC,	2,2,	OK	)/*;f9	(matrix_a,matrix_b)	*/
OPC (	MatRot,	MAT_ROT ,	Matrix, undef,		MatRot,	F,HC,	F,HC,	2,5,	OK	)/*;	(mat,angle,mag,x0,y0)	*/
OPC (	MatRotInt,	MAT_ROT_INT ,	Matrix, undef,		MatRotInt,	F,HC,	F,HC,	2,5,	OK	)/*;	(mat,angle,mag,x0,y0)	*/
OPC (	Max,	MAX ,		MinMax, Add,		Max,		BU,HC,	BU,HC,	2,254,	OK	)/*;f9	(x,y..)			*/
OPC (	MaxExponent,	MAXEXPONENT ,	Scalar,Any,		MaxExponent,	F,HC,	L,L,	1,1,	OK	)/*;f9	(model)			*/
OPC (	MaxLoc,	MAXLOC ,		Trans,	Mask3,		MaxLoc,	T,HC,SUBSCRIPT,SUBSCRIPT,1,3,	OK	)/*;f9	(x,[dim],[mask])		*/
OPC (	MaxVal,	MAXVAL ,		Trans,	Mask3,		MaxVal,		T,HC,	T,HC,	1,3,	OK	)/*;f9	(x,[dim],[mask])	*/
OPC (	Mean,	MEAN ,		Trans,	Mask3,		Mean,		BU,HC,	F,HC,	1,3,	OK	)/*;	(x,[dim],[mask])	*/
OPC (	Median,	MEDIAN ,		Same,	Long2,		Median,		BU,HC,	BU,HC,	2,2,	OK	)/*;	(a,width)		*/
OPC (	Merge,	MERGE ,		Same,	Merge,		Merge,		XX,YY,	XX,YY,	3,3,	OK	)/*;f9	(tsource,fsource,mask)	*/
OPC (	MethodOf,	METHOD_OF ,	MethodOf, undef,	undef, METHOD,METHOD,	XX,YY,	1,1,	OK+I	)/*;mds	(method)		*/
OPC (	Min,	MIN ,		MinMax,Add,		Min,		BU,HC,	BU,HC,	2,254,	OK	)/*;f9	(x,y..)			*/
OPC (	MinExponent,	MINEXPONENT ,	Scalar,Any,		MinExponent,	F,HC,	L,L,	1,1,	OK	)/*;f9	(model)			*/
OPC (	MinLoc,	MINLOC ,		Trans,	Mask3,		MinLoc,	T,HC,SUBSCRIPT,SUBSCRIPT,1,3,	OK	)/*;f9	(x,[dim], [mask])		*/
OPC (	MinVal,	MINVAL ,		Trans,	Mask3,		MinVal,		T,HC,	T,HC,	1,3,	OK	)/*;f9	(x,[dim],[mask])	*/
OPC (	Mod,	MOD ,		Same,	Add,		Mod,		BU,HC,	BU,HC,	2,2,	MUL+N	)/*;f9	(a,p)			*/
OPC (	ModelOf,	MODEL_OF ,	ModelOf, undef,		undef, CONGLOM,CONGLOM,	XX,YY,	1,1,	OK+I	)/*;mds	(conglom)		*/
OPC (	Multiply,	MULTIPLY ,	Same,	Multiply,	Multiply,	BU,HC,	BU,HC,	2,2,	MUL+S	)/*;%	a*b			*/
OPC (	NameOf,	NAME_OF ,	NameOf, undef,		undef, 	CONGLOM,CONGLOM,	T,T,	1,1,	OK+I	)/*;mds	(conglom)	*/
OPC (	Nand,	NAND ,		Same,	Land,		Nand,		BU,O,	VV,VV,	2,2,	LAND+N	)/*;%	(v,w)	.NOT.(v.AND.w)	*/
OPC (	NandNot,	NAND_NOT ,	Same,	Land,		NandNot,	BU,O,	VV,VV,	2,2,	LAND+N	)/*;%	(v,w)	.NOT.v.OR.w	*/
OPC (	Ndesc,	NDESC ,		Ndesc,	undef,		undef, PARAM,WITH_UNITS,BU,BU,	1,1,	OK	)/*;mds	(class_R)		*/
OPC (	Ne,	NE ,		Same,	Eq,		Ne,		XX,YY,	VV,VV,	2,2,	LEQ+N+S	)/*;%	a!=b a/=b a<>b a.NE.b	*/
OPC (	Nearest,	NEAREST ,	Same,	Sign,		Nearest,	F,HC,	F,HC,	2,2,	OK	)/*;f9	(x,s)			*/
OPC (	Neqv,	NEQV ,		Same,	Land,		Neqv,		BU,O,	VV,VV,	2,2,	LEQV+N	)/*;%	(v,w)	v.NEQV.w	*/
OPC (	Nint,	NINT ,		Same,	Aint,		Nint,		BU,HC,	L,L,	1,2,	OK	)/*;f9	(x,[kind])		*/
OPC (	Nor,	NOR ,		Same,	Land,		Nor,		BU,O,	VV,VV,	2,2,	LOR+N	)/*;%	(v,w)	.NOT.(v.OR.w)	*/
OPC (	NorNot,	NOR_NOT ,	Same,	Land,		NorNot,	BU,O,	VV,VV,	2,2,	LOR+N	)/*;%	(v,w)	.NOT.v.AND.w	*/
OPC (	Not,	NOT ,		Same,	Not,		Not,		BU,O,	VV,VV,	1,1,	UNARY+N+S )/*;%	!a	.NOT.a		*/
OPC (	ObjectOf,	OBJECT_OF ,	ObjectOf, undef,	undef, METHOD,METHOD,	XX,YY,	1,1,	OK+I	)/*;mds	(method)		*/
OPC (	Octaword,	OCTAWORD ,	Same,	Keep,		undef,		O,O,	O,O,	1,1,	CAST+N+I )/*;%	(a)			*/
OPC (	OctawordUnsigned,	OCTAWORD_UNSIGNED ,Same,Keep,		undef,		OU,OU,	OU,OU,	1,1,	CAST+N+I )/*;%	(a)			*/
OPC (	OnError,	ON_ERROR ,	OnError, undef,		undef,		XX,YY,	XX,YY,	1,1,	OK	)/*;	(a,b..) a, on (error) b	*/
OPC (	OpcodeBuiltin,	OPCODE_BUILTIN ,	Trim,	undef,		OpcodeBuiltin,	WU,L,	T,T,	1,1,	OK	)/*;	(opcode)		*/
OPC (	OpcodeString,	OPCODE_STRING ,	Trim,	undef,		OpcodeString,	WU,L,	T,T,	1,1,	OK	)/*;	(opcode)		*/
OPC (	Fopen,	FOPEN ,		Fopen,	undef,		undef,		T,T,	L,L,	2,254,	OK+U	)/*;%cc	unit=(file,mode,[arg..])*/
OPC (	Optional,	OPTIONAL ,	Else,	undef,		undef,		XX,YY,	XX,YY,	1,1,	MODIF+N	)/*;	OPTIONAL a		*/
OPC (	Or,	OR ,		Same,	Land,		Or,		BU,O,	VV,VV,	2,2,	LOR+N	)/*;%	(v,w)	v||w	v.OR.w	*/
OPC (	OrNot,	OR_NOT ,		Same,	Land,		OrNot,		BU,O,	VV,VV,	2,2,	LOR+N	)/*;%	(v,w)	v.OR..NOT.w	*/
OPC (	Out,	OUT ,		Else,	undef,		undef,		XX,YY,	XX,YY,	1,1,	MODIF+N	)/*;	OUT a	write only	*/
OPC (	Pack,	PACK ,		Pack,	undef,		undef,		XX,YY,	XX,YY,	2,3,	OK	)/*;f9	(a,mask,[vector])	*/
OPC (	PhaseOf,	PHASE_OF ,	PhaseOf, undef,		undef, ACTION,DISPATCH,	T,T,	1,1,	OK+I	)/*;mds	(dispatch)		*/
OPC (	PostDec,	POST_DEC ,	PostDec, undef,		undef,		BU,HC,	BU,HC,	1,1,	INC+S+U	)/*;%cc	a--			*/
OPC (	PostInc,	POST_INC ,	PostInc, undef,		undef,		BU,HC,	BU,HC,	1,1,	INC+S+U	)/*;%cc	a++			*/
OPC (	Power,	POWER ,		Same,	Power,		Power,		BU,HC,	BU,HC,	2,2,	POWER+S	)/*;%	a^b a**b		*/
OPC (	Present,	PRESENT ,	Present, undef,		undef,		XX,YY,	VV,VV,	1,1,	OK	)/*;f9	(a)			*/
OPC (	PreDec,	PRE_DEC ,	PreDec, undef,		undef,		BU,HC,	BU,HC,	1,1,	INC+S+U	)/*;%cc	--a			*/
OPC (	PreInc,	PRE_INC ,	PreInc, undef,		undef,		BU,HC,	BU,HC,	1,1,	INC+S+U	)/*;%cc	++a			*/
OPC (	Private,	PRIVATE ,	Private, undef,		undef,		T,T,	XX,YY,	1,1,	MODIF+N	)/*;	PRIVATE a		*/
OPC (	ProcedureOf,	PROCEDURE_OF ,	ProcedureOf, undef,	undef, 	PROCEDURE,PROCEDURE,XX,YY,1,1,	OK+I	)/*;mds	(procedure)		*/
OPC (	Product,	PRODUCT ,	Trans,	Mask3,		Product,	BU,HC,	BU,HC,	1,3,	OK	)/*;f9	(array,[dim],[mask])	*/
OPC (	ProgramOf,	PROGRAM_OF ,	ProgramOf, undef,	undef, PROGRAM,PROGRAM,	XX,YY,	1,1,	OK+I	)/*;mds	(program)		*/
OPC (	Project,	PROJECT ,	Project, undef,		undef,		XX,YY,	XX,YY,	3,4,	OK	)/*;	(arr,mask,field,[dim])	*/
OPC (	Promote,	PROMOTE ,	Promote, undef,		undef,		XX,YY,	XX,YY,	2,2,	PROMO+S+U )/*;	ncopies@b		*/
OPC (	Public,	PUBLIC ,		Public,	undef,		undef,		T,T,	XX,YY,	1,1,	MODIF+N	)/*;	PUBLIC a		*/
OPC (	Quadword,	QUADWORD ,	Same,	Keep,		undef,		Q,Q,	Q,Q,	1,1,	CAST+N+I )/*;%	(a)			*/
OPC (	QuadwordUnsigned,	QUADWORD_UNSIGNED ,Same,Keep,		undef,		QU,QU,	QU,QU,	1,1,	CAST+N+I )/*;%	(a)			*/
OPC (	QualifiersOf,	QUALIFIERS_OF ,	QualifiersOf, undef,	undef, CONGLOM,CONGLOM,	XX,YY,	1,1,	OK+I	)/*;mds	(conglom)		*/
OPC (	Radix,	RADIX ,		Scalar,Any,		Radix,		BU,HC,	L,L,	1,1,	OK	)/*;f9	(model)			*/
OPC (	Ramp,	RAMP ,		Array,	undef,		Ramp,		XX,YY,	L,L,	0,2,	OK+U	)/*;	([size],[kind])		*/
OPC (	Random,	RANDOM ,		Array,	undef,		Random,		XX,YY,	FLOAT,F,	0,2,	OK+U	)/*;f9var ([size],[kind])	*/
OPC (	RandomSeed,	RANDOM_SEED ,	RandomSeed, undef,	undef,		L,L,	L,L,	0,1,	OK+U	)/*;f9var ([put])		*/
OPC (	DtypeRange,	DTYPE_RANGE ,	DtypeRange, undef,	undef,		BU,HC,	BU,HC,	2,3,	RANGE+S+U )/*;%	a:b:[c] a..b..[c]	*/
OPC (	Rank,	RANK ,		Scalar,Any,		Rank,		XX,YY,	L,L,	1,1,	OK	)/*;fex	(a)			*/
OPC (	RawOf,	RAW_OF ,		RawOf,	undef,		undef, SIGNAL,SIGNAL,	XX,YY,	1,1,	OK+I	)/*;mds	(signal)		*/
OPC (	Read,	READ ,		Read,	undef,		undef,		XX,YY,	T,T,	1,1,	OK	)/*;%	(unit|*,fmt|*,arg...)	*/
OPC (	Real,	REAL ,		Same,	Real,		undef,		F,H,	F,H,	1,2,	OK+I	)/*;f9	(a,[kind])		*/
OPC (	Rebin,	REBIN ,		Rebin,	undef,		undef,		BU,HC,	BU,HC,	2,4,	OK	)/*;	(a,size1,...)		*/
OPC (	Ref,	REF ,		Else,	undef,		undef,		XX,YY,	XX,YY,	1,1,	OK	)/*;vax	(any)			*/
OPC (	Repeat,	REPEAT ,		Same,	Repeat,		Repeat,		T,T,	T,T,	2,2,	OK+U	)/*;f9	(string,ncopies)	*/
OPC (	Replicate,	REPLICATE ,	Trans,	Sign,		Replicate,	XX,YY,	XX,YY,	3,3,	OK+U	)/*;fex	(array,dim,ncopies)	*/
OPC (	Reshape,	RESHAPE ,	Reshape, undef,		undef,		XX,YY,	XX,YY,	2,4,	OK	)/*;f9	(dimvec,a,[pad],[order])*/
OPC (	Return,	RETURN ,		Return,	undef,		undef,		XX,YY,	XX,YY,	0,1,	RETURN+N+U )/*;%cc (expr);		*/
OPC (	Rewind,	REWIND ,		Rewind,	undef,		undef,		L,L,	L,L,	1,1,	OK+U	)/*;%f9	(unit)			*/
OPC (	Rms,	RMS ,		Trans,	Mask3,		Rms,		F,HC,	F,H,	1,3,	OK	)/*;	(array,[dim],[mask])	*/
OPC (	RoutineOf,	ROUTINE_OF ,	RoutineOf, undef,	undef, FUNCTION,ROUTINE,T,T,	1,1,	OK+I	)/*;mds	(ext_fun routine)	*/
OPC (	RrSpacing,	RRSPACING ,	Same,	Keep,		RrSpacing,	F,HC,	F,HC,	1,1,	OK	)/*;f9	(x)			*/
OPC (	Scale,	SCALE ,		Same,	Long2,		Scale,		F,HC,	F,HC,	2,2,	OK	)/*;f9	(r,i)			*/
OPC (	Scan,	SCAN ,		Same,	Ttb,		Scan,	T,T,SUBSCRIPT,SUBSCRIPT,2,3,	OK	)/*;f9	(string,set,[back])	*/
OPC (	Fseek,	FSEEK ,		Fseek,	undef,		undef,		L,L,	L,L,	1,3,	OK+U	)/*;%cc	(unit,[offset],[origin])*/
OPC (	SetExponent,	SET_EXPONENT ,	Same,	Long2,		SetExponent,	F,HC,	F,HC,	2,2,	OK	)/*;f9	(x,i)			*/
OPC (	SetRange,	SET_RANGE ,	SetRange, undef,	undef,		XX,YY,	XX,YY,	2,1+MAXDIM,OK+I	)/*;%	(range...,name)		*/
OPC (	Ishft,	ISHFT ,		Same,	Shft,		Ishft,		BU,O,	BU,O,	2,2,	OK	)/*;f9	(a,shift)		*/
OPC (	Ishftc,	ISHFTC ,		Same,	Shft,		Ishftc,		BU,O,	BU,O,	3,3,	OK	)/*;f9	(a,shift,size)		*/
OPC (	ShiftLeft,	SHIFT_LEFT ,	Same,	Shft,		ShiftLeft,	BU,O,	BU,O,	2,2,	SHIFT+S	)/*;%cc	i<<j			*/
OPC (	ShiftRight,	SHIFT_RIGHT ,	Same,	Shft,		ShiftRight,	BU,O,	BU,O,	2,2,	SHIFT+S	)/*;%cc	i>>j sign=signed	*/
OPC (	Sign,	SIGN ,		Same,	Sign,		Sign,		BU,HC,	BU,HC,	2,2,	OK	)/*;f9	(a,b)			*/
OPC (	Signed,	SIGNED ,		Same,	Keep,		undef,		B,O,	B,O,	1,1,	CAST+N+I )/*;%	(a)			*/
OPC (	Sin,	SIN ,		Same,	NoHc,		Sin,		F,HC,	F,HC,	1,1,	OK	)/*;f9	(x) 			*/
OPC (	Sind,	SIND ,		Same,	None,		Sind,		F,HC,	F,HC,	1,1,	OK	)/*;vax	(x)			*/
OPC (	Sinh,	SINH ,		Same,	None,		Sinh,		F,HC,	F,HC,	1,1,	OK	)/*;f9	(x)			*/
OPC (	SizeOf,	SIZEOF ,		Scalar,Any,		SizeOf,		XX,YY,	L,L,	1,1,	SIZEOF+N )/*;%ccmod (expr)		*/
OPC (	SlopeOf,	SLOPE_OF ,	SlopeOf, undef,		undef, SLOPE,RANGE,	BU,HC,	1,2,	OK+I	)/*;mds	(slope range,[n])	*/
OPC (	Smooth,	SMOOTH ,		Same,	Long2,		Smooth,		BU,HC,	BU,HC,	2,2,	OK	)/*;	(a,width)		*/
OPC (	Solve,	SOLVE ,		Matrix,Divide,		Solve,		F,HC,	F,HC,	2,2,	OK	)/*;	(right,matrix)		*/
OPC (	SortVal,	SORTVAL ,	SortVal, undef,		undef,		T,HC,	T,HC,	1,2,	OK	)/*;	(array,[upcase])	*/
OPC (	Spacing,	SPACING ,	Same,	Keep,		Spacing,	F,HC,	F,HC,	1,1,	OK	)/*;f9	(x)			*/
OPC (	Spawn,	SPAWN ,		Spawn,	undef,		undef,		T,T,	L,L,	0,3,	OK+U	)/*;vax	(cmd,input,output)	*/
OPC (	Spread,	SPREAD ,		Trans,	Sign,		Spread,		XX,YY,	XX,YY,	3,3,	OK	)/*;f9	(a,dim,ncopies)		*/
OPC (	Sqrt,	SQRT ,		Same,	NoHc,		Sqrt,		F,HC,	F,HC,	1,1,	OK	)/*;f9	(z)			*/
OPC (	Square,	SQUARE ,		Same,	Square,		Square,		BU,HC,	BU,HC,	1,1,	OK	)/*;	(z) a**2		*/
OPC (	Statement,	STATEMENT ,	Statement, undef,	undef,		XX,YY,	XX,YY,	0,254,	OK+S	)/*;%cc	{a;..} evl&forget..null	*/
OPC (	StdDev,	STD_DEV ,	Trans,	Mask3,		StdDev,	F,HC,	F,HC,	1,3,	OK	)/*;	(array,[dim],[mask])	*/
OPC (	String,	STRING ,		String,	undef,		undef,		XX,YY,	T,T,	1,254,	OK	)/*;	(a,...[format])		*/
OPC (	StringOpcode,	STRING_OPCODE ,	Same,	None,		StringOpcode,	T,T,	W,W,	1,1,	OK	)/*;mds	(string)		*/
OPC (	Subscript,	SUBSCRIPT ,	Subscript, undef,	undef,		XX,YY,	XX,YY,	1,1+MAXDIM,OK+S	)/*;%cc	a[b..] range list sig	*/
OPC (	Subtract,	SUBTRACT ,	Same,	Add,		Subtract,	BU,HC,	BU,HC,	2,2,	ADD+S	)/*;%	a-b			*/
OPC (	Sum,	SUM ,		Trans,	Mask3,		Sum,		BU,HC,	BU,HC,	1,3,	OK	)/*;f9	(array,[dim],[mask])	*/
OPC (	Switch,	SWITCH ,		Switch,	undef,		undef,		XX,YY,	XX,YY,	2,254,	SWITCH+N )/*;%cc SWITCH(expr)stmt	*/
OPC (	SystemClock,	SYSTEM_CLOCK ,	Same,	None,		SystemClock,	XX,YY,	L,L,	1,1,	OK+U	)/*;f9variant (string)		*/
OPC (	Tan,	TAN ,		Same,	None,		Tan,		F,HC,	F,HC,	1,1,	OK	)/*;	(x)			*/
OPC (	Tand,	TAND ,		Same,	None,		Tand,		F,HC,	F,HC,	1,1,	OK	)/*;vax	(x)			*/
OPC (	Tanh,	TANH ,		Same,	None,		Tanh,		F,HC,	F,HC,	1,1,	OK	)/*;f9	(x)			*/
OPC (	TaskOf,	TASK_OF ,	TaskOf, undef,		undef, ACTION,ACTION,PROGRAM,METHOD,	1,1,	OK+I	)/*;mds	(action)	*/
OPC (	Text,	TEXT ,		Same,	Text,		undef,		BU,T,	T,T,	1,2,	OK	)/*;	(a,[length])		*/
OPC (	TimeoutOf,	TIME_OUT_OF ,	TimeoutOf, undef,	undef, PROGRAM,METHOD,	XX,YY,	1,1,	OK+I	)/*;mds	(prog rout proc meth)	*/
OPC (	Tiny,	TINY ,		Scalar,Keep,		Tiny,		F,HC,	F,H,	1,1,	OK	)/*;f9	(model)			*/
OPC (	Transfer,	TRANSFER ,	Transfer, undef,		undef,		XX,YY,	XX,YY,	2,3,	OK	)/*;f9	(a,mold,[size])		*/
OPC (	Transpose_,	TRANSPOSE_ ,	Matrix, undef,		Transpose,	XX,YY,	XX,YY,	1,1,	OK	)/*;f9	(matrix) 2D		*/
OPC (	Trim,	TRIM ,		Trim,	undef,		Trim,		T,T,	T,T,	1,1,	OK	)/*;f9	(string)		*/
OPC (	UnaryMinus,	UNARY_MINUS ,	Same,	Keep,		UnaryMinus,	BU,HC,	B,HC,	1,1,	UNARY+I+S )/*;%	-a			*/
OPC (	UnaryPlus,	UNARY_PLUS ,	Same,	Keep,		undef,		BU,HC,	B,HC,	1,1,	UNARY+I+S )/*;%	+a			*/
OPC (	Union,	UNION ,		Union,	undef,		undef,		XX,YY,	XX,YY,	0,254,	OK	)/*;	(a,b)			*/
OPC (	Units,	UNITS ,		Units,	undef,		undef,		XX,YY,	T,T,	1,1,	OK	)/*;mds	(with_units none)	*/
OPC (	UnitsOf,	UNITS_OF ,	UnitsOf, undef,		undef, WITH_UNITS,WITH_UNITS,UNITS,UNITS,1,1,OK+I	)/*;mds	(with_units)	*/
OPC (	Unpack,	UNPACK ,		Unpack,	undef,		undef,		XX,YY,	XX,YY,	3,3,	OK	)/*;f9	(vector,mask,field)	*/
OPC (	Unsigned,	UNSIGNED ,	Same,	Keep,		undef,		BU,OU,	BU,OU,	1,1,	CAST+N+I )/*;%	(a)			*/
OPC (	Val,	VAL ,		Else,	undef,		undef,		L,L,	XX,YY,	1,1,	OK	)/*;vax	(long)			*/
OPC (	ValidationOf,	VALIDATION_OF ,	ValidationOf, undef,	undef, PARAM,PARAM,		XX,YY,	1,1,	OK+I	)/*;mds	(param)		*/
OPC (	ValueOf,	VALUE_OF ,	ValueOf, undef,		undef,		XX,YY,	XX,YY,	1,1,	OK+I	)/*;mds	(param signal wind VMS)	*/
OPC (	Var,	VAR ,		Var,	undef,		undef,		T,T,	XX,YY,	1,2,	OK+U	)/*;	(string,[exp]) variable	*/
OPC (	Vector,	VECTOR ,		Vector,	undef,		undef,		XX,YY,	XX,YY,	0,254,	OK+I+S	)/*;%f9+ [a..z1:z2:dz..n@[this]]*/
OPC (	Verify,	VERIFY ,		Same,	Ttb,		Verify,	BU,T,SUBSCRIPT,SUBSCRIPT,2,3,	OK	)/*;f9	(string,set,[back])	*/
OPC (	Wait,	WAIT ,		Wait,	undef,		undef,		F,F,	F,F,	1,1,	OK+U	)/*;	(seconds)		*/
OPC (	WhenOf,	WHEN_OF ,	WhenOf, undef,		undef, ACTION,DISPATCH,	XX,YY,	1,1,	OK+I	)/*;mds	(dispatch)		*/
OPC (	Where,	WHERE ,		Where,	undef,		undef,		XX,YY,	XX,YY,	2,3,	WHERE+N	)/*;%f9	WHERE(expr)stmtELSEWHERE*/
OPC (	While,	WHILE ,		While,	undef,		undef,		XX,YY,	XX,YY,	2,254,	WHILE+N	)/*;%cc	WHILE(expr)stmt		*/
OPC (	WindowOf,	WINDOW_OF ,	WindowOf, undef,	undef, DIMENSION,DIMENSION,WINDOW,WINDOW,1,1,OK+I	)/*;mds	(dimension)	*/
OPC (	Word,	WORD ,		Same,	Keep,		undef,		W,W,	W,W,	1,1,	CAST+N+I )/*;%	(a) (signed short)	*/
OPC (	WordUnsigned,	WORD_UNSIGNED ,	Same,	Keep,		undef,		WU,WU,	WU,WU,	1,1,	CAST+N+I )/*;%	(a) (unsigned short)	*/
OPC (	Write,	WRITE ,		Write,	undef,		undef,		XX,YY,	L,L,	1,254,	OK+U	)/*;%	(unit|*,arg...)		*/
OPC (	Zero,	ZERO ,		Array,	undef,		Zero,		XX,YY,	FLOAT,F,	0,2,	OK+U	)/*;	([size],[kind])		*/
OPC (	2Pi,	$2PI ,		Constant, undef,	2Pi,		F,HC,	F,HC,	0,0,	CONST	)/*;	twice pi		*/
OPC (	Narg,	$NARG ,		Constant, undef,	Narg,		L,L,	L,L,	0,0,	CONST	)/*;	FUN number of arguments	*/
OPC (	Element,	ELEMENT ,	Same,	Element,	Element,	BU,T,	T,T,	3,3,	OK	)/*;dcl	(position,delim,string)	*/
OPC (	RcDroop,	RC_DROOP ,	Trans,	Sign,		RcDroop,	F,HC,	F,HC,	3,3,	OK	)/*;	(x,dim,rc-time)		*/
OPC (	ResetPrivate,	RESET_PRIVATE ,	ResetPrivate, undef,	undef,		XX,YY,	XX,YY,	0,0,	OK+U	)/*;mds	()			*/
OPC (	ResetPublic,	RESET_PUBLIC ,	ResetPublic, undef,	undef,		XX,YY,	XX,YY,	0,0,	OK+U	)/*;mds	()			*/
OPC (	ShowPrivate,	SHOW_PRIVATE ,	ShowPrivate, undef,	undef,		XX,YY,	XX,YY,	0,254,	OK+U	)/*;mds	([list...])		*/
OPC (	ShowPublic,	SHOW_PUBLIC ,	ShowPublic, undef,	undef,		XX,YY,	XX,YY,	0,254,	OK+U	)/*;mds	([list...])		*/
OPC (	ShowVm,	SHOW_VM ,	ShowVm, undef,		undef,		L,L,	XX,YY,	0,2,	OK+U	)/*;vax	(print_option, mask)	*/
OPC (	Translate,	TRANSLATE ,	Same,	Adjust,		Translate,	T,T,	T,T,	3,3,	OK	)/*;vax	(string,translat,match)	*/
OPC (	TransposeMul,	TRANSPOSE_MUL ,	Matrix, undef,		TransposeMul,	BU,FC,	BU,FC,	2,2,	OK	)/*;	(a,b)	(a+)*b		*/
OPC (	Upcase,	UPCASE ,		Same,	Adjust,		Upcase,		T,T,	T,T,	1,1,	OK	)/*;	(string)		*/
OPC (	Using,	USING ,		Using,	undef,		undef,		XX,YY,	XX,YY,	2,4,	USING+N	)/*;mds	(exp,[def],[sht],[expt])*/
OPC (	Validation,	VALIDATION ,	Validation, undef,	undef,		PARAM,PARAM,XX,YY,1,1,	OK	)/*;mds	(param)	data(val_of())	*/
OPC (	MdsDefault,	$DEFAULT ,	Trim,	undef,		MdsDefault,	XX,YY,	T,T,	0,0,	CONST	)/*;mds	tree path		*/
OPC (	Expt,	$EXPT ,		Trim,	undef,		Expt,		XX,YY,	T,T,	0,0,	CONST	)/*;mds	experiment name		*/
OPC (	Shot,	$SHOT ,		Trim,	undef,		Shot,		L,L,	L,L,	0,0,	CONST	)/*;mds	shot number		*/
OPC (	GetDbi,	GETDBI ,		GetDbi,	undef,		undef,		T,T,	T,BU,	1,2,	OK	)/*;mds	(string,[index])	*/
OPC (	Cull,	CULL ,		Cull,	undef,		undef,		XX,YY,	XX,YY,	1,4,	OK	)/*;mds	(arr dim,[dim],[sub],upc)*/
OPC (	Extend,	EXTEND ,		Extend,	undef,		undef,		XX,YY,	XX,YY,	1,4,	OK	)/*;mds	(arr dim,[dim],[sub],upc)*/
OPC (	ItoX,	I_TO_X ,		ItoX,	undef,		undef,		T,HC,	T,HC,	1,2,	OK	)/*;mds	(dimension,[indices])	*/
OPC (	XtoI,	X_TO_I ,		ItoX,	undef,		undef,		T,HC,	T,HC,	1,2,	OK	)/*;mds	(dimension,[axis_pts])	*/
OPC (	Map,	MAP ,		Map,	undef,		undef,	SUBSCRIPT,SUBSCRIPT,XX,YY,2,2,	OK	)/*;mds	(array,indices)		*/
OPC (	CompileDependency,	COMPILE_DEPENDENCY ,CompileDependency, undef, undef,T,T,DEPENDENCY,DEPENDENCY,1,1,OK+I	)/*;mds	(text)			*/
OPC (	DecompileDependency,	DECOMPILE_DEPENDENCY ,DecompileDependency, undef, undef,DEPENDENCY,DEPENDENCY,T,T,1,1,OK+I)/*;mds	(dependency)		*/
OPC (	BuildCall,	BUILD_CALL ,	Build,	undef,		undef, XX,YY,CALL,BU,		3,254,	OK+I	)/*;mds	(type,imag,rout,[arg]..)*/
OPC (	ErrorlogsOf,	ERRORLOGS_OF ,	ErrorlogsOf, undef,	undef, ACTION,ACTION,	XX,YY,	1,1,	OK+I	)/*;mds	(action)		*/
OPC (	PerformanceOf,	PERFORMANCE_OF ,	PerformanceOf, undef,	undef, ACTION,ACTION,	XX,YY,	1,1,	OK+I	)/*;mds	(action)		*/
OPC (	Xd,	XD ,		Else,	undef,		undef,		XX,YY,	DSC,DSC,1,1,	OK	)/*;mds	(any)			*/
OPC (	ConditionOf,	CONDITION_OF ,	ConditionOf, undef,	undef, CONDITION,CONDITION,XX,YY,1,1,	OK+I	)/*;mds	(condition)		*/
OPC (	Sort,	SORT ,		Sort,	undef,	undef,	T,HC,SUBSCRIPT,SUBSCRIPT,1,2,	OK	)/*;	(array,[upcase])	*/
OPC (	This,	$THIS ,		This,	undef,	undef,		XX,YY,	XX,YY,	0,0,	CONST	)/*;mds	signal param or dimen	*/
OPC (	DataWithUnits,	DATA_WITH_UNITS ,DataWithUnits, undef,	undef,		XX,YY,	XX,YY,	1,1,	OK+I	)/*;mds	(any)			*/
OPC (	Atm,	$ATM ,		Constant, undef,	P0,		F,HC,	F,HC,	0,0,	CONST	)/*;atmospheric pressure	*/
OPC (	Epsilon0,	$EPSILON0 ,	Constant, undef,	Epsilon0,	F,HC,	F,HC,	0,0,	CONST	)/*;permitivity of vacuum	*/
OPC (	Gn,	$GN ,		Constant, undef,	Gn,		F,HC,	F,HC,	0,0,	CONST	)/*;acceleration of gravity	*/
OPC (	Mu0,	$MU0 ,		Constant, undef,	Mu0,		F,HC,	F,HC,	0,0,	CONST	)/*;permeability of vacuum	*/
OPC (	Extract,	EXTRACT ,	Same,	Extract,	Extract,	BU,T,	T,T,	3,3,	OK	)/*;dcl	(start,length,string)	*/
OPC (	Finite,	FINITE ,		Same,	Any,		Finite,		T,HC,	VV,VV,	1,1,	OK	)/*;idl	(a)			*/
OPC (	BitSize,	BIT_SIZE ,	Scalar,Keep,		BitSize,	BU,HC,	L,L,	1,1,	OK	)/*;f9+ (i)			*/
OPC (	Modulo,	MODULO ,		Same,	Add,		Modulo,		BU,HC,	BU,HC,	2,2,	MUL+N	)/*;f9	(a,p)			*/
OPC (	SelectedIntKind,	SELECTED_INT_KIND ,Scalar,Any,		SelectedIntKind,BU,HC,L,L,	1,1,	OK	)/*;f9	(r)			*/
OPC (	SelectedRealKind,	SELECTED_REAL_KIND ,Scalar,Any,		SelectedRealKind,BU,HC,L,L,	1,2,	OK	)/*;f9	(p,r)			*/
OPC (	Dsql,	DSQL ,		Dsql,	undef,		undef,		XX,YY,	L,L,	1,254,	OK+U	)/*;	(string,in,...outs,...)	*/
OPC (	Isql,	ISQL ,		Isql,	undef,		undef,		T,T,	L,L,	1,1,	OK+U	)/*;	(string)		*/
OPC (	Ftell,	FTELL ,		Ftell,	undef,		undef,		L,L,	L,L,	1,1,	OK+U	)/*;%cc	pos=(unit)		*/
OPC (	MakeAction,	MAKE_ACTION ,	Make,	undef,		undef, XX,YY,ACTION,ACTION,	2,5,	OK+I	)/*;mds	(dis,task,err,comp,perf)*/
OPC (	MakeCondition,	MAKE_CONDITION ,	Make,	undef,		undef, XX,YY,CONDITION,WU,	2,2,	OK+I	)/*;mds	(opcode,condition)	*/
OPC (	MakeConglom,	MAKE_CONGLOM ,	Make,	undef,		undef, XX,YY,CONGLOM,CONGLOM,	4,4,	OK+I	)/*;mds	(image,model,name,qual)	*/
OPC (	MakeDependency,	MAKE_DEPENDENCY ,Make,	undef,		undef, XX,YY,DEPENDENCY,WU,	3,3,	OK+I	)/*;mds	(opcode,arg1,arg2)	*/
OPC (	MakeDim,	MAKE_DIM ,	Make,	undef,		undef, XX,YY,DIMENSION,DIMENSION,2,2,	OK+I	)/*;mds	(window,axis)		*/
OPC (	MakeDispatch,	MAKE_DISPATCH ,	Make,	undef,		undef, XX,YY,DISPATCH,BU,	5,5,	OK+I	)/*;mds	(type,id,phase,when,com)*/
OPC (	MakeFunction,	MAKE_FUNCTION ,	Make,	undef,		undef, XX,YY,FUNCTION,WU,	1,254,	OK+I	)/*;mds	(opcode,[arguments...])	*/
OPC (	MakeMethod,	MAKE_METHOD ,	Make,	undef,		undef, XX,YY,METHOD,METHOD,	3,254,	OK+I	)/*;mds	(timeout,method,obj,...)*/
OPC (	MakeParam,	MAKE_PARAM ,	Make,	undef,		undef, XX,YY,PARAM,PARAM,	3,3,	OK+I	)/*;mds	(value,help,validation)	*/
OPC (	MakeProcedure,	MAKE_PROCEDURE ,	Make,	undef,		undef, XX,YY,PROCEDURE,PROCEDURE,3,254,	OK+I	)/*;mds	(timeout,lang,proc,...)	*/
OPC (	MakeProgram,	MAKE_PROGRAM ,	Make,	undef,		undef, XX,YY,PROGRAM,PROGRAM,	2,2,	OK+I	)/*;mds	(timeout,program)	*/
OPC (	MakeRange,	MAKE_RANGE ,	Make,	undef,		undef, XX,YY,RANGE,RANGE,	2,3,	OK+I	)/*;mds	([low],[high],[step])	*/
OPC (	MakeRoutine,	MAKE_ROUTINE ,	Make,	undef,		undef, XX,YY,ROUTINE,ROUTINE,	3,254,	OK+I	)/*;mds	(timeout,image,rout,..)	*/
OPC (	MakeSignal,	MAKE_SIGNAL ,	Make,	undef,		undef, XX,YY,SIGNAL,SIGNAL,	2,2+MAXDIM,OK+I	)/*;mds	(data,raw,[dim...])	*/
OPC (	MakeWindow,	MAKE_WINDOW ,	Make,	undef,		undef, XX,YY,WINDOW,WINDOW,	3,3,	OK+I	)/*;mds	(istart,iend,x_at_0)	*/
OPC (	MakeWithUnits,	MAKE_WITH_UNITS ,Make,	undef,		undef, XX,YY,WITH_UNITS,WITH_UNITS,	2,2,OK+I)/*;mds	(data,units)		*/
OPC (	MakeCall,	MAKE_CALL ,	Make,	undef,		undef, XX,YY,CALL,BU,		3,254,	OK+I	)/*;mds	(type,imag,rout,[arg]..)*/
OPC (	ClassOf,	CLASS_OF ,	ClassOf, undef,		undef,		XX,YY,	BU,BU,	1,1,	OK	)/*;vms	(any)			*/
OPC (	DscptrOf,	DSCPTR_OF ,	DscptrOf, undef,	undef,		XX,YY,	XX,YY,	1,2,	OK+I	)/*;mds	(any_class_R,[num])	*/
OPC (	KindOf,	KIND_OF ,	KindOf, undef,		undef,		XX,YY,	BU,BU,	1,1,	OK	)/*;mds	(any)			*/
OPC (	NdescOf,	NDESC_OF ,	NdescOf, undef,		undef, PARAM,WITH_ERROR,BU,BU,	1,1,	OK	)/*;mds	(class_R)		*/
OPC (	Accumulate,	ACCUMULATE ,	Trans,	Mask3,		Accumulate,	BU,HC,	BU,HC,	1,3,	OK	)/*;	(x,[dim],[mask])	*/
OPC (	MakeSlope,	MAKE_SLOPE ,	Make,	undef,		undef, XX,YY,SLOPE,SLOPE,	1,254,	OK+I	)/*;mds	(slope,[begin,end]...)	*/
OPC (	Rem,	REM ,		Rem,	undef,		undef, 	XX,YY,	XX,YY,	1,254,	OK+U	)/*;	(any,...)		*/
OPC (	CompletionMessageOf,	COMPLETION_MESSAGE_OF ,CompletionMessageOf, undef, undef,ACTION,ACTION,XX,YY,1,1,OK+I	)/*;mds	(dispatch)		*/
OPC (	InterruptOf,	INTERRUPT_OF ,	InterruptOf, undef,	undef, ACTION,DISPATCH,	T,T,	1,1,	OK+I	)/*;mds	(dispatch)		*/
OPC (	Shotname,	$SHOTNAME ,	Trim,	undef,		Shotname,	T,T,	T,T,	0,0,	CONST	)/*;mds	shot number		*/
OPC (	BuildWithError,	BUILD_WITH_ERROR ,Build, undef,		undef, XX,YY,WITH_ERROR,WITH_ERROR,2,2,	OK+I	)/*;mds	(data,error)		*/
OPC (	ErrorOf,	ERROR_OF ,	ErrorOf, undef,		undef, WITH_ERROR,WITH_ERROR,XX,YY,	1,1,OK+I)/*;mds	(with_error)		*/
OPC (	MakeWithError,	MAKE_WITH_ERROR ,Make,	undef,		undef, XX,YY,WITH_ERROR,WITH_ERROR,	2,2,OK+I)/*;mds	(data,error)		*/
OPC (	DoTask,	DO_TASK ,	DoTask, undef,		undef,	ACTION,METHOD,	XX,XX,	1,1,	OK	)/*;mds	(routine/method)	*/
OPC (	IsqlSet,	ISQL_SET ,	IsqlSet, undef,		undef, 		L,L,	XX,YY,	1,3,	OK+U	)/*;	(width,head)		*/
OPC (	FS_float,	FS_FLOAT ,	Same,	Keep,		undef,		FS,FS,	FS,FS,	1,1,	CAST+N+I )/*;%	(a)			*/
OPC (	FS_complex,	FS_COMPLEX ,	Same,	Cmplx,		Complex,	FS,FS,	FSC,FSC, 1,2,	CAST+N+I )/*;%	(x,y)			*/
OPC (	FT_float,	FT_FLOAT ,	Same,	Keep,		undef,		FT,FT,	FT,FT,	1,1,	CAST+N+I )/*;%	(a)			*/
OPC (	FT_complex,	FT_COMPLEX ,	Same,	Cmplx,		Complex,	FT,FT,	FTC,FTC, 1,2,	CAST+N+I )/*;%	(x,y)			*/
OPC (   BuildOpaque, BUILD_OPAQUE ,Build, undef,         undef, XX,YY,OPAQUE,OPAQUE,2,2, OK+I    )/*;mds (data,type_string)            */
OPC (   MakeOpaque,     MAKE_OPAQUE ,   Make,   undef,          undef, XX,YY,OPAQUE,OPAQUE,     2,2,OK+I )/*;mds (data,type_string)     */
OPC (   Dict,    DICT ,           Dict,    undef,          undef,          XX,YY,  XX,YY,  0,254,  OK+I+S  )/*;%f9+ [a..z1:z2:dz..n@[this]]*/
OPC (   Tuple,   TUPLE ,          Tuple,   undef,          undef,          XX,YY,  XX,YY,  0,254,  OK+I+S  )/*;%f9+ [a..z1:z2:dz..n@[this]]*/
OPC (   List,    LIST ,           List,    undef,          undef,          XX,YY,  XX,YY,  0,254,  OK+I+S  )/*;%f9+ [a..z1:z2:dz..n@[this]]*/
COM/*
	.endm	;*/
