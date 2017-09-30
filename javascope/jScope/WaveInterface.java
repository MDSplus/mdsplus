package jScope;

import java.io.IOException;
import java.io.PrintWriter;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

public class WaveInterface
{
    static SignalBox sig_box = new SignalBox();

    static final int MAX_NUM_SHOT = 30;
    public Waveform wave;
    public int num_waves;
    public boolean x_log, y_log;
    public String in_label[], in_x[], in_y[], in_up_err[], in_low_err[];

    //Prameter used to evaluate waveform
    public String in_xmin, in_xmax, in_ymax, in_ymin, in_timemax, in_timemin;
    public String in_title, in_xlabel, in_ylabel;
    public String in_def_node;
    public boolean in_upd_limits = true;
    public String experiment;
    public int in_grid_mode;
    public int height;
    
    public boolean isContinuousUpdate = false;

    public String in_shot;
    public int num_shot = 1;
    private boolean modified = true;

    //Used by GetShotArray methods to define
    //if required shots must be evaluate
    //private String shot_str_eval = null;
    //private long shot_list[] = null;

    boolean reversed = false;
    boolean show_legend = false;
    double legend_x, legend_y;

    public int markers_step[];
    public int markers[];
    public int colors_idx[];
    public boolean interpolates[];
    public int mode2D[];
    public int mode1D[];
    public long shots[];
    public String error;
    private String curr_error;
    public String provider;
    public String w_error[];
    public Signal signals[];
    public double xmax, xmin, ymax, ymin, timemax, timemin;
    public String title, xlabel, ylabel, zlabel;
    protected DataProvider dp;

// Used for asynchronous Update
    public boolean asynch_update = true;
    Signal wave_signals[];
    protected boolean evaluated[];


    protected boolean is_image = false;
    boolean keep_ratio = true;
    boolean horizontal_flip = false;
    boolean vertical_flip = false;
    int     signal_select = -1;
    private Frames frames;

    protected boolean is_async_update = false;

    static public boolean auto_color_on_expr = false;

    //True when a signal is added
    protected boolean add_signal = false;
    protected boolean is_signal_added = false;

    boolean cache_enabled = false;
    static boolean brief_error = true;

    ColorMap colorMap = new ColorMap();

    boolean xLimitsLong;
    long xminLong = 0;
    long xmaxLong = 0;


    public WaveInterface()
    {
        CreateWaveInterface(null, null);
    }

    public WaveInterface(Waveform wave)
    {
        CreateWaveInterface(wave, null);
    }

    public WaveInterface(DataProvider dp)
    {
        CreateWaveInterface(null, dp);
    }

    public WaveInterface(Waveform wave, DataProvider dp)
    {
        CreateWaveInterface(wave, dp);
    }

    public void setExperiment(String experiment)
    {
        this.experiment = experiment;
    }
    public void Erase()
    {
        num_waves = 0;
        in_label = null;
        in_x = null;
        in_y = null;
        in_up_err = null;
        in_low_err = null;
        in_xmin = null;
        in_xmax = null;
        in_ymax = null;
        in_ymin = null;
        in_timemax = null;
        in_timemin = null;
        in_title = null;
        in_xlabel = null;
        in_ylabel = null;
        experiment = null;
        in_shot = null;
        num_shot = 1;
        modified = true;
        markers_step = null;
        markers = null;
        colors_idx = null;
        interpolates = null;
        mode2D = null;
        mode1D = null;
        shots = null;
        error = null;
        curr_error = null;
        w_error = null;
        signals = null;
        title = null;
        xlabel = null;
        ylabel = null;
        is_image = false;
        keep_ratio = true;
        horizontal_flip = false;
        vertical_flip = false;
        frames = null;
        show_legend = false;

        wave_signals = null;
        evaluated = null;
    }

    public void SetAsImage(boolean is_image)
    {
        this.is_image = is_image;
    }

    public void EnableCache(boolean state)
    {
    }

    static public void FreeCache()
    {
    }

    static void WriteLine(PrintWriter out, String prompt, String value)
    {
        if (value != null && value.length() != 0)
        {
            out.println(prompt + value);
        }
    }

    static String TrimString(String s)
    {
        String s_new = new String();
        int new_pos = 0, old_pos = 0;
        while ( (new_pos = s.indexOf(" ", old_pos)) != -1)
        {
            s_new = s_new.concat(s.substring(old_pos, new_pos));
            old_pos = new_pos + " ".length();
        }
        s_new = s_new.concat(s.substring(old_pos, s.length()));
        return s_new;
    }

