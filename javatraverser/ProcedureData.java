//package jTraverser;

public class ProcedureData extends CompoundData
{
    public static Data getData() {return new ProcedureData(); }
    public ProcedureData() {dtype = DTYPE_PROCEDURE; }
    public ProcedureData(Data timeout, Data language, Data procedure,
	Data [] arguments)
    {
	int ndescs;
	dtype = DTYPE_PROCEDURE;
	if(arguments != null)
	    ndescs = 3 + arguments.length;
	else
	    ndescs = 3;
	descs = new Data[ndescs];
	descs[0] = timeout;
	descs[1] = language;
	descs[2] = procedure;
	for(int i = 3; i < ndescs; i++)
	    descs[i] = arguments[i-3];
    }

     public final Data getTimeout() {return descs[0]; }
     public final Data getLanguage(){return descs[1]; }
     public final Data getProcedure() {return descs[2]; }
     public final Data[] getArguments()
     {
	Data []ris = new Data[descs.length-3];
	for(int i = 0; i < descs.length-3; i++)
	    ris[i] = descs[3+i];
	return ris;
     }
    }

