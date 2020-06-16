package mds.wave;

import javax.swing.JDialog;
import javax.swing.JFrame;

import mds.wave.FrameData;

import java.awt.event.*;

public class ProfileDialog extends JDialog implements WaveformListener
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	static final String TITLE[] =
	{ "X profile", "Y profile", "Pixel time profile" };
	private final WaveformContainer profile_container;
	// int row[] = {3};
	// Waveform wave[] = new Waveform[3];
	int row[] =
	{ 2 };
	Waveform wave[] = new Waveform[2];
	Waveform w_profile_line = null;
	private Waveform source_profile = null;

	public ProfileDialog(JFrame parent, Waveform source_profile)
	{
		super(parent, "Profile Dialog");
		this.source_profile = source_profile;
		profile_container = new WaveformContainer(row, false);
		final WavePopup wp = new WavePopup();
		profile_container.setPopupMenu(wp);
//        for(int i = 0; i < 3; i++)
		for (int i = 0; i < 2; i++)
		{
			wave[i] = new Waveform();
			wave[i].SetTitle(TITLE[i]);
		}
		profile_container.add(wave);
		getContentPane().add(profile_container);
		addWindowListener(new WindowAdapter()
		{
			@Override
			public void windowClosing(WindowEvent e)
			{
				if (ProfileDialog.this.source_profile != null)
				{
					ProfileDialog.this.source_profile.setSendProfile(false);
					ProfileDialog.this.source_profile.removeWaveformListener(ProfileDialog.this);
				}
				dispose();
			}
		});
		this.setAlwaysOnTop(true);
	}

	public void addProfileLine()
	{
		w_profile_line = new Waveform();
		// profile_container.add(w_profile_line, 4, 1);
		profile_container.add(w_profile_line, 3, 1);
		w_profile_line.SetTitle("Line Profile");
		profile_container.update();
	}

	@Override
	public void processWaveformEvent(WaveformEvent e)
	{
		final WaveformEvent we = e;
		final int we_id = we.getID();
		switch (we_id)
		{
		case WaveformEvent.PROFILE_UPDATE:
			if (isShowing())
			{
				if (e.frame_type == FrameData.BITMAP_IMAGE_32 || e.frame_type == FrameData.BITMAP_IMAGE_16
						|| e.frame_type == FrameData.BITMAP_IMAGE_U32 || e.frame_type == FrameData.BITMAP_IMAGE_U16
						|| e.frame_type == FrameData.BITMAP_IMAGE_F32)
				{
					updateProfiles(e.name, e.x_pixel, e.y_pixel, e.time_value, e.values_x, e.start_pixel_x, e.values_y,
							e.start_pixel_y);
					// e.values_signal, e.frames_time);
					if (e.values_line != null)
						updateProfileLine(e.values_line);
					else
						removeProfileLine();
				}
				else
				{
					updateProfiles(e.name, e.x_pixel, e.y_pixel, e.time_value, e.pixels_x, e.start_pixel_x, e.pixels_y,
							e.start_pixel_y);
					// e.pixels_signal, e.frames_time);
					if (e.pixels_line != null)
						updateProfileLine(e.pixels_line);
					else
						removeProfileLine();
				}
			}
			break;
		}
	}

	public void removeProfileLine()
	{
		if (w_profile_line == null)
			return;
		profile_container.removeComponent(w_profile_line);
		w_profile_line = null;
		profile_container.update();
	}

	public void setWaveSource(Waveform source_profile)
	{
		if (this.source_profile != null)
			(this.source_profile).removeWaveformListener(this);
		this.source_profile = source_profile;
		source_profile.addWaveformListener(this);
	}

	public synchronized void updateProfileLine(float values_line[])
	{
		final float xt[] = new float[values_line.length];
		if (w_profile_line == null)
			addProfileLine();
		for (int i = 0; i < values_line.length; i++)
		{
			xt[i] = i;
		}
		w_profile_line.Update(xt, values_line);
	}

	public synchronized void updateProfileLine(int pixels_line[])
	{
		final float x[] = new float[pixels_line.length];
		final float xt[] = new float[pixels_line.length];
		if (w_profile_line == null)
			addProfileLine();
		for (int i = 0; i < pixels_line.length; i++)
		{
			x[i] = pixels_line[i] & 0xff;
			xt[i] = i;
		}
		w_profile_line.Update(xt, x);
	}

	public synchronized void updateProfiles(String name, int x_pixel, int y_pixel, double time, float values_x[],
			int start_pixel_x, float values_y[], int start_pixel_y)
	// float values_signal[], float frames_time[])
	{
		// if(!name.equals(this.name))
		{
			// this.name = new String(name);
			setTitle("Profile Dialog - " + name + " x_pix : " + x_pixel + " y_pix : " + y_pixel + " time : " + time);
		}
		if (values_x != null && values_x.length > 0)
		{
			final float xt[] = new float[values_x.length];
			for (int i = 0; i < values_x.length; i++)
				xt[i] = (float) start_pixel_x + i;
			wave[0].setProperties(
					"expr=" + name + "\nx_pix=" + x_pixel + "\ny_pix=" + y_pixel + "\ntime=" + time + "\n");
			wave[0].Update(xt, values_x);
		}
		if (values_y != null && values_y.length > 0)
		{
			final float yt[] = new float[values_y.length];
			for (int i = 0; i < values_y.length; i++)
				yt[i] = (float) start_pixel_y + i;
			wave[1].setProperties(
					"expr=" + name + "\nx_pix=" + x_pixel + "\ny_pix=" + y_pixel + "\ntime=" + time + "\n");
			wave[1].Update(yt, values_y);
		}
		/*
		 * if(values_signal != null && values_signal.length > 0 && frames_time != null
		 * && frames_time.length > 0) { wave[2].Update(frames_time, values_signal); }
		 */ }

	public synchronized void updateProfiles(String name, int x_pixel, int y_pixel, double time, int pixels_x[],
			int start_pixel_x, int pixels_y[], int start_pixel_y)
	// int pixels_signal[], float frames_time[])
	{
		// if(!name.equals(this.name))
		{
			// this.name = new String(name);
			setTitle("Profile Dialog - " + name + " x_pix : " + x_pixel + " y_pix : " + y_pixel + " time : " + time);
		}
		if (pixels_x != null && pixels_x.length > 0)
		{
			final float x[] = new float[pixels_x.length];
			final float xt[] = new float[pixels_x.length];
			for (int i = 0; i < pixels_x.length; i++)
			{
				x[i] = pixels_x[i] & 0xff;
				xt[i] = (float) start_pixel_x + i;
			}
			wave[0].setProperties(
					"expr=" + name + "\nx_pix=" + x_pixel + "\ny_pix=" + y_pixel + "\ntime=" + time + "\n");
			wave[0].Update(xt, x);
		}
		if (pixels_y != null && pixels_y.length > 0)
		{
			final float y[] = new float[pixels_y.length];
			final float yt[] = new float[pixels_y.length];
			for (int i = 0; i < pixels_y.length; i++)
			{
				y[i] = pixels_y[i] & 0xff;
				yt[i] = (float) start_pixel_y + i;
			}
			wave[1].setProperties(
					"expr=" + name + "\nx_pix=" + x_pixel + "\ny_pix=" + y_pixel + "\ntime=" + time + "\n");
			wave[1].Update(yt, y);
		}
		/*
		 * if(pixels_signal != null && pixels_x.length > 0 && frames_time != null &&
		 * frames_time.length > 0) { float s[] = new float[pixels_signal.length];
		 * for(int i = 0; i < pixels_signal.length; i++) { s[i] =
		 * (float)(pixels_signal[i] & 0xff); } wave[2].Update(frames_time, s); }
		 */ }
}