    static String RemoveNewLineCode(String s)
    {
        String y_new = new String();
        int new_pos = 0, old_pos = 0;

        while ( (new_pos = s.indexOf("|||", old_pos)) != -1)
        {
            y_new = y_new.concat(s.substring(old_pos, new_pos));
            old_pos = new_pos + "|||".length();
            y_new += '\n';
        }
        y_new = y_new.concat(s.substring(old_pos, s.length()));
        return y_new;
    }

    static String AddNewLineCode(String s)
    {
        String s_new = new String();
        int new_pos = 0, old_pos = 0;

        if (s == null)
            return null;
        while ( (new_pos = s.indexOf("\n", old_pos)) != -1)
        {
            s_new = s_new.concat(s.substring(old_pos, new_pos));
            old_pos = new_pos + "\n".length();
            s_new += "|||";
        }
        s_new = s_new.concat(s.substring(old_pos, s.length()));
        return s_new;
    }

    private void CreateWaveInterface(Waveform wave, DataProvider dp)
    {
        this.wave = wave;

        this.dp = dp;
        if (dp == null)
        experiment = null;
        shots = null;
        in_xmin = in_xmax = in_ymin = in_ymax = in_title = null;
        in_xlabel = in_ylabel = in_timemax = in_timemin = null;
        markers = null;
        interpolates = null;
        mode2D = null;
        mode1D = null;
        x_log = y_log = false;
        in_upd_limits = true;
        show_legend = false;
        reversed = false;
    }

    public void SetDataProvider(DataProvider _dp)
    {
        dp = _dp;
        error = null;
        curr_error = null;
        w_error = null;
        signals = null;
        modified = true;

        //When change data provide
        //shot variable is reset to
        //assure shot avaluation
        //shot_str_eval = null;
        //shot_list = null;
    }

    public DataProvider getDataProvider()
    {
        return dp;
    }

    public void setSignalState(String name, boolean state)
    {

        if (num_waves != 0)
        {
            int ns = (num_shot == 0) ? 1 : num_shot;
            for (int i = 0; i < num_waves; i++)
                if (name.equals(in_y[i]) || name.equals(in_label[i]))
                {
                    for (int j = i; j < i + ns; j++)
                    {
                        interpolates[j] = state;
                        markers[i] = Signal.NONE;
                    }
                    return;
                }
        }
    }

    public void setColorMap(ColorMap colorMap)
    {
        this.colorMap = colorMap;
    }

    public ColorMap getColorMap()
    {
        return colorMap;
    }


    public void ShowLegend(boolean state)
    {
        show_legend = state;
    }

    public void setModified(boolean state)
    {
        modified = state;
    }

    public boolean getModified()
    {
        return modified;
    }

    
    public void SetLegendPosition(double x, double y)
    {
        legend_x = x;
        legend_y = y;
        show_legend = true;
    }

    private String GetFirstLine(String str)
    {
        int idx = str.indexOf("\n");
        if (idx != -1)
            return str.substring(0, idx);
        else
            return str;
    }

    public int GetNumEvaluatedSignal()
    {
        if (signals == null)
            return 0;
        int i, n = 0;
        for (i = 0; i < signals.length; i++)
            if (signals[i] != null)
                n++;
        return n;
    }

    public boolean IsSignalAdded()
    {
        return is_signal_added;
    }

    public void SetIsSignalAdded(boolean is_signal_added)
    {
        this.is_signal_added = is_signal_added;
    }

    public String getErrorTitle()
    {
        return getErrorTitle(brief_error);
    }

    public String getErrorTitle(boolean brief)
    {
        int n_error = 0;
        String er = error;

        if (num_waves == 0 || (is_image && frames == null))
        {
            if (error != null)
                if (brief)
                    er = GetFirstLine(new String(error));
            return er;
        }

        for (int ii = 0; ii < num_waves; ii++)
        {
            if (w_error != null && w_error[ii] != null)
                n_error++;
        }

        if (error == null && n_error > 1 && n_error == num_waves)
        {
            er = "Evaluation error on all signals";
        }
        else
        {
            if (error != null)
                er = error;
            else
            {
                if (n_error == 1 && num_waves == 1)
                {
                    er = w_error[0];
                }
                else
                if (n_error > 0)
                    er = "< Evaluation error on " + n_error + " signal" +
                        (n_error > 1 ? "s" : "") + " >";
            }
        }

        if (er != null && brief)
        {
            int idx = (er.indexOf('\n') == -1 ? er.length() : er.indexOf('\n'));
            er = er.substring(0, idx);
        }
        return er;
    }

    public boolean GetSignalState(int i)
    {
        boolean state = false;
        //int idx = i * (num_shot > 0 ? num_shot : 1);
        if (i < num_waves)
            state = (interpolates[i] || (markers[i] != Signal.NONE));
        return state;
    }

