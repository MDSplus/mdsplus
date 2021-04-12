package mds.devices;

public interface Interface
{
	interface Setup
	{
		public void dataChanged(int... nids);
	}

	public void dataChanged(int... nids);

	public void doDeviceMethod(int nid, String method) throws Exception;

	public String execute(String expr) throws Exception;

	public String getDataExpr(int nid) throws Exception;

	public int getDefault() throws Exception;

	public double getDouble(String expr) throws Exception;

	public float getFloat(String expr) throws Exception;

	public float[] getFloatArray(String expr) throws Exception;

	public String getFullPath(int nid) throws Exception;

	public int getInt(String expr) throws Exception;

	public int[] getIntArray(String expr) throws Exception;

	public String getName();

	public int getNode(String path) throws Exception;

	public String getNodeName(int nid) throws Exception;

	public int getNumConglomerateNids(int nid) throws Exception;

	public int getShot();

	public String getString(String expr) throws Exception;

	public String[] getStringArray(String expr) throws Exception;

	public String getUsage(int nid) throws Exception;

	public boolean isOn(int nid) throws Exception;

	public void putDataExpr(int nid, String expr) throws Exception;

	public void setDefault(int nid) throws Exception;

	public void setOn(int nid, boolean on) throws Exception;
}
