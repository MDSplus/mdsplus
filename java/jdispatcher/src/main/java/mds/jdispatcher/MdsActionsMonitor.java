package mds.jdispatcher;

class MdsActionsMonitor extends MdsMonitor
{
	public MdsActionsMonitor(int port)
	{
		super(port);
	}

	// silent unwanted events
	@Override
	public void beginSequence(MonitorEvent event)
	{}

	@Override
	public void build(MonitorEvent event)
	{}

	@Override
	public void buildBegin(MonitorEvent event)
	{}

	@Override
	public void buildEnd(MonitorEvent event)
	{}

	@Override
	public void connect(MonitorEvent event)
	{}

	@Override
	public void disconnect(MonitorEvent event)
	{}

	@Override
	public void dispatched(MonitorEvent event)
	{}

	@Override
	public void doing(MonitorEvent event)
	{}

	@Override
	public void endPhase(MonitorEvent event)
	{}

	@Override
	public void endSequence(MonitorEvent event)
	{}

	@Override
	public void startPhase(MonitorEvent event)
	{}
}
