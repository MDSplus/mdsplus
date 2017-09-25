/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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

