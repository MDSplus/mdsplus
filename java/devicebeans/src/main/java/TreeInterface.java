
public interface TreeInterface {
    public String getName();
    public int getShot();
    public void setDefault(int nid) throws Exception;
    public int getDefault() throws Exception;
    public String getFullPath(int nid) throws Exception;
    public void doDeviceMethod(int nid, String method) throws Exception;
    public String getDataExpr(int nid)  throws Exception;
    public int getInt(String expr) throws Exception;
    public float getFloat(String expr) throws Exception;
    public int[] getIntArray(String expr) throws Exception;
    public float[] getFloatArray(String expr) throws Exception;
    public double getDouble(String expr) throws Exception;
    public String getString(String expr) throws Exception;
    public String []getStringArray(String expr) throws Exception;
    public String getNodeName(int nid) throws Exception;
    public String execute(String expr) throws Exception;
    public String evaluate(String expr) throws Exception;
    public void putDataExpr(int nid, String epr)  throws Exception;
    public boolean isString(String expr);
    public String getUsage(int nid);
    public boolean isOn(int nid)  throws Exception;
    public void setOn(int nid, boolean on)  throws Exception;
    public int getNumConglomerateNids(int nid)  throws Exception;
    public int getNode(String path)  throws Exception;
    
  }