    public boolean[] GetSignalsState()
    {
        boolean state[] = null;

        if (num_waves != 0)
        {
            int ns = (num_shot > 0 ? num_shot : 1);
            state = new boolean[num_waves / ns];
            for (int i = 0, j = 0; i < num_waves; i += ns)
                state[j++] = (interpolates[i] || (markers[i] != Signal.NONE));
        }
        return state;
    }

    public String[] GetSignalsName()
    {
        String name[] = null, s;

        if (num_waves != 0)
        {
            int ns = (num_shot > 0 ? num_shot : 1);
            name = new String[num_waves / ns];
            for (int i = 0, j = 0; i < num_waves; i += ns)
            {
                s = (in_label[i] != null && in_label[i].length() != 0) ?
                    in_label[i] : in_y[i];
                name[j++] = s;
            }
        }
        return name;
    }

    
    public Frames getFrames()
    {
        return frames;
    }
    
    public void setFrames(Frames f)
    {
        frames = f;
    }
    
    public void AddFrames(String frames)
    {
        AddFrames(frames, null);
    }

    public void AddFrames(String frames, String frames_time)
    {
        SetAsImage(true);
        in_x = new String[1];
        in_y = new String[1];
        in_x[0] = frames_time;
        in_y[0] = frames;
    }

    public boolean AddSignal(String y_expr)
    {
        return AddSignal("", y_expr);
    }

    public boolean AddSignal(String x_expr, String y_expr)
    {
        String x[] = new String[1];
        String y[] = new String[1];
        x[0] = x_expr;
        y[0] = y_expr;
        return AddSignals(x, y);
    }

    public boolean AddSignals(String x_expr[], String y_expr[])
    {

        if (x_expr.length != y_expr.length || x_expr.length == 0)
            return false;

        int new_num_waves;
        int num_sig = x_expr.length;
        boolean is_new[] = null;

        if (num_waves != 0)
        {
            is_new = new boolean[x_expr.length];
            for (int j = 0; j < x_expr.length; j++)
            {
                is_new[j] = true;
                for (int i = 0; i < num_waves; i++)
                {
                    if (y_expr[j].equals(in_y[i]) &&
                        (in_x[i] != null && x_expr[j].equals(in_x[i])))
                    {
                        /*
                             if(evaluated != null && evaluated[i])
                            return true;
                                                 else
                            return false;
                         */
                        is_new[j] = false;
                        num_sig--;
                    }
                }
            }

            if (num_sig == 0)
                return true;

            new_num_waves = num_waves +
                (num_shot != 0 ? num_shot : 1) * num_sig;
        }
        else
            new_num_waves = x_expr.length;

        String new_in_label[] = new String[new_num_waves];
        String new_in_x[] = new String[new_num_waves];
        String new_in_y[] = new String[new_num_waves];
        String new_in_up_err[] = new String[new_num_waves];
        String new_in_low_err[] = new String[new_num_waves];
        int new_markers[] = new int[new_num_waves];
        int new_markers_step[] = new int[new_num_waves];
        int new_colors_idx[] = new int[new_num_waves];
        boolean new_interpolates[] = new boolean[new_num_waves];
        int new_mode2D[] = new int[new_num_waves];
        int new_mode1D[] = new int[new_num_waves];
        long new_shots[] = null;
        if (shots != null)
            new_shots = new long[new_num_waves];

        boolean new_evaluated[] = new boolean[new_num_waves];
        Signal new_signals[] = new Signal[new_num_waves];
        String new_w_error[] = new String[new_num_waves];

        for (int i = 0; i < num_waves; i++)
        {
            new_in_label[i] = in_label[i];
            new_in_x[i] = in_x[i];
            new_in_y[i] = in_y[i];
            new_in_up_err[i] = in_up_err[i];
            new_in_low_err[i] = in_low_err[i];
            new_markers[i] = markers[i];
            new_markers_step[i] = markers_step[i];
            new_colors_idx[i] = colors_idx[i];
            new_interpolates[i] = interpolates[i];
            new_mode2D[i] = mode2D[i];
            new_mode1D[i] = mode1D[i];
            if (shots != null)
                new_shots[i] = shots[i];
            if (evaluated != null)
                new_evaluated[i] = evaluated[i];
            else
                new_evaluated[i] = false;
            if (signals != null)
                new_signals[i] = signals[i];
            if (w_error != null)
                new_w_error[i] = w_error[i];
        }

        for (int i = 0, k = num_waves; i < x_expr.length; i++)
        {
            if (is_new != null && !is_new[i])
                continue;
            for (int j = 0; j < num_shot; j++)
            {
                new_in_label[k] = "";
                new_in_x[k] = new String(x_expr[i]);
                new_in_y[k] = new String(y_expr[i]);
                new_in_up_err[k] = "";
                new_in_low_err[k] = "";
                new_markers[k] = 0;
                new_markers_step[k] = 1;

                if (auto_color_on_expr)
                    new_colors_idx[k] = (k - j) % Waveform.colors.length;
                else
                    new_colors_idx[k] = j % Waveform.colors.length;

                new_interpolates[k] = true;
                new_evaluated[k] = false;
                new_mode2D[k] = Signal.MODE_XZ;
                new_mode1D[k] = Signal.MODE_LINE;
                if (shots != null && shots.length != 0 && num_shot > 0)
                    new_shots[k] = shots[j];
                k++;
            }
        }

        in_label = new_in_label;
        in_x = new_in_x;
        in_y = new_in_y;
        in_up_err = new_in_up_err;
        in_low_err = new_in_low_err;
        markers = new_markers;
        markers_step = new_markers_step;
        colors_idx = new_colors_idx;
        interpolates = new_interpolates;
        mode2D = new_mode2D;
        mode1D = new_mode1D;
        shots = new_shots;
        num_waves = new_num_waves;
        evaluated = new_evaluated;
        signals = new_signals;
        w_error = new_w_error;
        add_signal = true;
        return true;
    }

