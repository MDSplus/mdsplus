package jScope;

class SSHDataProvider extends MdsDataProvider
{
    public MdsConnection getConnection()
    {
        return new MdsConnectionSSH();
    }
    public MdsConnection getConnection(String arg)
    {
        return new MdsConnectionSSH(arg);
    }
}