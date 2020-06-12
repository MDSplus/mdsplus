package mds.jdispatcher;

interface MonitorListener
{
	public void beginSequence(MonitorEvent event);

	public void build(MonitorEvent event);

	public void buildBegin(MonitorEvent event);

	public void buildEnd(MonitorEvent event);

	public void connect(MonitorEvent event);

	public void disconnect(MonitorEvent event);

	public void dispatched(MonitorEvent event);

	public void doing(MonitorEvent event);

	public void done(MonitorEvent event);

	public void endPhase(MonitorEvent event);

	public void endSequence(MonitorEvent event);

	public void startPhase(MonitorEvent event);
}