    public boolean isAddSignal()
    {
        return add_signal;
    }

    public void setAddSignal(boolean add_signal)
    {
        this.add_signal = add_signal;
    }

    public boolean UpdateShot(long curr_shots[]) throws IOException
    {
        int curr_num_shot;

        if (curr_shots == null)
        {
            curr_num_shot = 1;
            shots = null;
            if (num_shot == 0)
                return false;
        }
        else
            curr_num_shot = curr_shots.length;


        int num_signal;
        int num_expr;
        if (num_shot == 0)
        {
            num_signal = num_waves * curr_num_shot;
            num_expr = num_waves;
        }
        else
        {
            num_signal = num_waves / num_shot * curr_num_shot;
            num_expr = num_waves / num_shot;
        }


        if (is_image)
        {
            modified = true;
            shots = curr_shots;
            return true;
        }

        if (num_signal == 0)
            return false;

        modified = true;

        String[] in_label = new String[num_signal];
        String[] in_x = new String[num_signal];
        String[] in_y = new String[num_signal];
        String[] in_up_err = new String[num_signal];
        String[] in_low_err = new String[num_signal];
        int[] markers = new int[num_signal];
        int[] markers_step = new int[num_signal];
        int[] colors_idx = new int[num_signal];
        boolean[] interpolates = new boolean[num_signal];
        int[] mode2D = new int[num_signal];
        int[] mode1D = new int[num_signal];
        long[] shots = null;
        if (curr_shots != null)
            shots = new long[num_signal];

        int sig_idx = (this.num_shot == 0) ? 1 : this.num_shot;
        for (int i = 0, k = 0; i < num_expr; i++)
        {
            for (int j = 0; j < curr_num_shot; j++, k++)
            {
                in_label[k] = this.in_label[i * sig_idx];
                in_x[k] = this.in_x[i * sig_idx];
                in_y[k] = this.in_y[i * sig_idx];
                if (j < this.num_shot)
                {
                    markers[k] = this.markers[i * this.num_shot + j];
                    markers_step[k] = this.markers_step[i * this.num_shot + j];
                    interpolates[k] = this.interpolates[i * this.num_shot + j];
                    mode2D[k] = this.mode2D[i * this.num_shot + j];
                    mode1D[k] = this.mode1D[i * this.num_shot + j];
                    if (curr_shots != null)
                        shots[k] = curr_shots[j];
                    in_up_err[k] = this.in_up_err[i * this.num_shot + j];
                    in_low_err[k] = this.in_low_err[i * this.num_shot + j];
                    colors_idx[k] = this.colors_idx[i * this.num_shot + j];
                }
                else
                {
                    markers[k] = this.markers[i * this.num_shot];
                    markers_step[k] = this.markers_step[i * this.num_shot];
                    interpolates[k] = this.interpolates[i * this.num_shot];
                    mode2D[k] = this.mode2D[i * this.num_shot];
                    mode1D[k] = this.mode1D[i * this.num_shot];
                    in_up_err[k] = this.in_up_err[i * this.num_shot];
                    in_low_err[k] = this.in_low_err[i * this.num_shot];

                    if (auto_color_on_expr)
                        colors_idx[k] = i % Waveform.colors.length; //this.colors_idx[i * this.num_shot];
                    else
                        colors_idx[k] = j % Waveform.colors.length;

                    if (curr_shots != null)
                        shots[k] = curr_shots[j];
                }
            }
        }

        this.in_label = in_label;
        this.in_x = in_x;
        this.in_y = in_y;
        this.in_up_err = in_up_err;
        this.in_low_err = in_low_err;
        this.markers = markers;
        this.markers_step = markers_step;
        this.colors_idx = colors_idx;
        this.interpolates = interpolates;
        this.mode2D = mode2D;
        this.mode1D = mode1D;
        this.shots = shots;
        if (shots != null)
            this.num_shot = curr_num_shot;
        else
            this.num_shot = 1;
        num_waves = num_signal;

        return true;
    }

