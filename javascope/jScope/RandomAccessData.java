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
package jScope;

/* $Id$ */
import java.io.*;

/**
 * Insert the type's description here.
 * Creation date: (12/10/99 18:49:35)
 */
public class RandomAccessData {
   byte buffer[];
   int position = 0;
      
/**
 * RandomAccessData constructor comment.
 */
public RandomAccessData() {
	super();
}
public RandomAccessData(RandomAccessFile file) throws IOException {
	super();
	int len = (int)file.length();
    buffer = new byte[len];
    file.read(buffer);
}
public RandomAccessData(byte[] buffer) throws IOException {
	super();
    this.buffer = buffer;
}
/**
 * Insert the method's description here.
 * Creation date: (15/10/99 17:15:27)
 * @return long
 */
public long getFilePointer() {
	return (long)position;
}
/**
 * Insert the method's description here.
 * Creation date: (15/10/99 17:13:31)
 * @param size int
 */
public void readFully(byte data[]) {
    
    
    int len = data.length;
    for(int i = 0; i < data.length; i++)
        data[i] = buffer[position+i];
    position += data.length;
    }
/**
 * Insert the method's description here.
 * Creation date: (12/10/99 19:05:42)
 */
int readInt() throws java.io.IOException {
	int x;
	int pos = position;
	x = (short)(buffer[pos] & 0xFF);
	x <<= 8;
	x |= (short)(buffer[pos+1] & 0xFF);
	x <<= 8;
	x |= (short)(buffer[pos+2] & 0xFF);
	x <<= 8;
	x |= (short)(buffer[pos+3] & 0xFF);
	position+=4;
	return x;
}
/**
 * Insert the method's description here.
 * Creation date: (12/10/99 19:02:37)
 * @exception java.io.IOException The exception description.
 */
short readShort() throws java.io.IOException {
	short x;
	int pos = position;
	x = (short)(buffer[pos] & 0xFF);
	x <<= 8;
	x |= (short)(buffer[pos+1] & 0xFF);
	position+=2;
	return x;
}
/**
 * Insert the method's description here.
 * Creation date: (15/10/99 17:17:05)
 * @return java.lang.String
 */
public String readString() {
    System.out.println("DreadString not supported");
	return null;
}
/**
 * Insert the method's description here.
 * Creation date: (12/10/99 19:00:27)
 * @exception java.io.IOException The exception description.
 */
public void seek(long pos) throws java.io.IOException {
	position = (int)pos;
	}
/**
 * Insert the method's description here.
 * Creation date: (15/10/99 17:16:08)
 * @param amount int
 */
public void skipBytes(int amount) 
{
    position += amount;
}
}
