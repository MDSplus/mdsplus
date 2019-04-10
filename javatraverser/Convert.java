import java.io.*;
import java.util.*;

class Convert
{
    String path, fileName;
    int shotNum = -1;
    float data[];
    public Convert(String path, String fileName)
    {
	this.path = path;
	this.fileName = fileName;
	loadData();
    }

    public Convert(String path, String fileName, int shotNum)
    {
	this.path = path;
	this.fileName = fileName;
	this.shotNum = shotNum;
	loadData();
    }


    public static void main(String args[])
    {
	if(args.length == 2)
	{
	    Convert conv = new Convert(args[0], args[1]);
	    conv.convertMatrix();
	}
	else if(args.length == 1)
	{
	   Convert conv = new Convert("", args[0] + ".dat");
	   int key = conv.getKey();
	   try {
	       BufferedWriter bw = new BufferedWriter(new FileWriter(args[0] +
	               ".key"));
	       bw.write(""+key);
	       bw.close();
	   }catch(Exception exc)
	   {
	       System.err.println(exc);
	   }
	}
    }

    void loadData()
    {
	data = new float[192*192];
	if(fileName.equalsIgnoreCase("diagonal"))
	{
	    for(int i = 0; i < 192; i++)
	    {
	        for(int j = 0; j < 192; j++)
	        {
	            if(i == j)
	                data[192*i+j] = 1;
	            else
	                data[192*i+j] = 0;
	        }
	    }
	}
	else
	{
	    try {
	        BufferedReader br = new BufferedReader(new FileReader(fileName));
	        for(int i = 0; i < 192; i++)
	        {
	            System.out.println(i);
	            String line = br.readLine();
	            StringTokenizer st = new StringTokenizer(line, "[], ");
	            for(int j = 0; j < 192; j++)
	                data[192*i+j] = Float.parseFloat(st.nextToken());
	        }
	    }catch(Exception exc){System.err.println(exc);}
	 }
    }

    static int getKey(float data[])
    {
	int outHash = 0;
	for(int i = 0; i < 192 * 192; i++)
	{
	    int intVal = Float.floatToIntBits(data[i]);
	    outHash = 37 * outHash + intVal;
	}
	return outHash;
    }

    int getKey() {return getKey(data);}
    public void convertMatrix()
    {
	System.out.println(path);
	Database rfx = new Database("rfx", shotNum);
	 try {
	    rfx.open();
	    NidData nid = rfx.resolve(new PathData(path), 0);
	    FloatArray array = new FloatArray(data);
	    rfx.putData(nid, array, 0);
	    rfx.close(0);
	 }catch(Exception exc){System.err.println(exc);}
    }

    public void convertMatrix(Database db)
    {
	System.out.println(path);
	 try {
	    NidData nid = db.resolve(new PathData(path), 0);
	    FloatArray array = new FloatArray(data);
	    db.putData(nid, array, 0);
	 }catch(Exception exc){System.err.println(exc);}
    }


 }