    public synchronized int StartEvaluate() throws IOException
    {
        error = null;

        if (modified)
            evaluated = null;

        if ( (in_y == null || in_x == null))
        {
            error = "Missing Y or X values";
            signals = null;
            return 0;
        }

        if (shots == null && ! ( experiment == null || experiment.trim().length() == 0) )
        {
            error = "Missing shot value";
            signals = null;

        }

        if (shots != null && ( experiment == null || experiment.trim().length() == 0) )
        {
            error = "Missing experiment name";
            signals = null;
        }


        num_waves = in_y.length;

        if (modified)
        {
            if (!is_image)
                signals = new Signal[num_waves];

            evaluated = new boolean[num_waves];
            w_error = new String[num_waves];
        }

        if (in_x != null && num_waves != in_x.length)
        {
            error = "X values are different from Y values";
            return 0;
        }

        if (shots != null && shots.length > 0)
        {
            int i = 0;
            do
            {
                dp.Update(experiment, shots[i]);
                i++;
            }
            while (i < shots.length && dp.ErrorString() != null);
        }
        else
            dp.Update(null, 0);

        if (dp.ErrorString() != null)
        {
            error = dp.ErrorString();
            return 0;
        }

        //Compute title
        if (in_title != null && (in_title.trim()).length() != 0)
        {
            title = dp.GetString(in_title);
            if (title == null)
            {
                error = dp.ErrorString();
                return 0;
            }
        }

        //compute limits
        if (in_xmin != null &&
            (in_xmin.trim()).length() != 0 &&
            in_upd_limits)
        {
            xmin = dp.GetFloat(in_xmin);
            if (dp.ErrorString() != null)
            {
                error = dp.ErrorString();
                return 0;
            }
            long timeLong = getDate(in_xmin);
            if(timeLong != -1)
            {
                xLimitsLong = true;
                xminLong = timeLong;
            }
        }
        else
            xmin = (!is_image) ? -Double.MAX_VALUE : -1;

        if (in_xmax != null &&
            (in_xmax.trim()).length() != 0 &&
            in_upd_limits)
        {
            xmax = dp.GetFloat(in_xmax);
            if (dp.ErrorString() != null)
            {
                error = dp.ErrorString();
                return 0;
            }
            long timeLong = getDate(in_xmax);
            if(timeLong != -1)
            {
                xLimitsLong = true;
                xmaxLong = timeLong;
            }
        }
        else
            xmax = (!is_image) ? Double.MAX_VALUE : -1;

        if (in_ymax != null &&
            (in_ymax.trim()).length() != 0 &&
            in_upd_limits)
        {
            ymax = dp.GetFloat(in_ymax);
            if (dp.ErrorString() != null)
            {
                error = dp.ErrorString();
                return 0;
            }
        }
        else
            ymax = (!is_image) ? Double.MAX_VALUE : -1;

        if (in_ymin != null &&
            (in_ymin.trim()).length() != 0 &&
            in_upd_limits)
        {
            ymin = dp.GetFloat(in_ymin);
            if (dp.ErrorString() != null)
            {
                error = dp.ErrorString();
                return 0;
            }
        }
        else
            ymin = (!is_image) ? -Double.MAX_VALUE : -1;

        if (is_image)
        {
            if (in_timemax != null && (in_timemax.trim()).length() != 0)
            {
                timemax = dp.GetFloat(in_timemax);
                if (dp.ErrorString() != null)
                {
                    error = dp.ErrorString();
                    return 0;
                }
            }
            else
                timemax = Double.MAX_VALUE;

            if (in_timemin != null && (in_timemin.trim()).length() != 0)
            {
                timemin = dp.GetFloat(in_timemin);
                if (dp.ErrorString() != null)
                {
                    error = dp.ErrorString();
                    return 0;
                }
            }
            else
                timemin = -Double.MAX_VALUE;
        }

        //Compute x label, y_label

        if (in_xlabel != null && (in_xlabel.trim()).length() != 0)
        {
            xlabel = dp.GetString(in_xlabel);
            if (xlabel == null)
            {
                error = dp.ErrorString();
                return 0;
            }
        }
        if (in_ylabel != null && (in_ylabel.trim()).length() != 0)
        {
            ylabel = dp.GetString(in_ylabel);
            if (ylabel == null)
            {
                error = dp.ErrorString();
                return 0;
            }
        }
        if (xmin > xmax)
            xmin = xmax;
        if (ymin > ymax)
            ymin = ymax;

        return 1;
    }

