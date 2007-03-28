/* $Id$ */
import java.awt.AWTEvent;
import java.awt.Event;
import java.util.*;
import java.text.*;

public class WaveformEvent
    extends AWTEvent
{
    static final int POINT_UPDATE = AWTEvent.RESERVED_ID_MAX + 1;
    static final int MEASURE_UPDATE = AWTEvent.RESERVED_ID_MAX + 2;
    static final int STATUS_INFO = AWTEvent.RESERVED_ID_MAX + 3;
    static final int BROADCAST_SCALE = AWTEvent.RESERVED_ID_MAX + 4;
    static final int COPY_PASTE = AWTEvent.RESERVED_ID_MAX + 5;
    static final int COPY_CUT = AWTEvent.RESERVED_ID_MAX + 6;
    static final int EVENT_UPDATE = AWTEvent.RESERVED_ID_MAX + 7;
    static final int PROFILE_UPDATE = AWTEvent.RESERVED_ID_MAX + 8;
    static final int POINT_IMAGE_UPDATE = AWTEvent.RESERVED_ID_MAX + 9;
    static final int START_UPDATE = AWTEvent.RESERVED_ID_MAX + 10;
    static final int END_UPDATE = AWTEvent.RESERVED_ID_MAX + 11;
    static final int CACHE_DATA = AWTEvent.RESERVED_ID_MAX + 12;

    int frame_type;
    int signal_idx;
    int pixel_value;
    float point_value;
    double point_x;
    double point_y;
    double delta_x;
    double delta_y;
    String name;
    String status_info;
    int pixels_x[];
    float values_x[];
    int start_pixel_x;
    int pixels_y[];
    float values_y[];
    int start_pixel_y;
    int pixels_signal[];
    float values_signal[];
    int pixels_line[] = null;
    float values_line[] = null;
    float frames_time[];
    float x_value = Float.NaN;
    float time_value = Float.NaN;
    float data_value = Float.NaN;
    boolean is_mb2 = false;

    long dateValue;
    boolean showXasDate = false;

    public WaveformEvent(Object source, int event_id, String status_info)
    {
        super(source, event_id);
        this.status_info = status_info;
    }

    public WaveformEvent(Object source, int event_id)
    {
        super(source, event_id);
    }

    public WaveformEvent(Object source, String status_info)
    {
        super(source, STATUS_INFO);
        this.status_info = status_info;
    }

    public WaveformEvent(Object source, int event_id,
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

    public void setPointValue(float val)
    {
        point_value = val;
    }

    public void setDateValue(long date)
    {
        long dayMilliSeconds = 24 * 60 * 60 * 1000;
        dateValue = date - (date % dayMilliSeconds );
        showXasDate = true;
    }

    public float getPointValue()
    {
        return point_value;
    }

    public void setFrameType(int frame_type)
    {
        this.frame_type = frame_type;
    }

    public WaveformEvent(Object source,
                         String name,
                         int pixels_x[], int start_pixel_x,
                         int pixels_y[], int start_pixel_y,
                         int pixels_signal[], float frames_time[])
    {
        super(source, PROFILE_UPDATE);
        this.name = name;
        this.pixels_x = pixels_x;
        this.pixels_y = pixels_y;
        this.pixels_signal = pixels_signal;
        this.frames_time = frames_time;
        this.start_pixel_x = start_pixel_x;
        this.start_pixel_y = start_pixel_y;
    }

    public WaveformEvent(Object source,
                         String name,
                         float values_x[], int start_pixel_x,
                         float values_y[], int start_pixel_y,
                         float values_signal[], float frames_time[])
    {
        super(source, PROFILE_UPDATE);
        this.name = name;
        this.values_x = values_x;
        this.values_y = values_y;
        this.values_signal = values_signal;
        this.frames_time = frames_time;
        this.start_pixel_x = start_pixel_x;
        this.start_pixel_y = start_pixel_y;
    }

    public void setIsMB2(boolean is_mb2)
    {
        this.is_mb2 = is_mb2;
    }

    public void setPixelsLine(int p_line[])
    {
        pixels_line = p_line;
    }

    public void setValuesLine(float v_line[])
    {
        values_line = v_line;
    }

    public void setXValue(float x_value)
    {
        this.x_value = x_value;
    }

    public void setTimeValue(float time_value)
    {
        this.time_value = time_value;
    }

    public void setDataValue(float data_value)
    {
        this.data_value = data_value;
    }

    private String SetStrSize(String s, int size)
    {
        StringBuffer sb = new StringBuffer(size);

        sb.append(s.substring(0, ( (s.length() < size) ? s.length() : size)));

        if (sb.length() < size)
        {
            for (int i = sb.length(); i < size; i++)
                sb.append(" ");
        }
        return (new String(sb));
    }

    public String toString()
    {
        String s = null;
        int event_id = getID();
        Waveform w = (Waveform) getSource();

        switch (event_id)
        {
            case WaveformEvent.MEASURE_UPDATE:
                double dx_f;

                if (Math.abs(delta_x) < 1.e-20)
                    dx_f = 1.e-20;
                else
                    dx_f = Math.abs(delta_x);

                if(showXasDate)
                {
                    DateFormat format = new SimpleDateFormat("d-MMM-yyyy HH:mm:ss");
                    DateFormat format1 = new SimpleDateFormat("HHH:mm:ss");
                    format.setTimeZone(new SimpleTimeZone(0, "GMT") );
                    format1.setTimeZone(new SimpleTimeZone(0, "GMT"));
                    Date date = new Date();
                    date.setTime(dateValue + (long)point_x);
                    Date date1 = new Date();
                    date1.setTime((long)delta_x);

                    s = SetStrSize("[" +  format.format(date).toString() +
                                    ", "
                                    + Waveform.ConvertToString(point_y, false) +
                                    "; dx "
                                    + format1.format(date1).toString() +
                                    "; dy "
                                    + Waveform.ConvertToString(delta_y, false) +
                                    "]", 80);

                }
                else
                {
                    s = SetStrSize("[" + Waveform.ConvertToString(point_x, false) +
                                   ", "
                                   + Waveform.ConvertToString(point_y, false) +
                                   "; dx "
                                   + Waveform.ConvertToString(delta_x, false) +
                                   "; dy "
                                   + Waveform.ConvertToString(delta_y, false) +
                                   "; 1/dx "
                                   + Waveform.ConvertToString(1. / dx_f, false) +
                                   "]", 80);
                }

            case WaveformEvent.POINT_UPDATE:
            case WaveformEvent.POINT_IMAGE_UPDATE:
                if (s == null)
                {
                    if (!w.IsImage())
                    {
                        Float xf = new Float(x_value);
                        Float tf = new Float(time_value);
                        Float df = new Float(data_value);
                        Float nan_f = new Float(Float.NaN);
                        String xt_string = null;
                        if (!xf.equals(nan_f))
                            xt_string = ", Y = " +
                                Waveform.ConvertToString(x_value, false);
                        else
                        if (!tf.equals(nan_f))
                            if(showXasDate)
                            {
                                DateFormat format = new SimpleDateFormat("d-MMM-yyyy HH:mm:ss");
                                format.setTimeZone(new SimpleTimeZone(0, "GMT"));
                                Date date = new Date();
                                date.setTime(dateValue + (long)time_value);
                                xt_string = ", T = " + format.format(date).toString();
                                showXasDate = false;
                            }
                            else
                                xt_string = ", X = " + Waveform.ConvertToString(time_value, false);
                        else
                        if (!df.equals(nan_f))
                            xt_string = ", Z = " +
                                Waveform.ConvertToString(data_value, false);


                        String x_string = null;
                        int string_size = 30;
                        if(showXasDate)
                        {
                            DateFormat format = new SimpleDateFormat("d-MMM-yyyy HH:mm:ss");
                            format.setTimeZone(new SimpleTimeZone(0, "GMT"));
                            Date date = new Date();
                            date.setTime(dateValue + (long)point_x);
                            x_string = format.format(date).toString();
                            string_size = 35;
                        }
                        else
                           x_string = ""+new Float(point_x);

                        if (xt_string == null)
                            s = SetStrSize("[" + x_string + ", "
                                           + new Float(point_y) + "]", string_size);
                        else
                            s = SetStrSize("[" + x_string + ", "
                                           + new Float(point_y) + xt_string +
                                           "]", string_size + 20);
                    }
                    else
                    if (frame_type == FrameData.BITMAP_IMAGE_32 ||
                        frame_type == FrameData.BITMAP_IMAGE_16)
                    {
                        s = SetStrSize("[" + ( (int) point_x) + ", "
                                       + ( (int) point_y) + " : "
                                       + "(" + new Float(point_value) + ")"
                                       + " : " + delta_x + "]", 50);
                    }
                    else
                    {
                        s = SetStrSize("[" + ( (int) point_x) + ", "
                                       + ( (int) point_y) + " : "
                                       + "(" + ( (pixel_value >> 16) & 0xff) +
                                       ","
                                       + ( (pixel_value >> 8) & 0xff) + ","
                                       + (pixel_value & 0xff) + ")"
                                       + " : " + delta_x + "]", 50);
                    }
                }
                break;
        }
        return (s == null ? "" : s);
    }
}
