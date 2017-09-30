package jScope;

import java.io.IOException;
import java.io.PrintStream;

public class JiNcSource implements JiDataSource 
{
	private int mRecs = 0;
	private JiDim[] mDimArray;
	private JiNcVar[] mGattArray;
	private JiNcVar[] mVarArray;
	private RandomAccessData mInput;
	private String mName;
	// netCDF record size is sum of record variable lengths
	private int mRecordSize = 0;	

	public static final boolean DEBUG = false;

	public static final int NcDimension = 10;
	public static final int NcVariable = 11;
	public static final int NcAttribute = 12;

	public JiNcSource(String name, RandomAccessData in) throws IOException
	  {
	  mName = name;
	  mInput = in;
	  readHeader(in);
	  }  
	public static void doPad(RandomAccessData in) throws IOException
	  {
	  long current = in.getFilePointer();
 // Everything on 4 byte boundary
	  int residue = (4 - (int)(current % 4)) % 4;
	  if (DEBUG){
	      System.out.print("doPad: (current,residue) ");
	      System.out.println(current + " " + residue);
	  }
	  in.skipBytes(residue);
	  }  
	public void dump(PrintStream out)
	  {
	  out.println("netcdf " + mName + "{");
	  out.println("variables:");
	  JiVar[] vars = getVars();
	  if (vars != null){
	      for (int i=0; i < vars.length; ++i){
		  toStringVar(out, vars[i]);
	      }
	  }
	  JiVar[] gatts = getGlobalAtts();
	  if (gatts != null){
	      out.println("\n// global attributes:");
	      for (int i = 0; i < gatts.length; ++i){
		  toStringAtt(out, null, gatts[i]);
	      }
	  }

	  if (vars != null){
	      out.println("\ndata:");
	      for (int i=0; i < vars.length; ++i){
		  toStringVarData(out, vars[i]);
	      }
	  }
	  }  
	private JiVar findVariable(String name, JiNcVar[] varray)
	  {
	  JiNcVar rval = null;
	  for (int i=0; i < varray.length; ++i){
	      if (name.equals(varray[i].getName())){
		  rval = varray[i];
		  break;
	      }
	  }
	  return rval;
	  }  
	/**
	 * Get a global attribute named 'name'
	 *
	 * @exception IOException
	 * @return the global attribute named 'name'
	 */
	public JiVar getGlobalAtt(String name) throws IOException
	  {
	  return findVariable(name, mGattArray);
	  }  
	/**
	 * Get all the global attributes for this source
	 *
	 * @return Vector containing global attributes
	 */
	public JiVar[] getGlobalAtts()
	  {
	  JiVar[] rvar = new JiVar[mGattArray.length];
	  for (int i=0; i < mGattArray.length; ++i){
	      rvar[i] = mGattArray[i];
	  }
	  return rvar;
	  }  
	public RandomAccessData getInput()
	  {
	  return mInput;
	  }  
	public int getNumRecords()
	  {
	  return mRecs;
	  }  
	public int getRecordSize()
	  {
	  return mRecordSize;
	  }  
	/**
	 * Get a variable named 'name'
	 *
	 * @exception IOException
	 * @return the variable named 'name'
	 */
	public JiVar getVar(String name) throws IOException
	  {
	  return findVariable(name, mVarArray);
	  }  
	/**
	 * Get all the variables for this source
	 *
	 * @return Vector containing variables
	 */
	public JiVar[] getVars()
	  {
	  JiVar[] rvar = new JiVar[mVarArray.length];
	  for (int i=0; i < mVarArray.length; ++i){
	      rvar[i] = mVarArray[i];
	  }
	  return rvar;
	  }  
	private JiNcVar readAttribute(RandomAccessData in) throws IOException
	  {
	  String name = readString(in);
	  int type = in.readInt();
	  int nelems = in.readInt();

	  if (DEBUG){
	      System.out.print("readAttribute: (name,type,nelems) ");
	      System.out.println(name + " " + type + " " + nelems);
	  }

	  JiDim dims[] = new JiDim[1];
	  dims[0] = new JiDim(null, 0, nelems);
	  JiNcVar nvar = new JiNcVar(this, name, dims, in.getFilePointer(), type,
				     false);
	  in.skipBytes(nvar.size());
	  doPad(in);
	  return nvar;
	  }  
	private JiNcVar[] readAttributes(RandomAccessData in) throws IOException
	  {
	  int type = in.readInt();
	  int nelems = in.readInt();

	  if (DEBUG){
	      System.out.println("readAttributes: (type,nelems) " +
				 type + " " + nelems);
	  }

	  if (type == 0 && nelems == 0)
	    return null;
	  if (type != NcAttribute)
	    throw new IOException("Bad file -- expected netCDF global atts");
	  JiNcVar[] rval = new JiNcVar[nelems];
	  for (int i=0; i < nelems; i++){
	      rval[i] = readAttribute(in);
	  }
	  return rval;
	  }  
	private void readDims(RandomAccessData in) throws IOException
	  {
	  int type = in.readInt();
	  int nelems = in.readInt();
	  if (DEBUG){
	      System.out.println("readDims: type " + type + " nelems " + nelems);
	  }
	  if (type == 0 && nelems == 0)
	    return;
	  if (type != NcDimension)
	    throw new IOException("Bad file -- expected netCDF dimensions");
	  mDimArray = new JiDim[nelems];
	  for (int i=0; i < nelems; i++){
	      String name = readString(in);
	      int size = in.readInt();
	      if (DEBUG){
		  System.out.println("readDims: (name, size) " + name + " " + size);
	      }
	      mDimArray[i] = new JiDim(name, 0, size);
	  }
	  }  
	private void readGatts(RandomAccessData in) throws IOException
	  {
	  int type = in.readInt();
	  int nelems = in.readInt();

	  if (DEBUG){
	      System.out.println("readGatts: (type,nelems) " + type + " " + nelems);
	  }

	  if (type == 0 && nelems == 0)
	    return;
	  if (type != NcAttribute)
	    throw new IOException("Bad file -- expected netCDF global atts");
	  mGattArray = new JiNcVar[nelems];
	  for (int i=0; i < nelems; i++){
	      mGattArray[i] = readAttribute(in);
	  }
	  }  
	private void readHeader(RandomAccessData in) throws IOException
	  {
	  byte b[] = new byte[4];
	  in.readFully(b);
	  if (!(b[0] == 'C' && b[1] == 'D' && b[2] == 'F' &&
		b[3] == 1)){
	      throw new IOException("Not a netCDF file");
	  }
	  mRecs = in.readInt();
	  readDims(in);
	  readGatts(in);
	  readVars(in);
	  if (DEBUG){
	      System.out.println("readHeader: (recs, recsize): " +
				 mRecs + " " + mRecordSize);
	  }

	  }  
	private String readString(RandomAccessData in) throws IOException
	  {
	  int nelems = in.readInt();
	  if (nelems < 0)
	    throw new IOException("Bad netCDF string");
	  if (DEBUG){
	      System.out.println("readString: nelems " + nelems);
	  }
	  byte b[] = new byte[nelems];
	  in.readFully(b);
	  doPad(in);
	  return new String(b);
	  }  
	private JiNcVar readVar(RandomAccessData in) throws IOException
	  {
	  String name = readString(in);
	  int nelems = in.readInt();
	  JiDim[] dims = new JiDim[nelems];
	  for (int i=0; i < nelems; i++){
	      int dimid = in.readInt();
	      dims[i] = (JiDim)mDimArray[dimid].clone();
	  }
	  boolean isRecord = false;
	  if (dims[0].mCount == 0){
	      isRecord = true;
	      dims[0].mCount = getNumRecords();
	  }

	  JiNcVar[] atts = readAttributes(in);
	  int type = in.readInt();
	  int size = in.readInt();
	  int offset = in.readInt();
	  if (DEBUG){
	      System.out.print("readVar: (name,rank,type,size,offset,isRecord) ");
	      System.out.println(name + " " + nelems + " " + type + " " + size
				 + " " + offset + " " + isRecord);
	  }
	  JiNcVar nvar = new JiNcVar(this, name, dims, offset, type, isRecord);
	  nvar.addAtts(atts);
	  if (isRecord){
	      mRecordSize += size;
	  }
	  return nvar;
	  }  
	private void readVars(RandomAccessData in) throws IOException
	  {
	  int type = in.readInt();
	  int nelems = in.readInt();

	  if (DEBUG){
	      System.out.println("readVars: (type,nelems) " + type + " " + nelems);
	  }

	  if (type == 0 && nelems == 0)
	    return;
	  if (type != NcVariable)
	    throw new IOException("Bad file -- expected netCDF global atts");
	  mVarArray = new JiNcVar[nelems];
	  for (int i=0; i < nelems; i++){
	      mVarArray[i] = readVar(in);
	  }
	  }  
	private void toStringAtt(PrintStream out, JiVar parent, JiVar v)
	  {
	  if (parent == null){
	      out.print("\t\t:");
	  } else {
	      out.print("\t\t" + parent.getName() + ":");
	  }
	  out.print(v.getName() + " = ");
	  toStringData(out, v);
	  }  
	private void toStringData(PrintStream out, JiVar v)
	  {
	  try {
	      byte bytes[] = null;
	      char chars[] = null;
	      short shorts[] = null;
	      int ints[] = null;
	      float floats[] = null;
	      double doubles[] = null;
	      Object result = v.read(v.getDims());
	      int maxPerLine = 8;

	      if (result instanceof byte[]){
		  bytes = (byte[])result;
		  out.print(bytes[0]);
		  for (int i=1; i < bytes.length; ++i){
		      out.print(", " + bytes[i]);
		      if (i%maxPerLine == 0)
			out.println();
		  }
		  out.println(";");
	      } else if (result instanceof char[]){
		  chars = (char[])result;
		  out.println("\"" + new String(chars) + "\";");
	      } else if (result instanceof short[]){
		  shorts = (short[])result;
		  out.print(shorts[0] + "s");
		  for (short i=1; i < shorts.length; ++i){
		      out.print(", " + shorts[i] + "s");
		      if (i%maxPerLine == 0)
			out.println();
		  }
		  out.println(";");
	      } else if (result instanceof int[]){
		  ints = (int[])result;
		  out.print(ints[0]);
		  for (int i=1; i < ints.length; ++i){
		      out.print(", " + ints[i]);
		      if (i%maxPerLine == 0)
			out.println();
		  }
		  out.println(";");
	      } else if (result instanceof float[]){
		  floats = (float[])result;
		  out.print(floats[0]);
		  for (int i=1; i < floats.length; ++i){
		      out.print(", " + floats[i] + "f");
		      if (i%maxPerLine == 0)
			out.println();
		  }
		  out.println(";");
	      } else if (result instanceof double[]){
		  doubles = (double[])result;
		  out.print(doubles[0]);
		  for (int i=1; i < doubles.length; ++i){
		      out.print(", " + doubles[i] + "d");
		      if (i%maxPerLine == 0)
			out.println();
		  }
		  out.println(";");
	      } 
	  } catch (IOException e) {
	      System.out.println(e);
	  }
	  }  
	private void toStringVar(PrintStream out, JiVar v)
	  {
	  JiDim[] dims = v.getDims();
		  out.print("\t" + v.getTypeString() + " " + v.getName() + "(");
	  out.print(dims[0].getName());
	  for (int j=1; j < dims.length; ++j){
	      out.print(", " + dims[j].getName());
	  }
	  out.println(") ;");
	  JiVar [] atts = v.getAtts();
	  if (atts != null){
	      for (int i=0; i < atts.length; ++i){
		  toStringAtt(out, v, atts[i]);
	      }
	  }
	  }  
	private void toStringVarData(PrintStream out, JiVar v)
	  {
	  out.print(v.getName() + " = ");
		  toStringData(out, v);
	  }  
}