    public synchronized void EvaluateShot(long shot) throws Exception
    {
        int curr_wave;

        if (is_image)
            return;

        //dp.enableAsyncUpdate(false);

        for (curr_wave = 0; curr_wave < num_waves; curr_wave++)
        {
            if ( ( shot == 0 ) || ( shots[curr_wave] == shot && !evaluated[curr_wave] &&
                (interpolates[curr_wave] ||
                 markers[curr_wave] != Signal.NONE)) )
            {
                w_error[curr_wave] = null;
                signals[curr_wave] = GetSignal(curr_wave, -Double.MAX_VALUE, Double.MAX_VALUE);
                evaluated[curr_wave] = true;
                if (signals[curr_wave] == null)
                {
                    w_error[curr_wave] = curr_error;
                    evaluated[curr_wave] = false;
                }
                else
                {
                    sig_box.AddSignal(in_x[curr_wave], in_y[curr_wave]);
                    setLimits(signals[curr_wave]);
                }
            }
        }
        //dp.enableAsyncUpdate(true);
   }

    public void setLimits()
    {
        try
        {
            for (int i = 0; i < signals.length; i++)
                if (signals[i] != null)
                    setLimits(signals[i]);
        }
        catch (Exception e)
        {}
    }

    public void setLimits(Signal s) throws Exception
    {
        s.setXLimits(xmin, xmax, Signal.AT_CREATION);
        s.setYmin(ymin, Signal.AT_CREATION);
        s.setYmax(ymax, Signal.AT_CREATION);
    }

    public boolean allEvaluated()
    {
        if (evaluated == null)
            return false;
        for (int curr_wave = 0; curr_wave < num_waves; curr_wave++)
            if (!evaluated[curr_wave])
            {
                modified = true;
                return false;
            }
        modified = false;
        return true;
    }

    public synchronized void EvaluateOthers() throws Exception
    {
        int curr_wave;

        if (is_image)
        {
            if(!evaluated[0])
            {
                InitializeFrames();
                if (frames != null)
                    frames.SetViewRect( (int) xmin, (int) ymin, (int) xmax,
                                   (int) ymax);
                error = curr_error;
            }
            return;
        }

        if (evaluated == null)
        {
            signals = null;
            return;
        }

        for (curr_wave = 0; curr_wave < num_waves; curr_wave++)
        {
            if (!evaluated[curr_wave] &&
                ! (!interpolates[curr_wave] &&
                   this.markers[curr_wave] == Signal.NONE))
            {
                w_error[curr_wave] = null;
                signals[curr_wave] = GetSignal(curr_wave, xmin, xmax);
                evaluated[curr_wave] = true;
                if (signals[curr_wave] == null) {
                    w_error[curr_wave] = curr_error;
                    evaluated[curr_wave] = false;
                } else {
                    sig_box.AddSignal(in_x[curr_wave], in_y[curr_wave]);
                    setLimits(signals[curr_wave]);
                }
            }
        }
        modified = false;
    }

    private void CreateNewFramesClass(int image_type) throws IOException
    {
        if (image_type == FrameData.JAI_IMAGE) {
            try {
                Class cl = Class.forName("jScope.FrameJAI");
                frames = (Frames) cl.newInstance();
            } catch (Exception e) {
                throw (new IOException(
                    "Java Advanced Imaging must be installed to show this type of image"));
            }
        } else {
            frames = new Frames();
            frames.setColorMap(colorMap);
        }
    }

    private void InitializeFrames()
    {
        curr_error = null;
        WaveformEvent we;
        int mode = this.wave.GetMode();

        this.wave.SetMode(Waveform.MODE_WAIT);

        if (in_y[0] == null)
        {
            curr_error = "Missing Y value";
            return;
        }

        if (shots != null && shots.length != 0)
            dp.Update(experiment, shots[0]);
        else
            dp.Update(null, 0);

        try
        {
            we = new WaveformEvent(wave, "Loading single or multi frame image");
            wave.dispatchWaveformEvent(we);
            FrameData fd = dp.GetFrameData(in_y[0], in_x[0], (float)timemin, (float)timemax);
            if (fd != null)
            {
                CreateNewFramesClass(fd.GetFrameType());
                /*
                frames.setHorizontalFlip(horizontal_flip);
                frames.setVerticalFlip(vertical_flip);
                */
                frames.SetFrameData(fd);
                if (in_label != null && in_label[0] != null &&
                    in_label[0].length() != 0)
                    frames.setName(in_label[0]);
                else
                    frames.setName(in_y[0]);
                evaluated[0] = true;
            }
            else
            {
                frames = null;
                curr_error = dp.ErrorString();
                evaluated[0] = false;
            }
            //frames.WaitLoadFrame();

            this.wave.SetMode(mode);

        }
        catch (Throwable e)
        {
            //e.printStackTrace();
            this.wave.SetMode(mode);
            frames = null;
            curr_error = " Load Frames error " + e.getMessage();
        }
    }

