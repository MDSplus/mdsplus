package jScope;

import java.awt.Color;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Graphics2D;
import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.KeyEvent;
import java.awt.image.BufferedImage;
import java.util.Enumeration;
import java.util.StringTokenizer;

import javax.swing.ButtonGroup;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPopupMenu;
import javax.swing.JRadioButtonMenuItem;
import javax.swing.JSeparator;
import javax.swing.KeyStroke;

public class WavePopup
    extends JPopupMenu
    implements ItemListener
{
    protected Waveform wave = null;
    protected SetupWaveformParams setup_params;
    protected JSeparator sep1, sep2, sep3;
    protected JMenuItem setup, autoscale, autoscaleY, autoscaleAll,
        autoscaleAllY,
        allSameScale, allSameXScale, allSameXScaleAutoY, allSameYScale,
        resetScales, resetAllScales, playFrame, remove_panel,
        set_point, undo_zoom, maximize, cb_copy, profile_dialog, colorMap, saveAsText;
    protected JMenu markerList, colorList, markerStep, mode_2d, mode_1d;
    protected JRadioButtonMenuItem plot_y_time, plot_x_y, plot_contour, plot_image;
    protected JRadioButtonMenuItem plot_line, plot_no_line, plot_step;

    protected ButtonGroup markerList_bg, colorList_bg, markerStep_bg,
        mode_2d_bg, mode_1d_bg;

    protected int curr_x, curr_y;
    protected Container parent;

    private Waveform profile_source = null;
    ProfileDialog profDialog;
    ColorMapDialog colorMapDialog = null;

    public WavePopup()
    {
        this(null, null);
    }

    public WavePopup(SetupWaveformParams setup_params, ProfileDialog profDialog)
    {

        setup = new JMenuItem("Set Limits...");
        setup.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                ShowDialog();
            }
        }
        );
        this.setup_params = setup_params;
        this.profDialog = profDialog;

        remove_panel = new JMenuItem("Remove panel");
        remove_panel.setEnabled(false);
        remove_panel.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                Object[] options =
                    {
                    "Yes",
                    "No"};
                int opt = JOptionPane.showOptionDialog(null,
                    "Are you sure you want to remove this wave panel?",
                    "Warning",
                    JOptionPane.YES_NO_OPTION,
                    JOptionPane.QUESTION_MESSAGE,
                    null,
                    options,
                    options[1]);
                switch (opt)
                {
                    case JOptionPane.YES_OPTION:
                        ( (WaveformManager) parent).removePanel(wave);
                        ;
                        break;
                }

            }
        }
        );

        maximize = new JMenuItem("Maximize Panel");
        maximize.setEnabled(false);
        maximize.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                if ( ( (WaveformManager) WavePopup.this.parent).isMaximize())
                {
                    ( (WaveformManager) WavePopup.this.parent).
                        maximizeComponent(null);
                }
                else
                {
                    ( (WaveformManager) WavePopup.this.parent).
                        maximizeComponent(wave);
                }
            }
        }
        );

        set_point = new JMenuItem("Set Point");
        set_point.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                WavePopup.this.SetDeselectPoint(wave);
            }
        }
        );

        markerList = new JMenu("Markers");
        JRadioButtonMenuItem ob;
        markerList_bg = new ButtonGroup();

        for (int i = 0; i < Signal.markerList.length; i++)
        {
            markerList_bg.add(ob = new JRadioButtonMenuItem(Signal.markerList[i]));
            ob.getModel().setActionCommand("MARKER " + i);
            markerList.add(ob);
            ob.addItemListener(this);
        }
        markerList.setEnabled(false);

        markerStep_bg = new ButtonGroup();
        markerStep = new JMenu("Marker step");
        for (int i = 0; i < Signal.markerStepList.length; i++)
        {
            markerStep_bg.add(ob = new JRadioButtonMenuItem("" +
                Signal.markerStepList[i]));
            ob.getModel().setActionCommand("MARKER_STEP " + i);
            markerStep.add(ob);
            ob.addItemListener(this);
        }
        markerStep.setEnabled(false);

        colorList = new JMenu("Colors");
        colorList.setEnabled(false);

        mode_1d_bg = new ButtonGroup();
        mode_1d = new JMenu("Mode Plot 1D");
        mode_1d.add(plot_line = new JRadioButtonMenuItem("Line"));
        mode_1d_bg.add(plot_line);
        plot_line.addItemListener(new ItemListener()
        {
            public void itemStateChanged(ItemEvent e)
            {
                if (e.getStateChange() == ItemEvent.SELECTED)
                    SetMode1D(Signal.MODE_LINE);
            }
        });

        mode_1d.add(plot_no_line = new JRadioButtonMenuItem("No Line"));
        mode_1d_bg.add(plot_no_line);
        plot_no_line.addItemListener(new ItemListener()
        {
            public void itemStateChanged(ItemEvent e)
            {
                if (e.getStateChange() == ItemEvent.SELECTED)
                    SetMode1D(Signal.MODE_NOLINE);
            }
        });

        mode_1d.add(plot_step = new JRadioButtonMenuItem("Step Plot"));
        mode_1d_bg.add(plot_step);
        plot_step.addItemListener(new ItemListener()
        {
            public void itemStateChanged(ItemEvent e)
            {
                if (e.getStateChange() == ItemEvent.SELECTED)
                    SetMode1D(Signal.MODE_STEP);
//                wave.Update();
            }
        });

        mode_2d_bg = new ButtonGroup();
        mode_2d = new JMenu("signal 2D");
        mode_2d.add(plot_y_time = new JRadioButtonMenuItem("Plot xz(y)"));
        mode_2d_bg.add(plot_y_time);
        plot_y_time.addItemListener(new ItemListener()
        {
            public void itemStateChanged(ItemEvent e)
            {
                if (e.getStateChange() == ItemEvent.SELECTED)
                    SetMode2D(Signal.MODE_XZ);
            }
        });

        mode_2d.add(plot_x_y = new JRadioButtonMenuItem("Plot yz(x)"));
        mode_2d_bg.add(plot_x_y);
        plot_x_y.addItemListener(new ItemListener()
        {
            public void itemStateChanged(ItemEvent e)
            {
                if (e.getStateChange() == ItemEvent.SELECTED)
                    SetMode2D(Signal.MODE_YZ);
            }
        });


        mode_2d.add(plot_contour = new JRadioButtonMenuItem("Plot Contour"));
        mode_2d_bg.add(plot_contour);
        plot_contour.addItemListener(new ItemListener()
        {
          public void itemStateChanged(ItemEvent e)
          {
            if (e.getStateChange() == ItemEvent.SELECTED)
            {
              SetMode2D(Signal.MODE_CONTOUR);
            }
          }
        });

        mode_2d.add(plot_image = new JRadioButtonMenuItem("Plot Image"));
        mode_2d_bg.add(plot_image);
        plot_image.addItemListener(new ItemListener()
        {
            public void itemStateChanged(ItemEvent e)
            {
                if (e.getStateChange() == ItemEvent.SELECTED)
                {
                    wave.setShowSigImage(true);
                    SetMode2D(Signal.MODE_IMAGE);
                }
                else
                    wave.setShowSigImage(false);
            }
        });

        sep1 = new JSeparator();
        sep2 = new JSeparator();

        autoscale = new JMenuItem("Autoscale");
        autoscale.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                wave.Autoscale();
            }
        }
        );

        autoscaleY = new JMenuItem("Autoscale Y");
        autoscaleY.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                wave.AutoscaleY();
            }
        }
        );

        autoscaleAll = new JMenuItem("Autoscale all");
        autoscaleAll.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_B,
            ActionEvent.CTRL_MASK));
        autoscaleAll.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                if (wave.IsImage())
                    ( (WaveformManager) WavePopup.this.parent).
                        AutoscaleAllImages();
                else
                    ( (WaveformManager) WavePopup.this.parent).AutoscaleAll();
            }
        }
        );

        autoscaleAllY = new JMenuItem("Autoscale all Y");
        autoscaleAllY.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Y,
            ActionEvent.CTRL_MASK));
        autoscaleAllY.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                ( (WaveformManager) WavePopup.this.parent).AutoscaleAllY();
            }
        }
        );

        allSameScale = new JMenuItem("All same scale");
        allSameScale.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                ( (WaveformManager) WavePopup.this.parent).AllSameScale(wave);
            }
        }
        );

        allSameXScale = new JMenuItem("All same X scale");
        allSameXScale.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                ( (WaveformManager) WavePopup.this.parent).AllSameXScale(wave);
            }
        }
        );

        allSameXScaleAutoY = new JMenuItem("All same X scale (auto Y)");
        allSameXScaleAutoY.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                ( (WaveformManager) WavePopup.this.parent).AllSameXScaleAutoY(
                    wave);
            }
        }
        );

        allSameYScale = new JMenuItem("All same Y scale");
        allSameYScale.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                ( (WaveformManager) WavePopup.this.parent).AllSameYScale(wave);
            }
        }
        );

        resetScales = new JMenuItem("Reset scales");
        resetScales.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                wave.ResetScales();
            }
        }
        );

        resetAllScales = new JMenuItem("Reset all scales");
        resetAllScales.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                ( (WaveformManager) WavePopup.this.parent).ResetAllScales();
            }
        }
        );

        undo_zoom = new JMenuItem("Undo Zoom");
        undo_zoom.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                wave.undoZoom();
            }
        }
        );

        cb_copy = new JMenuItem("Copy to Clipboard");
        cb_copy.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                Dimension dim = wave.getSize();
                BufferedImage ri = new BufferedImage(dim.width, dim.height,
                    BufferedImage.TYPE_INT_RGB);
                Graphics2D g2d = (Graphics2D) ri.getGraphics();
                g2d.setBackground(Color.white);
                wave.paint(g2d, dim, Waveform.PRINT);
                try
                {
                    ImageTransferable imageTransferable = new ImageTransferable(
                        ri);
                    Clipboard cli = Toolkit.getDefaultToolkit().
                        getSystemClipboard();
                    cli.setContents(imageTransferable, imageTransferable);
                }
                catch (Exception exc)
                {
                    System.out.println("Exception " + exc);
                }
            }
        }
        );

        playFrame = new JMenuItem();
        playFrame.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                if (wave.Playing())
                    wave.StopFrame();
                else
                    wave.PlayFrame();
            }
        }
        );

        profile_dialog = new JMenuItem("Show profile dialog");
        profile_dialog.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                ShowProfileDialog(wave);
            }
        }
        );

        colorMap = new JMenuItem("Color Palette");
        colorMap.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                ShowColorMapDialog(wave);
            }
        }
        );
        
        sep3 = new JSeparator();
        saveAsText = new JMenuItem("Save as text ...");
        saveAsText.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                ( (WaveformContainer) WavePopup.this.parent).SaveAsText( wave, false);
            }
        }
        );

    }

    protected void ShowDialog()
    {
        if (setup_params != null)
            setup_params.Show(wave);
    }

    protected void SelectListItem(ButtonGroup bg, int idx)
    {
        int i;
        JRadioButtonMenuItem b = null;
        Enumeration e;

        for (e = bg.getElements(), i = 0; e.hasMoreElements() && i <= idx; i++)
            b = (JRadioButtonMenuItem) e.nextElement();
        if (b != null)
            bg.setSelected(b.getModel(), true);
    }

    protected void InitColorMenu()
    {
        if (!Waveform.isColorsChanged() && colorList_bg != null)
            return;

        if (colorList.getItemCount() != 0)
            colorList.removeAll();

        String[] colors_name = Waveform.getColorsName();
        JRadioButtonMenuItem ob = null;
        colorList_bg = new ButtonGroup();
        if (colors_name != null)
        {
            for (int i = 0; i < colors_name.length; i++)
            {
                colorList.add(ob = new JRadioButtonMenuItem(colors_name[i]));
                ob.getModel().setActionCommand("COLOR_LIST " + i);
                colorList_bg.add(ob);
                ob.addItemListener(this);
            }
        }
    }

    protected void SetMenuItem(boolean is_image)
    {

        if (getComponentCount() != 0)
            removeAll();

        if( parent != null && parent instanceof WaveformManager )
        {
            if ( ( (WaveformManager) parent).isMaximize() )
            {
                maximize.setText("Show All Panels");
            }
            else
            {
                maximize.setText("Maximize Panel");
            }
        }    

        if (is_image)
        {
            add(setup);
            colorList.setText("Colors");
            if (profDialog != null)
                add(profile_dialog);
            if (parent != null && parent instanceof WaveformManager)
            {
                add(maximize);
                add(remove_panel);
            }
            add(colorList);
            add(colorMap);
            add(playFrame);
            add(set_point);
            add(sep2);
            add(autoscale);
            if (parent != null && parent instanceof WaveformManager)
            {
                autoscaleAll.setText("Autoscale all images");
                add(autoscaleAll);
                maximize.setEnabled( ( (WaveformManager) parent).
                                    GetWaveformCount() > 1);
            }
            set_point.setEnabled( (wave.mode == Waveform.MODE_POINT));
        }
        else
        {
            add(setup);
            setup.setEnabled( (setup_params != null));
            add(set_point);
            set_point.setEnabled( (wave.mode == Waveform.MODE_POINT));
            add(sep1);
            add(markerList);
            add(markerStep);
            colorList.setText("Colors");
            add(colorList);
            if (wave.mode == Waveform.MODE_POINT ||
                wave.GetShowSignalCount() == 1)
            {
                if (wave.getSignalType() == Signal.TYPE_1D ||
                    (wave.getSignalType() == Signal.TYPE_2D &&
                    (wave.getSignalMode2D() == Signal.MODE_XZ || wave.getSignalMode2D() == Signal.MODE_YZ ) ) )
                {
                    add(mode_1d);
                    switch (wave.getSignalMode1D())
                    {
                        case Signal.MODE_LINE:
                            mode_1d_bg.setSelected(plot_line.getModel(), true);
                            break;
                        case Signal.MODE_NOLINE:
                            mode_1d_bg.setSelected(plot_no_line.getModel(), true);
                            break;
                        case Signal.MODE_STEP:
                            mode_1d_bg.setSelected(plot_step.getModel(), true);
                            break;
                    }
                }
                if (wave.getSignalType() == Signal.TYPE_2D)
                {
                    add(colorMap);
                    add(mode_2d);
                    mode_2d.setEnabled(wave.getSignalMode2D() != Signal.MODE_PROFILE);
                    switch (wave.getSignalMode2D())
                    {
                        case Signal.MODE_XZ:
                            mode_2d_bg.setSelected(plot_y_time.getModel(), true);
                            break;
                        case Signal.MODE_YZ:
                            mode_2d_bg.setSelected(plot_x_y.getModel(), true);
                            break;
                        case Signal.MODE_CONTOUR:
                            mode_2d_bg.setSelected(plot_contour.getModel(), true);
                            break;
                        case Signal.MODE_IMAGE:
                            mode_2d_bg.setSelected(plot_image.getModel(), true);
                            break;
                    }
                    plot_image.setEnabled(!wave.IsShowSigImage());
                }
            }

            add(sep2);
            add(autoscale);
            add(autoscaleY);
            if (parent != null && parent instanceof WaveformManager)
            {
                insert(maximize, 1);
                insert(remove_panel, 2);
                autoscaleAll.setText("Autoscale all");
                add(autoscaleAll);
                add(autoscaleAllY);
                add(allSameScale);
                add(allSameXScale);
                add(allSameXScaleAutoY);
                add(allSameYScale);
                add(resetAllScales);
                maximize.setEnabled( ( (WaveformManager) parent).
                                    GetWaveformCount() > 1);

            }
            add(resetScales);
            add(undo_zoom);
            //Copy image to clipborad can be done only with
            //java release 1.4
            //if(System.getProperty("java.version").indexOf("1.4") != -1)
            {
                add(cb_copy);
            }
            add(sep3);
            add(saveAsText);

        }
    }

    protected void SetImageMenu()
    {
        SetMenuItem(true);
        boolean state = (wave.frames != null && wave.frames.getNumFrame() != 0);
        colorList.setEnabled(state);
        SelectListItem(colorList_bg, wave.GetColorIdx());
        playFrame.setEnabled(state);
        set_point.setEnabled(state && ( (wave.mode == Waveform.MODE_POINT)));

        profile_dialog.setEnabled(!wave.isSendProfile());

    }

    public void setColorMapDialog(ColorMapDialog colorMapDialog)
    {
        this.colorMapDialog = colorMapDialog;
    }


    public void ShowColorMapDialog(Waveform wave)
    {
        /*
        if (colorMapDialog != null && colorMapDialog.isVisible())
            colorMapDialog.dispose();
        colorMapDialog = new ColorMapDialog(null, wave);
        */
        if (colorMapDialog == null )
        {
            colorMapDialog = new ColorMapDialog(null, null);
        }
        else
            colorMapDialog.setWave(wave);
        colorMapDialog.setLocationRelativeTo(wave);
        colorMapDialog.setVisible(true);
    }


    public void ShowProfileDialog(Waveform wave)
    {
        if (profDialog != null && profDialog.isVisible())
            profDialog.dispose();
            //profDialog = new ProfileDialog(null, wave);
        profDialog.setWaveSource(wave);
        profDialog.pack();
        profDialog.setSize(200, 250);
        if (profile_source != null)
            profile_source.setSendProfile(false);
        wave.setSendProfile(true);
        profile_source = wave;
        profDialog.setLocationRelativeTo(wave);
        profDialog.setVisible(true);
        wave.sendProfileEvent();
    }

    protected void SetSignalMenu()
    {
        SetMenuItem(false);
        if (wave.GetShowSignalCount() != 0)
        {
            InitOptionMenu();
        }
        else
        {
            markerList.setEnabled(false);
            colorList.setEnabled(false);
            markerStep.setEnabled(false);
            set_point.setEnabled(false);
        }
        //undo_zoom.setEnabled(wave.undoZoomPendig());
        undo_zoom.setEnabled(wave.undoZoomPendig());
    }

    protected void InitOptionMenu()
    {
        boolean state = (wave.GetShowSignalCount() == 1);
        markerList.setEnabled(state);
        colorList.setEnabled(state);
        set_point.setEnabled(true);

        if (state)
        {
            boolean state_m = (wave.GetMarker() != Signal.NONE);
            markerStep.setEnabled(state_m);
            SelectListItem(markerList_bg, wave.GetMarker());

            int st;
            for (st = 0; st < Signal.markerStepList.length; st++)
                if (Signal.markerStepList[st] == wave.GetMarkerStep())
                    break;
            SelectListItem(markerStep_bg, st);

            SelectListItem(colorList_bg, wave.GetColorIdx());

        }
        else
            markerStep.setEnabled(false);
    }

    public void Show(Waveform w, int x, int y, int tran_x, int tran_y)
    {
        //   parent = (Container)this.getParent();

        // if(wave != w)
        {
            wave = w;
            SetMenu();
        }
        //else
        //     if(!w.IsImage())
        //         InitOptionMenu();

        SetMenuLabel();

        curr_x = x;
        curr_y = y;
        show(w, x - tran_x, y - tran_y);
    }

    protected void SetMenuLabel()
    {
        if (!wave.IsImage())
        {
            if (wave.ShowMeasure())
            {
                set_point.setText("Deselect Point");
            }
            else
                set_point.setText("Set Point");
        }
        else
        {

            if (wave.ShowMeasure()) // && wave.sendProfile())
                set_point.setText("Deselect Point");
            else
                set_point.setText("Set Point");

            if (wave.is_playing)
                playFrame.setText("Stop play");
            else
                playFrame.setText("Start play");
        }
    }

    protected void SetMenu()
    {
        InitColorMenu();
        if (wave.is_image)
            SetImageMenu();
        else
            SetSignalMenu();
        if (parent != null && parent instanceof WaveformManager)
            remove_panel.setEnabled( ( (WaveformManager) parent).
                                    GetWaveformCount() > 1);
    }

    protected void SetMode1D(int mode)
    {
        wave.setSignalMode1D(mode);
    }

    protected void SetMode2D(int mode)
    {
        wave.setSignalMode2D(mode);
    }

    protected void SetMarker(int idx)
    {
        if (wave.GetMarker() != idx)
            wave.SetMarker(idx);
    }

    protected void SetMarkerStep(int step)
    {
        if (wave.GetMarkerStep() != step)
            wave.SetMarkerStep(step);
    }

    public void setParent(Container parent)
    {
        this.parent = parent;
    }

    protected void SetColor(int idx)
    {
        if (wave.GetColorIdx() != idx)
            wave.SetColorIdx(idx);
    }

    public void SetDeselectPoint(Waveform w)
    {
        if (w.ShowMeasure())
        {
            if (parent != null && parent instanceof WaveformManager)
                ( (WaveformManager) parent).SetShowMeasure(false);
            w.SetShowMeasure(false);
        }
        else
        {
            if (parent != null && parent instanceof WaveformManager)
                ( (WaveformManager) parent).SetShowMeasure(true);
            w.SetShowMeasure(true);
            w.SetPointMeasure();
        }
        w.repaint();
    }

    public void itemStateChanged(ItemEvent e)
    {
        Object target = e.getSource();

        if (target instanceof JRadioButtonMenuItem &&
            e.getStateChange() == ItemEvent.SELECTED)
        {
            JRadioButtonMenuItem cb = (JRadioButtonMenuItem) target;
            String action_cmd = cb.getModel().getActionCommand();

            if (action_cmd == null)
                return;

            StringTokenizer act = new StringTokenizer(action_cmd);
            String action = act.nextToken();
            int idx = Integer.parseInt(act.nextToken());

            if (action.equals("MARKER"))
            {
                SetMarker(idx);
                markerStep.setEnabled(! (wave.GetMarker() == Signal.NONE ||
                                         wave.GetMarker() == Signal.POINT));
                //wave.Repaint(true);
                wave.ReportChanges();
                return;
            }

            if (action.equals("MARKER_STEP"))
            {
                SetMarkerStep(Signal.markerStepList[idx]);
                //wave.Repaint(true);
                wave.ReportChanges();
                return;
            }

            if (action.equals("COLOR_LIST"))
            {
                SetColor(idx);
                //wave.Repaint(true);
                wave.ReportChanges();
                return;
            }
        }
    }
}
