import java.io.*;
import java.net.*;

class MdsMessage extends Object
{

final byte MAX_DIM = 8;
//final byte DTYPE_CSTRING = 14;
final byte JAVA_CLIENT = 3;
//final byte DTYPE_CHAR = 6;
//final byte DTYPE_SHORT = 7;
//final byte DTYPE_LONG = 8;
//final byte DTYPE_FLOAT = 10;

 static   int msgid = 0;
    	  int msglen;
public    int status;
public    short length;
public    byte nargs;
public    byte descr_idx;
public    byte message_id;
public    byte dtype;
public    byte client_type;
public    byte ndims;
public    int dims[];
public    byte body[];
    
public MdsMessage(String s)
{
    msglen = s.length() +  48;
    status = 0;
    length = (short)s.length();
    nargs = 0;
    descr_idx = 0;
    ndims = 0;
    dims = new int[MAX_DIM];
    for(int i = 0; i < MAX_DIM; i++)
	dims[i] = 0;
    dtype = Descriptor.DTYPE_CSTRING;
    client_type = (byte)(JAVA_CLIENT | 0x80);
    body = s.getBytes();
}

private void ReadBuf(byte buf[], DataInputStream dis) throws IOException
{
    int bytes_to_read = buf.length, read_bytes = 0, curr_offset = 0;
    while(bytes_to_read > 0)
    {
	read_bytes = dis.read(buf, curr_offset, bytes_to_read);
	curr_offset += read_bytes;
	bytes_to_read -= read_bytes;
    }
}
      
	

public void Send(DataOutputStream dos) throws IOException
{

    msglen = 48 + body.length;
    dos.writeInt(msglen);
    dos.writeInt(status);
    dos.writeShort((int)length);
    dos.writeByte(nargs);
    dos.writeByte(descr_idx);
    dos.writeByte(msgid++);
    dos.writeByte(dtype);
    dos.writeByte(client_type);
    dos.writeByte(ndims);
    for(int i = 0; i < MAX_DIM; i++)
	dos.writeInt(dims[i]);
    dos.write(body, 0, length);
    dos.flush();
    message_id++;
}
	
	 
public void Receive(DataInputStream dis)throws IOException
{
    byte bytes[];
    int i1, i2;
    bytes = new byte[4];
    ReadBuf(bytes, dis);
    msglen = ToInt(bytes);
    ReadBuf(bytes, dis);
    status = ToInt(bytes);
    bytes = new byte[2];
    ReadBuf(bytes, dis);
    length = ToShort(bytes);
    nargs = dis.readByte();
    descr_idx = dis.readByte();
    message_id = dis.readByte();
    dtype = dis.readByte();
    client_type = dis.readByte();
    ndims = dis.readByte();
    bytes = new byte[4];
    for(int i = 0; i < MAX_DIM; i++)
    {
        ReadBuf(bytes, dis);
        dims[i] = ToInt(bytes);
    }   
    if(msglen > 48)
    {
        body = new byte[msglen - 48];
        ReadBuf(body, dis);
    }
 }	
 private void Flip(byte bytes[], int size)
 {
    int i;
    byte b;
    for(i = 0; i < bytes.length; i += size)
    {
    
        if(size == 2)
	{
	    b = bytes[i];
	    bytes[i] = bytes[i+1];
	    bytes[i+1] = b;
	}
	else if(size == 4)
	{
	    b = bytes[i];
	    bytes[i] = bytes[i+3];
	    bytes[i+3] = b;
	    b = bytes[i+1];
	    bytes[i+1] = bytes[i+2];
	    bytes[i+2] = b;
	}
    }
}
private int ToInt(byte bytes[]) throws IOException
{
    Flip(bytes, 4);
    ByteArrayInputStream bis = new ByteArrayInputStream(bytes);
    DataInputStream dis = new DataInputStream(bis);
    return dis.readInt();
}
private short ToShort(byte bytes[]) throws IOException
{
    Flip(bytes, 2);
    ByteArrayInputStream bis = new ByteArrayInputStream(bytes);
    DataInputStream dis = new DataInputStream(bis);
    return dis.readShort();
}
private float ToFloat(byte bytes[]) throws IOException
{
    Flip(bytes, 4);
    ByteArrayInputStream bis = new ByteArrayInputStream(bytes);
    DataInputStream dis = new DataInputStream(bis);
    return dis.readFloat();
}
public int[] ToIntArray() throws IOException
{
    Flip(body, 4);
    ByteArrayInputStream bis = new ByteArrayInputStream(body);
    DataInputStream dis = new DataInputStream(bis);
    int out[] = new int[body.length / 4];
    for(int i = 0; i < body.length / 4; i++)
	out[i] = dis.readInt();
    return out;
}
public short[] ToShortArray() throws IOException
{
    Flip(body, 2);
    ByteArrayInputStream bis = new ByteArrayInputStream(body);
    DataInputStream dis = new DataInputStream(bis);
    short out[] = new short[body.length / 2];
    for(int i = 0; i < body.length / 2; i++)
	out[i] = dis.readShort();
    return out;
}
public double[] ToDoubleArray() throws IOException
{
    Flip(body, 4);
    ByteArrayInputStream bis = new ByteArrayInputStream(body);
    DataInputStream dis = new DataInputStream(bis);
    double out[] = new double[body.length / 4];
    for(int i = 0, j = 0; i < body.length / 4; i++, j += 4)
	if(IsRoprand(body, j))
	    out[i] = Double.NaN;
	else
	    out[i] = (double)dis.readFloat();
    return out;
}
 
private boolean IsRoprand(byte arr[], int idx)
{
    return  (arr[idx] == 0 && arr[idx + 1] == 0 && arr[idx + 2] == -128
	&& arr[idx + 3] == 0);
}  
       
}
