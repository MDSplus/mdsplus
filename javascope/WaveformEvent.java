import java.awt.AWTEvent;
import java.awt.Event;

public class WaveformEvent extends AWTEvent {

    static final int POINT_UPDATE     = AWTEvent.RESERVED_ID_MAX + 1;
    static final int MEASURE_UPDATE   = AWTEvent.RESERVED_ID_MAX + 2;
    static final int STATUS_INFO      = AWTEvent.RESERVED_ID_MAX + 3;
    static final int BROADCAST_SCALE  = AWTEvent.RESERVED_ID_MAX + 4;
    static final int COPY_PASTE       = AWTEvent.RESERVED_ID_MAX + 5;
    static final int EVENT_UPDATE     = AWTEvent.RESERVED_ID_MAX + 8;
    static final int PROFILE_UPDATE   = AWTEvent.RESERVED_ID_MAX + 9;
    
    int    signal_idx;
    int    pixel_value;
    double point_x;
    double point_y;
    double delta_x;
    double delta_y;
    String signal_name;
    String status_info;
    int    pixels_x[];
    int    start_pixel_x;
    int    pixels_y[];
    int    start_pixel_y;
    int    pixels_signal[];
    int    pixels_line[] = null;
    float  frames_time[];
    float  x_value = Float.NaN;
    float  time_value = Float.NaN;

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
                          int pixel_value,
                          int signal_idx) 
    {
        super(source, event_id);
        this.signal_idx = signal_idx;
        this.point_x = point_x;
        this.point_y = point_y;
        this.delta_x = delta_x;
        this.delta_y = delta_y;
        this.pixel_value = pixel_value;
    }

    public WaveformEvent (Object source, int pixels_x[], int start_pixel_x, 
                                         int pixels_y[], int start_pixel_y,
                                         int pixels_signal[], float frames_time[]) 
    {
        super(source, PROFILE_UPDATE);
        this.pixels_x = pixels_x;
        this.pixels_y = pixels_y;
        this.pixels_signal = pixels_signal;
        this.frames_time = frames_time;
        this.start_pixel_x = start_pixel_x;
        this.start_pixel_y = start_pixel_y;
    }
    
    public void setPixelsLine(int p_line[])
    {
        pixels_line = p_line;
    }
    
    public void setXValue(float x_value)
    {
        this.x_value = x_value;
    }
    
    public void setTimeValue(float time_value)
    {
        this.time_value = time_value;
    }
    
    
    private String SetStrSize(String s, int size)
    {
	    StringBuffer sb = new StringBuffer(size);
	
	    sb.append(s.substring(0, ((s.length() < size) ? s.length() : size)));

    	if(sb.length() < size)
	    {
	        for(int i = sb.length(); i < size; i++)
		    sb.append(" ");
	    }	
	    return (new String(sb));
    }

    
    public String toString()
    {
        String s = null;
	    int event_id = getID();
	    Waveform w = (Waveform)getSource();
	    	 	     
	    switch(event_id)
	    {
    	    case WaveformEvent.MEASURE_UPDATE:
	            double dx_f;
	         
	            if(Math.abs(delta_x) < 1.e-20)
	                dx_f = 1.e-20;
	            else
	                dx_f = Math.abs(delta_x);
	         
                s = SetStrSize("[" + Waveform.ConvertToString(point_x, false) + ", " 
				        + Waveform.ConvertToString(point_y, false) + "; dx "
				        + Waveform.ConvertToString(delta_x, false) + "; dy "
				        + Waveform.ConvertToString(delta_y, false) + "; 1/dx "
				        + Waveform.ConvertToString(1./dx_f, false) +
				        "]", 80);

	        case WaveformEvent.POINT_UPDATE:
                if(s == null)
                {
	                if(!w.IsImage())
	                {
	                    Float xf = new Float(x_value);
	                    Float tf = new Float(time_value);
	                    Float nan_f = new Float(Float.NaN);
	                    String xt_string = null;
	                    if(!xf.equals(nan_f))
	                        xt_string = ", X = "+ Waveform.ConvertToString(x_value, false);
                        else
	                        if(!tf.equals(nan_f))
	                            xt_string = ", T = "+ Waveform.ConvertToString(time_value, false);
                                
                        if(xt_string == null)                               
	                        s = SetStrSize("[" + Waveform.ConvertToString(point_x, false) + ", " 
				                + Waveform.ConvertToString(point_y, false) + "]", 30);
				        else 
	                        s = SetStrSize("[" + Waveform.ConvertToString(point_x, false) + ", " 
				                + Waveform.ConvertToString(point_y, false) + xt_string + "]", 50);
		            } else 
	                        s = SetStrSize("[" + ((int)point_x) + ", " 
				                       + ((int)point_y) + " : " 
				                       + "(" + ((pixel_value >> 16) & 0xff) + "," 
				                       + ((pixel_value >> 8) & 0xff) + "," 
				                       + (pixel_value & 0xff) + ")" 
				                       + " : " + delta_x + "]", 50);
		        }
	        break;
	    }
	    return s;
    }
}
