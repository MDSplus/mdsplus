package jScope;

/* $Id$ */
import jScope.WaveformManager;
import jScope.Waveform;
import jScope.SetupDataDialog;
import jScope.MultiWavePopup;
import jScope.ProfileDialog;
import jScope.ColorMapDialog;
import java.awt.Point;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.io.File;

import java.io.FileWriter;
import java.io.BufferedWriter;
import java.io.IOException;
import javax.swing.*;

class jScopeWavePopup
    extends MultiWavePopup
{
    private SetupDataDialog setup_dialog;
    protected JMenuItem refresh;
    protected JMenuItem selectWave;
    //profile_dialog;
    protected JSeparator sep1;

    public jScopeWavePopup(SetupDataDialog setup_dialog,
                           ProfileDialog profDialog,
                           ColorMapDialog colorMapDialog)
    {
        super(null, profDialog);
        this.setColorMapDialog(colorMapDialog);
        setup.setText("Setup data source...");
        this.setup_dialog = setup_dialog;

        selectWave = new JMenuItem("Select wave panel");
        selectWave.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                if (wave !=
                    ( (WaveformManager) jScopeWavePopup.this.parent).GetSelected())
                    ( (WaveformManager) jScopeWavePopup.this.parent).Select(
                        wave);
                else
                    ( (WaveformManager) jScopeWavePopup.this.parent).Deselect();
            }
        }
        );

        sep1 = new JSeparator();
        refresh = new JMenuItem("Refresh");
        refresh.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                ( (jScopeWaveContainer) jScopeWavePopup.this.parent).Refresh( ( (
                    jScopeMultiWave) wave), "Refresh ");
            }
        }
        );

