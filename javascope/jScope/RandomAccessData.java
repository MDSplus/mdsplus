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
