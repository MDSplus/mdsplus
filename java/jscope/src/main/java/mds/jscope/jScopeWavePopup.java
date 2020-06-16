package mds.jscope;

import java.awt.Point;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.io.*;
import javax.swing.*;

import mds.wave.*;

class jScopeWavePopup extends MultiWavePopup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	private final SetupDataDialog setup_dialog;
	protected JMenuItem refresh;
	protected JMenuItem selectWave;
	// profile_dialog;
	protected JSeparator sep1;

	public jScopeWavePopup(SetupDataDialog setup_dialog, ProfileDialog profDialog, ColorMapDialog colorMapDialog)
	{
		super(null, profDialog);
		this.setColorMapDialog(colorMapDialog);
		setup.setText("Setup data source...");
		this.setup_dialog = setup_dialog;
		selectWave = new JMenuItem("Select wave panel");
		selectWave.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				if (wave != ((WaveformManager) jScopeWavePopup.this.parent).gGetSelected())
					((WaveformManager) jScopeWavePopup.this.parent).select(wave);
				else
					((WaveformManager) jScopeWavePopup.this.parent).deselect();
			}
		});
		sep1 = new JSeparator();
		refresh = new JMenuItem("Refresh");
		refresh.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				((jScopeWaveContainer) jScopeWavePopup.this.parent).Refresh(((jScopeMultiWave) wave), "Refresh ");
			}
		});
		/*
		 * sep3 = new JSeparator(); saveAsText = new JMenuItem("Save as text ...");
		 * saveAsText.addActionListener(new ActionListener() { public void
		 * actionPerformed(ActionEvent e) { ( (jScopeWaveContainer)
		 * jScopeWavePopup.this.parent).SaveAsText( ( ( jScopeMultiWave) wave), false);
		 * } } );
		 */
		/*************
		 * profile_dialog = new JMenuItem("Show profile dialog");
		 * profile_dialog.addActionListener(new ActionListener() { public void
		 * actionPerformed(ActionEvent e) {
		 * ((jScopeWaveContainer)jScopeWavePopup.this.parent).ShowProfileDialog(((jScopeMultiWave)wave));
		 * } } );
		 **************/
	}

	@Override
	protected void PositionLegend(Point p)
	{
		super.PositionLegend(p);
		((jScopeMultiWave) wave).wi.ShowLegend(true);
		((jScopeMultiWave) wave).wi.SetLegendPosition(((jScopeMultiWave) wave).GetLegendXPosition(),
				((jScopeMultiWave) wave).GetLegendYPosition());
	}

	@Override
	protected void RemoveLegend()
	{
		super.RemoveLegend();
		((jScopeMultiWave) wave).wi.ShowLegend(false);
	}

	@Override
	public void SetColor(int idx)
	{
		super.SetColor(idx);
		final jScopeMultiWave w = (jScopeMultiWave) wave;
		final int sigIdx = w.GetSelectedSignal();
		if (sigIdx != -1 && w.wi.colors_idx[sigIdx] != idx)
		{
			w.wi.colors_idx[w.GetSelectedSignal()] = idx % Waveform.getColors().length;
			w.SetCrosshairColor(idx);
		}
	}

	@Override
	public void SetDeselectPoint(Waveform w)
	{
		final String f_name = System.getProperty("jScope.save_selected_points");
		if (w.ShowMeasure() && f_name != null && f_name.length() != 0)
		{
			long shot = 0;
			final jScopeMultiWave mw = (jScopeMultiWave) w;
			if (mw.wi.shots != null)
				shot = mw.wi.shots[mw.GetSelectedSignal()];
			try
			{
				boolean exist = false;
				final File f = new File(f_name);
				if (f.exists())
					exist = true;
				final BufferedWriter out = new BufferedWriter(new FileWriter(f_name, true));
				if (!exist)
				{
					out.write(" Shot X1 Y1 X2 Y2");
					out.newLine();
				}
				out.write(" " + shot + w.getIntervalPoints());
				out.newLine();
				out.close();
			}
			catch (final IOException e)
			{}
		}
		super.SetDeselectPoint(w);
	}

	@Override
	protected void SetInterpolate(boolean state)
	{
		super.SetInterpolate(state);
		final jScopeMultiWave w = (jScopeMultiWave) wave;
		w.wi.interpolates[w.GetSelectedSignal()] = state;
	}

	@Override
	public void SetMarker(int idx)
	{
		super.SetMarker(idx);
		final jScopeMultiWave w = (jScopeMultiWave) wave;
		if (w.wi.markers[w.GetSelectedSignal()] != idx)
			w.wi.markers[w.GetSelectedSignal()] = idx;
	}

	@Override
	public void SetMarkerStep(int step)
	{
		super.SetMarkerStep(step);
		final jScopeMultiWave w = (jScopeMultiWave) wave;
		if (w.wi.markers_step[w.GetSelectedSignal()] != step)
			w.wi.markers_step[w.GetSelectedSignal()] = step;
	}

	@Override
	protected void SetMenu()
	{
		super.SetMenu();
		// remove_panel.setEnabled(((WaveformManager)parent).GetWaveformCount() > 1);
		jScopeFacade.jScopeSetUI(this);
	}

	@Override
	protected void SetMenuItem(boolean is_image)
	{
		super.SetMenuItem(is_image);
		insert(refresh, this.getComponentCount() - 2);
		setup.setEnabled((setup_dialog != null));
		if (is_image)
		{
			// insert(profile_dialog, 3);
		}
		else
		{
			insert(selectWave, 2);
			add(sep3);
			add(saveAsText);
		}
		// come patch da eliminare
		if (wave != null && ((jScopeMultiWave) wave).wi.num_waves == 1)
			((jScopeMultiWave) wave).wi.signal_select = 0;
	}

	@Override
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

	@Override
	protected void SetMode1D(int mode)
	{
		final jScopeMultiWave w = (jScopeMultiWave) wave;
		w.wi.mode1D[w.GetSelectedSignal()] = mode;
		super.SetMode1D(mode);
	}

	@Override
	protected void SetMode2D(int mode)
	{
		final jScopeMultiWave w = (jScopeMultiWave) wave;
		w.wi.mode2D[w.GetSelectedSignal()] = mode;
		super.SetMode2D(mode);
//        w.Refresh();
	}

	@Override
	public void SetSignalState(String label, boolean state)
	{
		final jScopeMultiWave w = (jScopeMultiWave) wave;
		w.SetSignalState(label, state);
	}

	@Override
	protected void ShowDialog()
	{
		jScopeWavePopup.this.ShowSetupDialog();
	}

	protected void ShowSetupDialog()
	{
		final jScopeMultiWave w = (jScopeMultiWave) wave;
		if (w.mode == Waveform.MODE_POINT) // && w.wi.signal_select != -1)
		{
			setup_dialog.selectSignal(w.GetSelectedSignal());
		}
		else if (w.GetShowSignalCount() > 0 || w.is_image && w.wi.num_waves != 0)
			setup_dialog.selectSignal(1);
		final Timer t = new Timer(20, new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent ae)
			{
				final Point p = ((WaveformManager) jScopeWavePopup.this.parent).getWavePosition(wave);
				setup_dialog.Show(wave, p.x, p.y);
			}
		});
		t.setRepeats(false);
		t.start();
	}
}
