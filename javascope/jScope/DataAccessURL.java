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
import java.util.Vector;
import java.io.IOException;

class DataAccessURL  
{
    
    static Vector<DataAccess> dataAccessVector = new Vector<>();
        
    static public void addProtocol(DataAccess dataAccess)
    {
        dataAccessVector.addElement(dataAccess);
    }

    static public Signal getSignal(String url) throws IOException
    {
        return getSignal(url, null, null);
    }

    static public Signal getSignal(String url, String passwd) throws IOException
    {
        return getSignal(url, null, passwd);
    }
    
    static public DataAccess getDataAccess(String url) throws IOException
    {
        for (DataAccess da : dataAccessVector)
            if (da.supports(url))
                return da;
        throw(new IOException("Protocol not recognized"));
    }
    
    static public Signal getSignal(String url, String name, String passwd) throws IOException
    {
        DataAccess da = null;
        
        if((da = getDataAccess(url)) != null)
        {
            da.setPassword(passwd);
            Signal s = da.getSignal(url);
            if(s == null && da.getError() == null)
                throw(new IOException("Incorrect password or read signal error"));
            
            if(da.getError() == null)
            {
                if(name == null)
                    name = s.getName();
                    
                if(name == null)
                    name = da.getSignalName()+" "+da.getShot();
                else
                    name = name+" "+da.getShot();                
                s.setName(name);
                return s;
            }
            else
            {
                throw(new IOException(da.getError()));
            }
        }
        return null;
    }    

    static public void getImages(String url, Frames f) throws Exception
    {
        getImages(url, null, null, f);
    }

    static public void getImages(String url, String passwd, Frames f) throws Exception
    {
        getImages(url, null, passwd, f);
    }

    static public void getImages(String url, String name, String passwd, Frames f) throws Exception
    {
        DataAccess da = null;
        
        if((da = getDataAccess(url)) != null || f == null)
        {
            da.setPassword(passwd);
            FrameData fd = da.getFrameData(url);
            if(fd == null && da.getError() == null)
                throw(new IOException("Incorrect password or read images error"));
            
            f.SetFrameData(fd);
            f.setName(da.getSignalName());
            
            if(da.getError() != null)
            {
                throw(new IOException(da.getError()));
            }
        }
        else
            throw(new IOException("Protocol not recognized"));
    }

    static public void close()
    {
        for (DataAccess da : dataAccessVector)
            if (da != null)
                da.close();
    }

    static public int getNumProtocols()
    {
        return dataAccessVector.size();
    }

}
