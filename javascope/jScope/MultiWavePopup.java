package jScope;

import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;

import javax.swing.JCheckBoxMenuItem;
import javax.swing.JMenu;
import javax.swing.JMenuItem;

public class MultiWavePopup
    extends WavePopup
{
    protected MultiWaveform wave = null;
    protected JMenuItem legend, remove_legend;
    protected JMenu signalList;

    public MultiWavePopup()
    {
        this(null, null);
    }

    public MultiWavePopup(SetupWaveformParams setup_params,
                          ProfileDialog profDialog)
    {
        super(setup_params, profDialog);

        legend = new JMenuItem("Position legend");
        legend.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                if (!wave.isFixedLegend() || !wave.IsShowLegend())
                    PositionLegend(new Point(curr_x, curr_y));
                else
                if (wave.isFixedLegend())
                    RemoveLegend();
            }
        });
        legend.setEnabled(false);
        remove_legend = new JMenuItem("Remove legend");
        remove_legend.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                RemoveLegend();
            }
        });
        remove_legend.setEnabled(false);

        signalList = new JMenu("Signals");
        signalList.setEnabled(false);

    }
    protected Waveform getWave() {return super.wave;}

    protected void RemoveLegend()
    {
        wave.RemoveLegend();
    }

    protected void PositionLegend(Point p)
    {
        wave.SetLegend(p);
    }

    protected void SetMenuItem(boolean is_image)
    {
        int start = 0;
        super.SetMenuItem(is_image);
        if (!is_image)
        {
            if (parent instanceof WaveformManager)
                start += 2;

            insert(legend, start + 1);
            if (wave.isFixedLegend())
            {
                insert(signalList, start + 4);
                legend.setText("Show Legend");
            }
            else
            {
                insert(remove_legend, start + 2);
                insert(signalList, start + 5);
            }
        }
    }

    protected void SetSignalMenu()
    {
        super.SetSignalMenu();
        if (wave.GetShowSignalCount() == 0)
        {
            legend.setEnabled(false);
            remove_legend.setEnabled(false);
            signalList.setEnabled(false);
        }
    }

    protected void InitOptionMenu()
    {
        int sig_idx;
        String s_name[] = wave.GetSignalsName();
        boolean s_state[] = wave.GetSignalsState();

        if (! (s_name != null && s_state != null &&
               s_name.length > 0 && s_state.length > 0 &&
               s_name.length == s_state.length))
            return;

        boolean state = (wave.mode == Waveform.MODE_POINT ||
                         wave.GetShowSignalCount() == 1);

        markerList.setEnabled(state);
        colorList.setEnabled(state);
        set_point.setEnabled(wave.mode == Waveform.MODE_POINT);

        if (state)
        {
            if (wave.GetShowSignalCount() == 1)
                sig_idx = 0;
            else
                sig_idx = wave.GetSelectedSignal();

            boolean state_m = state && (wave.GetMarker(sig_idx) != Signal.NONE
                                        &&
                                        wave.GetMarker(sig_idx) != Signal.POINT);
            markerStep.setEnabled(state_m);
            SelectListItem(markerList_bg, wave.GetMarker(sig_idx));

            int st;
            for (st = 0; st < Signal.markerStepList.length; st++)
                if (Signal.markerStepList[st] == wave.GetMarkerStep(sig_idx))
                    break;
            SelectListItem(markerStep_bg, st);

            SelectListItem(colorList_bg, wave.GetColorIdx(sig_idx));

        }
        else
            markerStep.setEnabled(false);

        JCheckBoxMenuItem ob;
        if (s_name != null)
        {

            if (signalList.getItemCount() != 0)
                signalList.removeAll();
            signalList.setEnabled(s_name.length != 0);
            legend.setEnabled(s_name.length != 0);

            for (int i = 0; i < s_name.length; i++)
            {
                ob = new JCheckBoxMenuItem(s_name[i]);
                signalList.add(ob);
                ob.setState(s_state[i]);
                ob.addItemListener(new ItemListener()
                {
                    public void itemStateChanged(ItemEvent e)
                    {
                        Object target = e.getSource();

                        SetSignalState( ( (JCheckBoxMenuItem) target).getText(),
                                       ( (JCheckBoxMenuItem) target).getState());
                        wave.Repaint(true);
                    }
                });
            }
        }

        if (wave.isFixedLegend())
        {
            if (wave.IsShowLegend())
                legend.setText("Hide Legend");
            else
                legend.setText("Show Legend");
        }
        else
        {
            legend.setText("Position Legend");
            if (wave.IsShowLegend())
                remove_legend.setEnabled(true);
            else
                remove_legend.setEnabled(false);
        }

    }

    protected void SetMenu()
    {
        this.wave = (MultiWaveform)super.wave;
        super.SetMenu();
    }


    protected void SetInterpolate(boolean state)
    {
        wave.SetInterpolate(wave.GetSelectedSignal(), state);
    }

    protected void SetMode2D(int mode)
    {
        wave.setSignalMode(wave.GetSelectedSignal(), mode);
    }

    public void SetSignalState(String label, boolean state)
    {
        wave.SetSignalState(label, state);
    }

    public void SetMarker(int idx)
    {
        if (wave.GetMarker(wave.GetSelectedSignal()) != idx)
            wave.SetMarker(wave.GetSelectedSignal(), idx);
    }

    public void SetMarkerStep(int step)
    {
        if (wave.GetMarkerStep(wave.GetSelectedSignal()) != step)
            wave.SetMarkerStep(wave.GetSelectedSignal(), step);
    }

    public void SetColor(int idx)
    {
        if (wave.GetColorIdx(wave.GetSelectedSignal()) != idx)
            wave.SetColorIdx(wave.GetSelectedSignal(), idx);
    }

}
