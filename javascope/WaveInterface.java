/* $Id$ */
import java.awt.*;
import java.io.*;
import java.awt.image.*;
import java.util.Vector;

public class WaveInterface
{
    static SignalBox sig_box = new SignalBox();

    static final int MAX_NUM_SHOT = 30;
    public Waveform wave;
    public boolean full_flag;
    public int num_waves;
    public boolean x_log, y_log;
    public String in_label[], in_x[], in_y[], in_up_err[], in_low_err[];

    //Prameter used to evaluate waveform
    public String in_xmin, in_xmax, in_ymax, in_ymin, in_timemax, in_timemin;
    public String in_title, in_xlabel, in_ylabel;
    public boolean in_upd_limits = true;
    public String experiment;
    public int in_grid_mode;
    public int height;

    public String in_shot;
    public int num_shot = 1;
    protected boolean modified = true;

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
    double wave_xmin, wave_xmax;
    int wave_timestamp;
    AsynchUpdater du;
    ContinuousUpdater cu;
    boolean request_pending;
    double orig_xmin, orig_xmax;
    protected boolean evaluated[];

    static final double HUGE = (double) 1E8;

    protected boolean is_image = false;
    boolean keep_ratio = true;
    boolean horizontal_flip = false;
    boolean vertical_flip = false;
    int signal_select = -1;
    Frames frames;

    protected boolean is_async_update = false;

    static public boolean auto_color_on_expr = false;

    //True when a signal is added
    protected boolean add_signal = false;
    protected boolean is_signal_added = false;

    boolean cache_enabled = false;
    static SignalCache sc = null;
    static boolean brief_error = true;

    ColorMap colorMap = new ColorMap();

    boolean handlingTime = false;


    public WaveInterface()
    {
        CreateWaveInterface(null, null);
    }

    public WaveInterface(DataProvider dp)
    {
        CreateWaveInterface(null, dp);
    }

    public WaveInterface(Waveform wave, DataProvider dp)
    {
        CreateWaveInterface(wave, dp);
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
        zlabel = null;
        is_image = false;
        keep_ratio = true;
        horizontal_flip = false;
        vertical_flip = false;
        frames = null;
        show_legend = false;

        if (cu != null && cu.isAlive())
        {
            cu.terminate();
            try
            {
                cu.join(); //force termination and wait
            }
            catch (Exception exc)
            {}
            cu = null;
        }

        if (du != null && du.isAlive())
        {
            du.Destroy();
            try
            {
                du.join();
            }
            catch (InterruptedException exc)
            {}
            du = null;
        }
        wave_signals = null;
        evaluated = null;
    }

    public void SetAsImage(boolean is_image)
    {
        this.is_image = is_image;
    }

    public void EnableCache(boolean state)
    {
        if (state && sc == null)
            sc = new SignalCache();
        if (!state)
            sc = null;
        cache_enabled = state;
    }

    static public void FreeCache()
    {
        sc.freeCache();
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
        full_flag = true;
        experiment = null;
        shots = null;
        in_xmin = in_xmax = in_ymin = in_ymax = in_title = null;
        in_xlabel = in_ylabel = in_timemax = in_timemin = null;
        markers = null;
        interpolates = null;
        mode2D = null;
        mode1D = null;
        du = null;
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
        int l = 0, curr_num_shot;

        if (curr_shots == null)
        {
            curr_num_shot = 1;
            shots = null;
            if (num_shot == 0)
                return false;
        }
        else
            curr_num_shot = curr_shots.length;

        modified = true;

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
            shots = curr_shots;
            return true;
        }

