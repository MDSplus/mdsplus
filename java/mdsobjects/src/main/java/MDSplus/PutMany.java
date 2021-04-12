package MDSplus;

public interface PutMany
{
	public void append(java.lang.String path, java.lang.String expr, Data args[]);

	public void insert(java.lang.String path, java.lang.String prevName, java.lang.String expr, Data args[]);

	public void remove(java.lang.String path);

	public void execute() throws MdsException;

	public void checkStatus(java.lang.String path) throws MdsException;
}