/*
        sep3 = new JSeparator();
        saveAsText = new JMenuItem("Save as text ...");
        saveAsText.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                ( (jScopeWaveContainer) jScopeWavePopup.this.parent).SaveAsText( ( (
                    jScopeMultiWave) wave), false);
            }
        }
        );
 */
        /*************
             profile_dialog = new JMenuItem("Show profile dialog");
             profile_dialog.addActionListener(new ActionListener()
                 {
                     public void actionPerformed(ActionEvent e)
                     {
                         ((jScopeWaveContainer)jScopeWavePopup.this.parent).ShowProfileDialog(((jScopeMultiWave)wave));
                     }
                 }
             );
         **************/

    }

    protected void ShowDialog()
    {
        jScopeWavePopup.this.ShowSetupDialog();
    }

    protected void SetMenuItem(boolean is_image)
    {
        super.SetMenuItem(is_image);
        insert(refresh, this.getComponentCount() - 2);
        setup.setEnabled( (setup_dialog != null));
        if (is_image)
        {
            //insert(profile_dialog, 3);
        }
        else
        {
            insert(selectWave, 2);
            add(sep3);
            add(saveAsText);
        }

        //come patch da eliminare
        if (wave != null && ( (jScopeMultiWave) wave).wi.num_waves == 1)
            ( (jScopeMultiWave) wave).wi.signal_select = 0;
    }

    protected void SetMenuLabel()
    {
        super.SetMenuLabel();
        if (!wave.IsImage())
        {
            if (wave.IsSelected())
                selectWave.setText("Deselect wave panel");
            else
                selectWave.setText("Select wave panel");
        }
        else
        {
            profile_dialog.setEnabled(!wave.isSendProfile());
        }
    }

    protected void SetMenu()
    {
        super.SetMenu();
        //      remove_panel.setEnabled(((WaveformManager)parent).GetWaveformCount() > 1);
        jScopeFacade.jScopeSetUI(this);
    }

    protected void ShowSetupDialog()
    {
        jScopeMultiWave w = (jScopeMultiWave) wave;
        if (w.mode == Waveform.MODE_POINT) // && w.wi.signal_select != -1)
        {
            setup_dialog.selectSignal(w.GetSelectedSignal());
        }
        else
        if (w.GetShowSignalCount() > 0 || w.is_image && w.wi.num_waves != 0)
            setup_dialog.selectSignal(1);
        Timer t = new Timer(20, new ActionListener()
        {
            public void actionPerformed(ActionEvent ae)
            {
                Point p = ( (WaveformManager) jScopeWavePopup.this.parent).
                    getWavePosition(wave);
                setup_dialog.Show(wave, p.x, p.y);
            }
        });
        t.setRepeats(false);
        t.start();
    }

    protected void RemoveLegend()
    {
        super.RemoveLegend();
        ( (jScopeMultiWave) wave).wi.ShowLegend(false);
    }

    protected void PositionLegend(Point p)
    {
        super.PositionLegend(p);
        ( (jScopeMultiWave) wave).wi.ShowLegend(true);
        ( (jScopeMultiWave) wave).wi.SetLegendPosition( ( (jScopeMultiWave)
            wave).GetLegendXPosition(),
            ( (jScopeMultiWave) wave).GetLegendYPosition());
    }


    protected void SetInterpolate(boolean state)
    {
        super.SetInterpolate(state);
        jScopeMultiWave w = (jScopeMultiWave)wave;
        w.wi.interpolates[w.GetSelectedSignal()] = state;
    }


    protected void SetMode1D(int mode)
    {
        jScopeMultiWave w = (jScopeMultiWave) wave;
        w.wi.mode1D[w.GetSelectedSignal()] = mode;
        super.SetMode1D(mode);
    }

    protected void SetMode2D(int mode)
    {
        jScopeMultiWave w = (jScopeMultiWave) wave;
        w.wi.mode2D[w.GetSelectedSignal()] = mode;
        super.SetMode2D(mode);
//        w.Refresh();
    }

    public void SetDeselectPoint(Waveform w)
    {

        String f_name = System.getProperty("jScope.save_selected_points");

        if (w.ShowMeasure() && f_name != null && f_name.length() != 0)
        {
            long shot = 0;
            jScopeMultiWave mw = (jScopeMultiWave) w;
            if (mw.wi.shots != null)
                shot = mw.wi.shots[mw.GetSelectedSignal()];

            try
            {
                boolean exist = false;
                File f = new File(f_name);
                if (f.exists())
                    exist = true;
                BufferedWriter out = new BufferedWriter(new FileWriter(f_name, true));
                if (!exist)
                {
                    out.write(" Shot X1 Y1 X2 Y2");
                    out.newLine();
                }
                out.write(" " + shot + w.getIntervalPoints());
                out.newLine();
                out.close();
            }
            catch (IOException e)
            {}
        }
        super.SetDeselectPoint(w);
    }

    public void SetSignalState(String label, boolean state)
    {
        jScopeMultiWave w = (jScopeMultiWave) wave;
        w.SetSignalState(label, state);
    }

    public void SetMarker(int idx)
    {
        super.SetMarker(idx);
        jScopeMultiWave w = (jScopeMultiWave) wave;
        if (w.wi.markers[w.GetSelectedSignal()] != idx)
            w.wi.markers[w.GetSelectedSignal()] = idx;
    }

    public void SetMarkerStep(int step)
    {
        super.SetMarkerStep(step);
        jScopeMultiWave w = (jScopeMultiWave) wave;
        if (w.wi.markers_step[w.GetSelectedSignal()] != step)
            w.wi.markers_step[w.GetSelectedSignal()] = step;
    }

    public void SetColor(int idx)
    {
        super.SetColor(idx);
        jScopeMultiWave w = (jScopeMultiWave) wave;
        int sigIdx = w.GetSelectedSignal();
        if (sigIdx != -1 && w.wi.colors_idx[sigIdx] != idx)
        {
            w.wi.colors_idx[w.GetSelectedSignal()] = idx % Waveform.colors.length;
            w.SetCrosshairColor(idx);
        }
    }
}
