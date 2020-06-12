package mds.jdispatcher;

interface ServerListener
{
	public void actionAborted(ServerEvent event);

	public void actionFinished(ServerEvent event);

	public void actionStarting(ServerEvent event);

	public void connected(ServerEvent event);

	public void disconnected(ServerEvent event);
}
