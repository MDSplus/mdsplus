class DemoProvider implements DataProvider 
{
    String error = null;
    
    public void SetEnvironment(String exp)
    {
        error = null;
    }
    public void Update(String exp, int s)
    {
        error = null;
    }
    public String GetString(String in)
    {
        error = null;
        return new String(in);
    }
    public float GetFloat(String in)
    {
        error = null;
        Float f = new Float(in); 
        return f.floatValue();
    }
    public float[] GetFloatArray(String in)
    {
        float d[] = new float[1000];
        
        error = null;
        if(!in.equals("sin") &&
           !in.equals("cos") &&
           !in.equals("sin*cos") &&
           !in.equals("sin_x") &&
           !in.equals("cos_x") &&
           !in.equals("sin*cos_x"))
        {
            error = new String("Not a demo signal \n Demo signals name are \n sin \n cos \n sin*cos \n");
            return null;
        }
        for(int i = 0 ; i < 1000; i++)
        {
            if(in.equals("sin")) {
                d[i] = (float)Math.sin(6.28/1000*i);
                continue;               
            }
            if(in.equals("cos")) {
                d[i] = (float)Math.cos(6.28/1000*i);
                continue;               
            }
            if(in.equals("sin*cos")) {
                d[i] = (float)(Math.sin(6.28/1000*i) * Math.cos(6.28/1000*i));
                continue;               
            }
            if(in.indexOf("_x") != -1)
                d[i] = (float)6.28/1000*i;
        }
        return d;
    }
    public int[] GetIntArray(String in)
    {
        error = null;
        int d[] = new int[1000];
        for(int i = 0 ; i < 1000; i++)
            d[i] = i;
        return d;
        
    }
    public String GetXSpecification(String in)
    {
        error = null;
        return new String(in+"_x");
    }
    public String ErrorString()
    {
        return error;
    }
    public boolean SupportsAsynch()
    {
        return false;
    }
    public void addMdsEventListener(MdsEventListener l, String event)
    {
    }
    public void removeMdsEventListener(MdsEventListener l, String event)
    {
    }
    public String GetDefaultTitle(String in_y[])
    {
        return null;
    }
    public String GetDefaultXLabel(String in_y[])
    {
        return null;
        
    }
    public String GetDefaultYLabel(String in_y[])
    {
        return null;
    }
 }	    