    public void setShotArray(String in_shot) throws IOException
    {
        this.in_shot = in_shot;
        long curr_shots[] = GetShotArray(in_shot);
        UpdateShot(curr_shots);
    }

    public long[] GetShotArray(String in_shots) throws IOException
    {
        long curr_shots[] = GetShotArray(in_shots, experiment, dp);
        return curr_shots;
    }

    static String processShotExpression(String shotExpr, String exp)
    {
        
        String outStr = "";
        int idx = 0;
        int prevIdx = 0;
        
        shotExpr = shotExpr.trim();
        
        if( exp == null || exp.length() == 0 )
            return shotExpr;
             
        while( (idx = shotExpr.indexOf('0', prevIdx) ) != -1 )
        {
            
            if( ( idx > 0 && Character.isLetterOrDigit( shotExpr.charAt( idx - 1 )) ) ||
                ( idx < ( shotExpr.length() - 1 ) && Character.isLetterOrDigit( shotExpr.charAt( idx + 1 )) ) )
            {
                outStr += shotExpr.substring(prevIdx, idx+1);
            }
            else
            {
                outStr += shotExpr.substring(prevIdx, idx) + "current_shot(\""+exp+"\")";
            }
            prevIdx = idx + 1;                
        }
        if( outStr.length() == 0 )
            return shotExpr;
        else
            outStr += ( prevIdx < shotExpr.length() ? shotExpr.substring(prevIdx, shotExpr.length()) :"" );
        return outStr;
    }
    
    static public long[] GetShotArray(String in_shots, String exp, DataProvider dp) throws
        IOException
    {
        long shot_list[] = null;
        String error;

        if (in_shots == null || in_shots.trim().length() == 0 || dp == null)
            return null;

        String shotExpr = in_shots;
        if( exp != null )
            shotExpr = processShotExpression(in_shots, exp);
        shot_list = dp.GetShots( shotExpr );
        if (shot_list == null || shot_list.length == 0 ||
            shot_list.length > MAX_NUM_SHOT)
        {
            if (shot_list != null && shot_list.length > MAX_NUM_SHOT)
                error = "Too many shots. Max shot list elements " +
                    MAX_NUM_SHOT + "\n";
            else
            {
                if (dp.ErrorString() != null)
                {
                    error = dp.ErrorString();
                    if (error.indexOf("_jScopeMainShots") != -1)
                        error = "Undefined main shot value";

                }
                else
                    error = "Shot syntax error\n";
            }
            throw (new IOException(error));
        }
        return shot_list;
    }

    private Signal GetSignal(int curr_wave, double xmin, double xmax) throws
        Exception
    {
        Signal out_signal = null;
        int mode = this.wave.GetMode();

        try
        {
            this.wave.SetMode(Waveform.MODE_WAIT);

            if (in_y[curr_wave] == null)
            {
                curr_error = "Missing Y value";
                this.wave.SetMode(mode);
                return null;
            }

            synchronized (dp)
            {
                out_signal = GetSignalFromProvider(curr_wave, xmin, xmax);
            }

            if (out_signal != null)
            {
                if (xmin > xmax)
                    xmin = xmax;
                if (ymin > ymax)
                    ymin = ymax;

                out_signal.setXLimits(xmin, xmax,
                                       Signal.AT_CREATION | Signal.FIXED_LIMIT);
 
                if (in_ymax != null && (in_ymax.trim()).length() != 0 &&
                    in_upd_limits)
                    out_signal.setYmax(ymax,
                                       Signal.AT_CREATION | Signal.FIXED_LIMIT);

                if (in_ymin != null && (in_ymin.trim()).length() != 0 &&
                    in_upd_limits)
                    out_signal.setYmin(ymin,
                                       Signal.AT_CREATION | Signal.FIXED_LIMIT);

             }
            this.wave.SetMode(mode);
       }
        catch(IOException exc)
        {
            this.wave.SetMode(mode);
            // 10-2-2009 throw(exc);
        }
        return out_signal;
    }

