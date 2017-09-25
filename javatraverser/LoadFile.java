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
//Stores a file as a bonary array into a pulse file
//Arguments:
//1) File name
//2) Esperiment
//3) shot
//4) Node name
import java.io.*;
public class LoadFile
{
    public static void main(String args[])
    {
        if(args.length < 3 || args.length > 5)
        {
            System.err.println("Usage: java StoreFile <filename> <nodename> <experiment> [< shot> ");
            System.exit(0);
        }
        String fileName = args[0];
        String nodeName = args[1];
        String experiment = args[2];

        int shot = -1;
        if(args.length == 4)
        {
            try
            {
                shot = Integer.parseInt(args[3]);
            }
            catch (Exception exc)
            {
                System.err.println("Invalid shot number");
                System.exit(0);
            }
        }
        Database tree = new Database(experiment, shot);
        try {
            tree.open();
        }catch(Exception exc)
        {
            System.err.println("Cannot open experiment "+ experiment + " shot " + shot + ": " + exc);
            System.exit(0);
        }
        NidData nid = null;
        try {
            nid = tree.resolve(new PathData(nodeName), 0);
        }catch(Exception exc)
        {
            System.err.println("Cannot find node "+ nodeName);
            System.exit(0);
        }
        byte [] serialized = null;
        try {
            ByteArray ba = (ByteArray)tree.getData(nid, 0);
            serialized = ba.getByteArray();
        }catch(Exception exc)
        {
            System.err.println("Error reading data in"+ nodeName + ": " + exc);
            System.exit(0);
        }
        try {
            RandomAccessFile raf = new RandomAccessFile(fileName, "rw");
            raf.write(serialized);
            raf.close();
        }catch(Exception exc)
        {
            System.err.println("Cannot read file "+ fileName + ": " + exc);
            System.exit(0);
        }
    }
}

