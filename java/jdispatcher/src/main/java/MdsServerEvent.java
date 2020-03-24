import java.util.*;

class MdsServerEvent extends EventObject
{
    static final int SrvJobABORTED     =1;
    static final int SrvJobSTARTING    =2;
    static final int SrvJobFINISHED    =3;
    static final int SrvJobJAMMED      =4;

    int jobid;
    int flags;
    int status;

    public MdsServerEvent(Object source, int jobid, int flags, int status)
    {
	super(source);
	this.jobid = jobid;
	this.flags = flags;
	this.status = status;
    }
    public int getJobid() {return jobid;}
    public int getFlags() {return flags;}
    public int getStatus(){return status;}

    public void setJobid(int jobid) {this.jobid = jobid;}
    public void setFlags(int flags) {this.flags = flags;}
    public void setStatus(int status){this.status = status;}

}