        if (num_signal == 0)
            return false;

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
        /*
        else
        {
            try{
                title = dp.GetWaveData(in_y[0]).GetTitle();
            }catch(Exception exc) {title = null;}
        }
        */


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
        }
        else
            xmin = (!is_image) ? -HUGE : -1;

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
        }
        else
            xmax = (!is_image) ? HUGE : -1;

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
            ymax = (!is_image) ? HUGE : -1;

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
            ymin = (!is_image) ? -HUGE : -1;

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
                timemax = HUGE;

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
                timemin = -HUGE;
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
        /*
              else
              {
            try {
                xlabel = dp.GetWaveData(in_y[0], in_x[0]).GetXLabel();
            }catch(Exception exc) {xlabel = null;}
              }
         */

        if (in_ylabel != null && (in_ylabel.trim()).length() != 0)
        {
            ylabel = dp.GetString(in_ylabel);
            if (ylabel == null)
            {
                error = dp.ErrorString();
                return 0;
            }
        }
        /*
              else
              {
            try {
                ylabel = dp.GetWaveData(in_y[0]).GetYLabel();
               }catch(Exception exc) {ylabel = null; }
           }
         */

        if (xmin > xmax)
            xmin = xmax;
        if (ymin > ymax)
            ymin = ymax;

        return 1;
    }

    public synchronized void EvaluateShot(long shot) throws IOException
    {
        int curr_wave;

        if (is_image)
            return;

        for (curr_wave = 0; curr_wave < num_waves; curr_wave++)
        {
            if (shots[curr_wave] == shot && !evaluated[curr_wave] &&
                (interpolates[curr_wave] ||
                 markers[curr_wave] != Signal.NONE))
            {
                w_error[curr_wave] = null;
                evaluated[curr_wave] = true;
                signals[curr_wave] = GetSignal(curr_wave, -HUGE, HUGE);
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

    public void setLimits(Signal s)
    {
        if (xmin != -HUGE)
            s.setXmin(xmin, Signal.SIMPLE);
        if (xmax != HUGE)
            s.setXmax(xmax, Signal.SIMPLE);
        if (ymin != -HUGE)
            s.setYmin(ymin, Signal.SIMPLE);
        if (ymax != HUGE)
            s.setYmax(ymax, Signal.SIMPLE);
    }

    public boolean allEvaluated()
    {
        if (evaluated == null)
            return false;
        for (int curr_wave = 0; curr_wave < num_waves; curr_wave++)
            if (!evaluated[curr_wave])
                return false;
        modified = false;
        return true;
    }

    public synchronized void EvaluateOthers() throws IOException
    {
        int curr_wave;

        if (is_image)
        {
            InitializeFrames();
            if (frames != null)
                frames.SetViewRect( (int) xmin, (int) ymin, (int) xmax,
                                   (int) ymax);
            error = curr_error;
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
                evaluated[curr_wave] = true;
//                signals[curr_wave] = GetSignal(curr_wave, (double) - HUGE, (double) HUGE);
                signals[curr_wave] = GetSignal(curr_wave, (double) xmin, (double) xmax);
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
        modified = false;
    }

    private void CreateNewFramesClass(int image_type) throws IOException
    {

        if (image_type == FrameData.JAI_IMAGE)
        {
            try
            {
                Class cl = Class.forName("FrameJAI");
                frames = (Frames) cl.newInstance();
            }
            catch (Exception e)
            {
                throw (new IOException(
                    "Java Advanced Imaging must be installed to show this type of image"));
            }
        }
        else
        {
            frames = new Frames();
            frames.setColorMap(colorMap);
        }
    }

    private void InitializeFrames()
    {
        float f_time[];
        int j = 0, i = 0;
        curr_error = null;
        byte buf[];
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
                frames.setHorizontalFlip(horizontal_flip);
                frames.setVerticalFlip(vertical_flip);
                frames.SetFrameData(fd);
                if (in_label != null && in_label[0] != null &&
                    in_label[0].length() != 0)
                    frames.setName(in_label[0]);
                else
                    frames.setName(in_y[0]);
            }
            else
                curr_error = dp.ErrorString();
            frames.WaitLoadFrame();

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
        return GetShotArray(in_shots, dp);
    }

    static public long[] GetShotArray(String in_shots, DataProvider dp) throws
        IOException
    {
        long shot_list[] = null;
        String error;

        if (in_shots == null || in_shots.trim().length() == 0)
            return null;

        shot_list = dp.GetShots(in_shots);
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
            shot_list = null;
            throw (new IOException(error));
        }
        return shot_list;
    }

    private Signal GetSignal(int curr_wave, double xmin, double xmax) throws
        IOException
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

            if (cache_enabled && full_flag && !is_async_update)
            {
                out_signal = GetSignalFromCache(curr_wave, xmin, xmax);
            }

            if (out_signal == null)
            {
                synchronized (dp)
                {
                        out_signal = GetSignalFromProvider(curr_wave, xmin, xmax);

                }

                if (! (full_flag ||
                       wave_signals == null ||
                       wave_signals.length <= curr_wave ||
                       wave_signals[curr_wave] == null))
                {
                    // In this case GetSignal is called only
                    // to inflate the signal and therefore
                    // limits must not be changed
                    this.wave.SetMode(mode);
                    return out_signal;
                }

            }

            if (out_signal != null)
            {
                if (xmin > xmax)
                    xmin = xmax;
                if (ymin > ymax)
                    ymin = ymax;

                if (xmin != -HUGE)
                    out_signal.setXmin(xmin,
                                       Signal.AT_CREATION | Signal.FIXED_LIMIT);
                if (xmax != HUGE)
                    out_signal.setXmax(xmax,
                                       Signal.AT_CREATION | Signal.FIXED_LIMIT);

                if (in_ymax != null && (in_ymax.trim()).length() != 0 &&
                    in_upd_limits)
                    out_signal.setYmax(ymax,
                                       Signal.AT_CREATION | Signal.FIXED_LIMIT);

                if (in_ymin != null && (in_ymin.trim()).length() != 0 &&
                    in_upd_limits)
                    out_signal.setYmin(ymin,
                                       Signal.AT_CREATION | Signal.FIXED_LIMIT);

                out_signal.setFullLoad(full_flag);
            }
            this.wave.SetMode(mode);
       }
        catch(IOException exc)
        {
            this.wave.SetMode(mode);
            throw(exc);
        }
        return out_signal;
    }

    private Signal GetSignalFromCache(int curr_wave, double xmin, double xmax) throws
        IOException
    {
        float curr_data[] = null, curr_x[] = null, up_err[] = null,
            low_err[] = null;
        double curr_x_double[] = null;
        long   curr_x_long[] = null;
        int x_samples = 0, min_len;
        long sh = 0;
        WaveData wd;
        Signal out_signal = null;
        DataCacheObject cd;
        String expr;

        expr = in_y[curr_wave];
        if (in_up_err[curr_wave] != null)
            expr = expr + in_up_err[curr_wave];

        if (in_low_err[curr_wave] != null)
            expr = expr + in_low_err[curr_wave];

        if (shots != null && shots.length != 0)
            sh = shots[curr_wave];

        cd = (DataCacheObject) sc.getCacheData(provider, expr, experiment, sh);
        if (cd != null)
        {

            WaveformEvent we = new WaveformEvent(wave, WaveformEvent.CACHE_DATA,
                                                 "Cache");
            wave.dispatchWaveformEvent(we);

            title = cd.title;
            xlabel = cd.x_label;
            ylabel = cd.y_label;
            zlabel = cd.z_label;
            up_err = cd.up_err;
            low_err = cd.low_err;
            curr_data = cd.data;
            curr_x = cd.x;
            curr_x_double = cd.x_double;
            if ( (curr_x == null && curr_x_double == null) || curr_data == null)
                return null;

            if (curr_x != null)
            {
                if (curr_data.length < curr_x.length)
                    min_len = curr_data.length;
                else
                    min_len = curr_x.length;
            }
            else
            {
                if (curr_data.length < curr_x_double.length)
                    min_len = curr_data.length;
                else
                    min_len = curr_x_double.length;
            }
            if (cd.dimension == 2)
            {
                float[] curr_y = cd.y;
                if (curr_x != null)
                    out_signal = new Signal(curr_data, curr_y, curr_x,
                                            Signal.TYPE_2D);
                else
                    out_signal = new Signal(curr_data, curr_y, curr_x_double,
                                            Signal.TYPE_2D);
                out_signal.setMode2D(mode2D[curr_wave]);
            }
            else
            {

                if (curr_x != null)
                    out_signal = new Signal(curr_x, curr_data, min_len);
                else
                    out_signal = new Signal(curr_x_double, curr_data, min_len);

                out_signal.setMode1D(mode1D[curr_wave]);
            }

            if (up_err != null && low_err != null)
                out_signal.AddAsymError(up_err, low_err);
            else
            if (up_err != null)
                out_signal.AddError(up_err);

            out_signal.setLabels(title, xlabel, ylabel, zlabel);
        }
        return out_signal;
    }

    private Signal GetSignalFromProvider(int curr_wave, double xmin, double xmax) throws
        IOException
    {
        float curr_data[] = null, curr_x[] = null, curr_y[] = null, up_err[] = null,
            low_err[] = null;
        double curr_x_double[] = null;
        long   curr_x_long[] = null;
        int x_samples = 0, min_len;
        WaveData wd = null;
        int dimension;
        Signal out_signal;
        String xlabel = null, ylabel = null, zlabel = null, title = null;

        if (shots != null && shots.length != 0)
            dp.Update(experiment, shots[curr_wave]);
        else
            dp.Update(null, 0);

        if (dp.ErrorString() != null)
        {
            error = dp.ErrorString();
            return null;
        }

        if (in_x[curr_wave] != null && (in_x[curr_wave].trim()).length() > 0)
        {
            dimension = 1;
        }
        else
        {
            try
            {
//                if(xmin != -HUGE || !full_flag) //If we actually have some limit or resampling enabled
                if(!full_flag) //If we actually have some limit or resampling enabled
                {
                    wd = dp.GetResampledWaveData(in_y[curr_wave], xmin, xmax,
                                                 Waveform.MAX_POINTS);
                }
                else
                    wd = dp.GetWaveData(in_y[curr_wave]);
                dimension = wd.GetNumDimension();
                if (dimension == 2)
                    zlabel = wd.GetZLabel();
            }
            catch (Exception exc)
            {
                dimension = 1;
            }

            if (dp.ErrorString() != null)
            {
                curr_error = dp.ErrorString();
                return null;
            }
        }
        if (dimension > 2)
        {
            curr_error = "Can't display signal with more than two dimensions";
            return null;
        }

        if (in_x[curr_wave] != null && (in_x[curr_wave].trim()).length() != 0)
        {

            wd = dp.GetWaveData(in_y[curr_wave], in_x[curr_wave]);
            if (wd == null)
                curr_data = null;
            else
            {
                xlabel = wd.GetXLabel();
                ylabel = wd.GetYLabel();
                curr_data = wd.GetFloatData();
            }

            if (curr_data != null && curr_data.length > 1 && in_up_err != null &&
                in_up_err[curr_wave] != null &&
                (in_up_err[curr_wave].trim()).length() != 0)
            {
                up_err = dp.GetWaveData(in_up_err[curr_wave]).GetFloatData();
                if (up_err == null || up_err.length <= 1)
                    curr_data = null;
            }

            if (curr_data != null && in_low_err != null &&
                in_low_err[curr_wave] != null &&
                (in_low_err[curr_wave].trim()).length() != 0)
            {
                low_err = dp.GetWaveData(in_low_err[curr_wave]).GetFloatData();
                if (low_err == null || low_err.length <= 1)
                    curr_data = null;
            }

            if (curr_data != null)
            {
                curr_x_double = wd.GetXDoubleData();
                if (curr_x_double == null || curr_x_double.length <= 1)
                    curr_x = wd.GetXData();
                if (curr_x == null || curr_x.length <= 1)
                    curr_x_long = wd.GetXLongData();

                if ( (curr_x == null || curr_x.length <= 1) &&
                    (curr_x_double == null || curr_x_double.length <= 1) &&
                    (curr_x_long == null || curr_x_long.length <= 1) )
                    curr_data = null;

             }
        }
        else // Campo X non definito
        {
//            if (full_flag || dimension > 1)
            if (dimension > 1)
            {
                if (wd == null)
                    wd = dp.GetWaveData(in_y[curr_wave]);
            }
            else
            {
//                if(xmin != -HUGE || !full_flag) //If we actually have some limit
                    if(!full_flag) //If we actually have some limit
                {
                    wd = dp.GetResampledWaveData(in_y[curr_wave], xmin, xmax,
                                                 Waveform.MAX_POINTS);
                }
                else
                    wd = dp.GetWaveData(in_y[curr_wave]);
            }
            if (wd == null)
                curr_data = null;
            else
            {
                xlabel = wd.GetXLabel();
                ylabel = wd.GetYLabel();
                curr_data = wd.GetFloatData();
            }

            if (dimension == 1)
            {
                if (curr_data != null && curr_data.length > 1 && in_up_err != null &&
                    in_up_err[curr_wave] != null
                    && (in_up_err[curr_wave].trim()).length() != 0)
                {
                    if (full_flag)
                        up_err = dp.GetWaveData(in_up_err[curr_wave]).
                            GetFloatData();
                    else
                        up_err = dp.GetResampledWaveData(in_up_err[curr_wave],
                            in_y[curr_wave], xmin, xmax, Waveform.MAX_POINTS).
                            GetFloatData();
                    if (up_err == null || up_err.length <= 1)
                        curr_data = null;
                }

                if (curr_data != null && in_low_err != null &&
                    in_low_err[curr_wave] != null &&
                    (in_low_err[curr_wave].trim()).length() != 0)
                {
                    if (full_flag)
                        low_err = dp.GetWaveData(in_low_err[curr_wave]).
                            GetFloatData();
                    else
                        low_err = dp.GetResampledWaveData(in_low_err[curr_wave],
                            in_y[curr_wave], xmin, xmax, Waveform.MAX_POINTS).
                            GetFloatData();

                    if (low_err == null || low_err.length <= 1)
                        curr_data = null;
                }
            }

            if (curr_data != null)
            {
                curr_x_double = wd.GetXDoubleData();
                if (curr_x_double != null)
                {
                    if (! (full_flag || dimension > 1))
                        x_samples = curr_x_double.length;
                }
                else
                {

                    curr_x_long = wd.GetXLongData();


                    if (curr_x_long != null && ! (full_flag || dimension > 1))
                        x_samples = curr_x_long.length;

                     if (curr_x_long == null)
                     {
                        curr_x = wd.GetXData();
                        if (curr_x != null && ! (full_flag || dimension > 1))
                            x_samples = curr_x.length;
                        if (curr_x == null)
                             curr_data = null;
                     }
                }
            }
        }
        if ( ( (curr_x == null        || curr_x.length < 1) &&
               (curr_x_long == null   || curr_x_long.length < 1 ) &&
               (curr_x_double == null || curr_x_double.length < 1 )) ||
               (curr_data == null     || curr_data.length < 1 ) )
        {
            curr_error = dp.ErrorString();
            return null;
        }
        // Se e' definito il campo x si assume full_flag true
        if (full_flag ||
            dimension > 1 ||
            (in_x[curr_wave] != null && (in_x[curr_wave].trim()).length() != 0))
            min_len = curr_data.length;
        else
        {
            if (curr_data.length < x_samples)
                min_len = curr_data.length;
            else
                min_len = x_samples;
        }

        if (dimension == 2)
        {
            curr_y = wd.GetYData();
            if (curr_x == null)
            {
                if (curr_x_double != null)
                    out_signal = new Signal(curr_data, curr_y, curr_x_double,
                                            Signal.TYPE_2D);
                else
                    out_signal = new Signal(curr_data, curr_y, curr_x_long,
                                            Signal.TYPE_2D);
            }
            else
                out_signal = new Signal(curr_data, curr_y, curr_x,
                                        Signal.TYPE_2D);

            out_signal.setMode2D( (int) mode2D[curr_wave]);
        }
        else
        {
            if (curr_x == null)
                if(curr_x_double != null)
                {
                    out_signal = new Signal(curr_x_double, curr_data, min_len);
                    handlingTime = false;
                }
                else
                {
                    out_signal = new Signal(curr_x_long, curr_data, min_len);
                    handlingTime = true;
                }
            else
            {
                out_signal = new Signal(curr_x, curr_data, min_len);
                handlingTime = false;
            }
            out_signal.setMode1D( (int) mode1D[curr_wave]);
        }

        if (wd != null)
            title = wd.GetTitle();

        if ( (cache_enabled || sc != null) && full_flag && !is_async_update)
        {
            long sh = 0;
            DataCacheObject cd = new DataCacheObject();

            String expr = in_y[curr_wave];
            if (in_up_err[curr_wave] != null)
                expr = expr + in_up_err[curr_wave];

            if (in_low_err[curr_wave] != null)
                expr = expr + in_low_err[curr_wave];

            cd.title = (this.title != null) ? this.title : title;
            cd.x_label = (this.xlabel != null) ? this.xlabel : xlabel;
            cd.y_label = (this.ylabel != null) ? this.ylabel : ylabel;
            cd.z_label = (this.zlabel != null) ? this.zlabel : zlabel;
            cd.up_err = up_err;
            cd.low_err = low_err;
            cd.data = curr_data;
            cd.x = curr_x;
            cd.x_double = curr_x_double;
            cd.y = curr_y;
            cd.dimension = dimension;

            if (shots != null && shots.length != 0)
                sh = shots[curr_wave];

            sc.putCacheData(provider, expr, experiment, sh, cd);
        }

        if (up_err != null && low_err != null)
            out_signal.AddAsymError(up_err, low_err);
        else
        if (up_err != null)
            out_signal.AddError(up_err);

        out_signal.setLabels(title, xlabel, ylabel, zlabel);
        return out_signal;
    }

    void ContinuousUpdate(Vector sigs, MultiWaveform w)
    {
        wave_signals = new Signal[sigs.size()];
        for (int i = 0; i < sigs.size(); i++)
            wave_signals[i] = (Signal) sigs.elementAt(i);
        if (!dp.SupportsContinuous())
            return;
        if (cu != null)
        {
            cu.terminate();
            try
            {
                cu.join(); //force termination and wait
            }
            catch (Exception exc)
            {}
        }
        cu = new ContinuousUpdater(this, w, dp);
        cu.start();
    }

    void AsynchUpdate(Vector sigs, double xmin, double xmax,
                      double _orig_xmin, double _orig_xmax,
                      int timestamp, boolean panning,
                      MultiWaveform w)
    {
        int curr_wave;
        boolean needs_update = false;
        if (full_flag)
            return;
        wave_signals = new Signal[sigs.size()];
        for (int i = 0; i < sigs.size(); i++)
            wave_signals[i] = (Signal) sigs.elementAt(i);
        wave_xmin = xmin;
        wave_xmax = xmax;
        wave_timestamp = timestamp;
        request_pending = true;
        orig_xmin = _orig_xmin;
        orig_xmax = _orig_xmax;
        for (curr_wave = 0; curr_wave < num_waves; curr_wave++)
        {
            if (wave_signals[curr_wave] == null)
                continue;
            if (wave_signals[curr_wave].n_points >= Waveform.MAX_POINTS - 5 ||
                (panning &&
                 (orig_xmin < xmin || orig_xmax > xmax)) ||
                (orig_xmin > xmin || orig_xmax < xmax))
                needs_update = true;
        }
        if (needs_update && dp.SupportsFastNetwork())
        {
            if (asynch_update)
            {
                if (du == null)
                {
                    du = new AsynchUpdater(this, w);
                    du.start();
                }
                else
                    du.Notify();
            }
            else
            {
                try
                {
                    DynamicUpdate(w, false);
                }
                catch (IOException e)
                {}
            }
        }
    }

    public
    /*synchronized*/ void DynamicUpdate(MultiWaveform w, boolean is_continuous) throws
        IOException
    {
        boolean needs_update = false;
        int curr_wave, saved_timestamp = wave_timestamp;
        WaveformEvent we;

        if (!is_continuous)
        {
            we = new WaveformEvent(w, "Asyncronous signal loading");
            w.dispatchWaveformEvent(we);
        }

        is_async_update = true;

        for (curr_wave = 0; curr_wave < num_waves; curr_wave++)
        {
            //Check if signal is full loaded, i.e. does not need dynamic update
            if (!is_continuous && wave_signals[curr_wave] != null &&
                wave_signals[curr_wave].isFullLoad())
                continue;

            needs_update = true;
            if (is_continuous) //continuous signal update
                wave_signals[curr_wave] = GetSignal(curr_wave, xmin, xmax); //-HUGE, HUGE);
            else //asyncronous update during zoom
            {
                wave_signals[curr_wave] = GetSignal(curr_wave, wave_xmin,
                    wave_xmax);
            }
        }
        if (needs_update && saved_timestamp == wave_timestamp)
        {
            synchronized (this)
            {
                w.UpdateSignals(wave_signals, wave_timestamp, is_continuous);
            }
        }

        is_async_update = false;

        if (!is_continuous)
        {
            we = new WaveformEvent(w, "Asyncronous operation completed");
            w.dispatchWaveformEvent(we);
        }
    }
}