    private Signal GetSignalFromProvider(int curr_wave, double xmin, double xmax) throws
        IOException
    {
        WaveData up_err = null, low_err = null;
        WaveData wd = null;
        WaveData xwd = null;
        
        int xDimension = 1;
        int yDimension = 1;
        Signal out_signal;
        String xlabel = null, ylabel = null, title = null;

        if (shots != null && shots.length != 0)
            dp.Update(experiment, shots[curr_wave]);
        else
            dp.Update(null, 0);

        if (dp.ErrorString() != null)
        {
            error = dp.ErrorString();
            return null;
        }
        
        if( in_def_node != null && in_def_node.length() > 0 )
        {
            dp.SetEnvironment("__default_node = " + in_def_node);
        } 
        else 
        {
            dp.SetEnvironment("__default_node = " + experiment + "::TOP");
        }
            
        if (dp.ErrorString() != null)
        {
            error = dp.ErrorString();
            return null;
        }
     
        if ( in_x[curr_wave] != null && (in_x[curr_wave].trim()).length() != 0 )
        {

            wd = dp.GetWaveData(in_y[curr_wave], in_x[curr_wave]);
            if (wd != null)
            {
                xlabel = wd.GetXLabel();
                ylabel = wd.GetYLabel();
            }

            if (wd != null && in_up_err != null &&
                in_up_err[curr_wave] != null &&
                (in_up_err[curr_wave].trim()).length() != 0)
            {
                up_err = dp.GetWaveData(in_up_err[curr_wave]);
            }

            if (in_low_err != null &&
                in_low_err[curr_wave] != null &&
                (in_low_err[curr_wave].trim()).length() != 0)
            {
                low_err = dp.GetWaveData(in_low_err[curr_wave]);
            }
        }
        else // X field not defined
        {
            if (wd == null)
                wd = dp.GetWaveData(in_y[curr_wave]);
/*            
            if (yDimension > 1)
            {
                if (wd == null)
                    wd = dp.GetWaveData(in_y[curr_wave]);
            }
            else
            {
                if(wd == null)
                {
                    wd = dp.GetWaveData(in_y[curr_wave]);
                }
            }
*/ 
            if (yDimension == 1)
            {
                if (in_up_err != null &&
                    in_up_err[curr_wave] != null
                    && (in_up_err[curr_wave].trim()).length() != 0)
                {
                    up_err = dp.GetWaveData(in_up_err[curr_wave]);
                }

                if (in_low_err != null &&
                    in_low_err[curr_wave] != null &&
                    (in_low_err[curr_wave].trim()).length() != 0)
                {
                    low_err = dp.GetWaveData(in_low_err[curr_wave]);
                }
            }
        }
        if(wd == null)
        {
            curr_error = dp.ErrorString();
            return null;
        }
        //Check for bidimensional X axis
        if(in_x[curr_wave] != null)
        {
            xwd = dp.GetWaveData(in_x[curr_wave]);
            if(xwd.getNumDimension() == 1)
                xwd = null; //xwd is different from null ONLY for bidimensional X axis 
        }

        if( xDimension == 1)
        {
            if(xLimitsLong)
                out_signal = new Signal(wd, xwd, xminLong, xmaxLong, low_err, up_err);
            else
                out_signal = new Signal(wd, xwd, xmin, xmax, low_err, up_err);
        }
        else
        {
           if(xLimitsLong)
                out_signal = new Signal(wd, xwd, xminLong, xmaxLong);
            else
                out_signal = new Signal(wd, xwd, xmin, xmax);
        }

        if(yDimension > 1)
            out_signal.setMode2D(mode2D[curr_wave]);
        else
            out_signal.setMode1D(mode1D[curr_wave]);
 
        if (wd != null)
        {
            try {
                title = wd.GetTitle();
            }catch(Exception exc){}
        }

        if (up_err != null && low_err != null)
            out_signal.AddAsymError(up_err, low_err);
        else
        if (up_err != null)
            out_signal.AddError(up_err);

        if (wd != null)
        {
            try {
                xlabel = wd.GetXLabel();
                ylabel = wd.GetYLabel();
            }catch(Exception exc){}
        }
        out_signal.setLabels(title, xlabel, ylabel, null);
        return out_signal;
    }
    //Try to convert the passed string to a date. Return the converted time in long format if it succeeds, -1 otherwise
    long getDate(String inVal)
    {
         try {
            Calendar cal = Calendar.getInstance();
            //cal.setTimeZone(TimeZone.getTimeZone("GMT+00"));
//            DateFormat df = new SimpleDateFormat("d-MMM-yyyy HH:mm z");
            DateFormat df = new SimpleDateFormat("d-MMM-yyyy HH:mm");
//            Date date = df.parse(inVal + " GMT");
            Date date = df.parse(inVal);
            cal.setTime(date);
            long javaTime = cal.getTime().getTime();
            return javaTime;
        }catch(Exception exc)
        {
            return -1;
        }
    }
}
