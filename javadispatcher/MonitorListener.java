interface MonitorListener
{
    public void buildBegin(MonitorEvent event);
    public void build(MonitorEvent event);
    public void buildEnd(MonitorEvent event);
    public void dispatched(MonitorEvent event);
    public void doing(MonitorEvent event);
    public void done(MonitorEvent event);
}