package MDSplus;

public interface GetMany
{
	public void append(java.lang.String name, java.lang.String expr, Data args[]);

	public void insert(java.lang.String name, java.lang.String prevName, java.lang.String expr, Data args[]);

	public void remove(java.lang.String name);

	public void execute() throws MdsException;

	public Data get(java.lang.String name) throws MdsException;
}
