import java.awt.AWTEvent;
import java.awt.Event;

public class WaveformEvent extends AWTEvent {

    static final int POINT_UPDATE     = AWTEvent.RESERVED_ID_MAX + 1;
    static final int MEASURE_UPDATE   = AWTEvent.RESERVED_ID_MAX + 2;
    static final int STATUS_INFO      = AWTEvent.RESERVED_ID_MAX + 3;
    static final int BROADCAST_SCALE  = AWTEvent.RESERVED_ID_MAX + 4;
    static final int COPY_PASTE       = AWTEvent.RESERVED_ID_MAX + 5;
    static final int EVENT_UPDATE     = AWTEvent.RESERVED_ID_MAX + 8;
    
    int    signal_idx;
    double point_x;
    double point_y;
    double delta_x;
    double delta_y;
    String signal_name;
    String status_info;

    public WaveformEvent (Object source, int event_id, String status_info) 
    {
        super(source, event_id);
        this.status_info = status_info;
    }

    public WaveformEvent (Object source, int event_id) 
    {
        super(source, event_id);
    }


    public WaveformEvent (Object source, String status_info) 
    {
        super(source, STATUS_INFO);
        this.status_info = status_info;
    }
    
    public WaveformEvent (Object source, int event_id, 
                          double point_x, double point_y,
                          double delta_x, double delta_y, 
                          int signal_idx) 
    {
        super(source, event_id);
        this.signal_idx = signal_idx;
        this.point_x = point_x;
        this.point_y = point_y;
        this.delta_x = delta_x;
        this.delta_y = delta_y;
    }
 
}
