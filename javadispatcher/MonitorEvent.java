import java.util.*;

class MonitorEvent extends EventObject
{
    String tree;
    int shot;
    String phase;
    Action action;

    public MonitorEvent(Object obj, String tree, int shot, String phase, Action action)
    {
        super(obj);
        this.tree = tree;
        this.shot = shot;
        this.phase = phase;
        this.action = action;
    }
    
    String getTree() {return tree; }
    int getShot() {return shot; }
    String getPhase() {return phase; }
    Action getAction() {return action; }
    
}