class ContinuousUpdater
    extends Thread
{
    WaveInterface wi;
    MultiWaveform w;
    boolean terminate;
    DataProvider dp;

    public ContinuousUpdater(WaveInterface wi, MultiWaveform w, DataProvider dp)
    {
        this.wi = wi;
        this.w = w;
        this.dp = dp;
        terminate = false;
    }

    public void run()
    {
        setName("Continuous Update Thread");
        w.SetEnabledPan(false);
        while (dp.DataPending())
        {
            try
            {
                wi.DynamicUpdate(w, true);
            }
            catch (Exception exc)
            {
                System.out.println(exc);
            }
            if (terminate)
                break;
        }
        w.SetEnabledPan(true);
    }

    public void terminate()
    {
        terminate = true;
    }
}

class AsynchUpdater
    extends Thread
{
    WaveInterface wi;
    MultiWaveform w;
    boolean done = false;

    public AsynchUpdater(WaveInterface wi, MultiWaveform w)
    {
        this.wi = wi;
        this.w = w;
    }

    public void run()
    {
        setName("Asynch Update Thread");
        while (!done)
        {
            while (wi.request_pending)
            {
                wi.request_pending = false;

                try
                {
                    wi.DynamicUpdate(w, false);
                }
                catch (Exception e)
                {}
            }
            try
            {
                synchronized (this)
                {
                    wait();
                }
            }
            catch (InterruptedException e)
            {}
        }
    }

    public synchronized void Notify()
    {
        notify();
    }

    public synchronized void Destroy()
    {
        done = true;
        notify();
    }

}
