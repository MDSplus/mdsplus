interface ServerListener
{
    public void actionStarting(ServerEvent event);
    public void actionFinished(ServerEvent event);
    public void actionAborted(ServerEvent event);
}
