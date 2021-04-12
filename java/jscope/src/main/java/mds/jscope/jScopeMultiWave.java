package mds.jscope;

import java.awt.*;
import java.awt.datatransfer.DataFlavor;
import java.awt.geom.AffineTransform;
import java.awt.image.RenderedImage;
import java.io.IOException;
import java.util.StringTokenizer;

import javax.swing.SwingUtilities;
import javax.swing.TransferHandler;

import mds.connection.UpdateEvent;
import mds.connection.UpdateEventListener;
import mds.wave.*;

/**
 * Class MultiWaveform extends the capability of class Waveform to deal with
 * multiple waveforms.
 */
public class jScopeMultiWave extends MultiWaveform implements UpdateEventListener
{
	// Inner class ToTransferHandler to receive jTraverser info
	class ToTransferHandler extends TransferHandler
	{
		private static final long serialVersionUID = 1L;

		@Override
		public boolean canImport(TransferHandler.TransferSupport support)
		{
			if (!support.isDrop())
				return false;
			if (!support.isDataFlavorSupported(DataFlavor.stringFlavor))
				return false;
			if ((support.getSourceDropActions() & TransferHandler.COPY_OR_MOVE) == 0)
				return false;
			// support.setDropAction(TransferHandler.COPY);
			return true;
		}

		@Override
		public boolean importData(TransferHandler.TransferSupport support)
		{
			if (!canImport(support))
				return false;
			try
			{
				final String data = (String) support.getTransferable().getTransferData(DataFlavor.stringFlavor);
				final StringTokenizer st = new StringTokenizer(data, ":");
				final String experiment = st.nextToken().substring(1);
				final String path = data;
				final boolean doErase = support.getDropAction() == TransferHandler.MOVE;
				new Thread(() ->
				{
					if (doErase)
						wi.Erase();
					wi.setExperiment(experiment);
					wi.AddSignal(path);
					dispatchWaveformEvent(new WaveformEvent(jScopeMultiWave.this, WaveformEvent.EVENT_UPDATE,
							"Update on Drop event "));
				}).start();
			}
			catch (final Exception e)
			{
				return false;
			}
			return true;
		}
	}
	private static final long serialVersionUID = 1L;

	String eventName;

	public jScopeMultiWave(DataProvider dp, jScopeDefaultValues def_values)
	{
		super();
		wi = new MdsWaveInterface(this, dp, def_values);
		setTransferHandler(new ToTransferHandler());
	}

	public void AddEvent() throws IOException
	{
		((MdsWaveInterface) wi).AddEvent(this);
	}

	public void AddEvent(String event) throws IOException
	{
		((MdsWaveInterface) wi).AddEvent(this, event);
	}

	@Override
	protected void DrawImage(Graphics g, Image img, Dimension dim, int type)
	{
		if (type != FrameData.JAI_IMAGE)
		{
			super.DrawImage(g, img, dim, type);
		}
		else
		{
			((Graphics2D) g).clearRect(0, 0, dim.width, dim.height);
			((Graphics2D) g).drawRenderedImage((RenderedImage) img, new AffineTransform(1f, 0f, 0f, 1f, 0F, 0F));
		}
	}

	public String getBriefError(String er, boolean brief)
	{
		if (brief)
		{
			final int idx = (er.indexOf('\n') == -1 ? er.length() : er.indexOf('\n'));
			er = er.substring(0, idx);
		}
		return er;
	}

	@Override
	public ColorMap getColorMap()
	{ return wi.getColorMap(); }

	@Override
	public int GetMarker(int idx)
	{
		if (idx < wi.num_waves)
		{
			return wi.markers[idx];
		}
		return 0;
	}

	@Override
	protected Color getSignalColor(int i)
	{
		if (i > wi.num_waves)
			return Color.black;
		return colors[wi.colors_idx[i] % Waveform.colors.length];
	}

	@Override
	public int getSignalCount()
	{ return wi.num_waves; }

	@Override
	protected String getSignalInfo(int i)
	{
		String s;
		final String name = (wi.in_label != null && wi.in_label[i] != null && wi.in_label[i].length() > 0)
				? wi.in_label[i]
				: wi.in_y[i];
		final String er = (wi.w_error != null && wi.w_error[i] != null) ? " ERROR " : "";
		// If the legend is defined in the signal, override it
		if (signals.size() > i && signals.elementAt(i) != null && signals.elementAt(i).getLegend() != null)
			return signals.elementAt(i).getLegend();
		if (wi.shots != null)
		{
			s = name + " " + wi.shots[i] + er;
		}
		else
		{
			s = name + er;
		}
		if (signals.size() > i && signals.elementAt(i) != null)
		{
//            s += signals.elementAt(i).getName();
			final Signal sign = signals.elementAt(i);
			if (sign != null && sign.getType() == Signal.TYPE_2D)
			{
				switch (sign.getMode2D())
				{
				case Signal.MODE_XZ:
					s = s + " [X-Z Y = " + Waveform.ConvertToString(sign.getYinXZplot(), false) + " ]";
					break;
				case Signal.MODE_YZ:
					s = s + " [Y-Z X = " + sign.getStringOfXinYZplot() +
					// Waveform.ConvertToString(sign.getTime(), false) +
							" ]";
					break;
				/*
				 * case Signal.MODE_YX: s = s + " [Y-X T = " + sign.getStringTime() +
				 * //Waveform.ConvertToString(sign.getTime(), false) + " ]"; break;
				 */
				}
			}
		}
		return s;
	}

	@Override
	public String[] GetSignalsName()
	{
		return wi.GetSignalsName();
	}

	@Override
	public boolean[] GetSignalsState()
	{
		return wi.GetSignalsState();
	}

	@Override
	protected boolean isSignalShow(int i)
	{
		return wi.GetSignalState(i);
	}

	public void jScopeErase()
	{
		Erase();
		wi.Erase();
	}

	public synchronized void jScopeWaveUpdate()
	{
		if (wi.isAddSignal())
		{
			// reset to previous configuration if signal/s are not added
			if (((MdsWaveInterface) wi).prev_wi != null && ((MdsWaveInterface) wi).prev_wi
					.GetNumEvaluatedSignal() == ((MdsWaveInterface) wi).GetNumEvaluatedSignal())
			{
				((MdsWaveInterface) wi).prev_wi.error = ((MdsWaveInterface) wi).error;
				((MdsWaveInterface) wi).prev_wi.w_error = ((MdsWaveInterface) wi).w_error;
				((MdsWaveInterface) wi).prev_wi.setAddSignal(wi.isAddSignal());
				wi = ((MdsWaveInterface) wi).prev_wi;
				wi.SetIsSignalAdded(false);
			}
			else
				wi.SetIsSignalAdded(true);
			((MdsWaveInterface) wi).prev_wi = null;
		}
		Update(wi);
		final WaveformEvent e = new WaveformEvent(this, WaveformEvent.END_UPDATE);
		dispatchWaveformEvent(e);
	}

	@Override
	public void processUpdateEvent(UpdateEvent e)
	{
		eventName = e.getName();
		SwingUtilities.invokeLater(new Runnable()
		{
			@Override
			public void run()
			{
				// System.out.println("Evento su waveform "+e.name);
				final WaveformEvent we = new WaveformEvent(jScopeMultiWave.this, WaveformEvent.EVENT_UPDATE,
						"Update on event " + eventName);
				dispatchWaveformEvent(we);
			}
		});
	}

	public void Refresh()
	{
		setCursor(new Cursor(Cursor.WAIT_CURSOR));
		try
		{
			AddEvent();
		}
		catch (final IOException e)
		{}
		final Thread p = new Thread()
		{
			@Override
			public void run()
			{
				final MdsWaveInterface mwi = (MdsWaveInterface) wi;
				try
				{
					mwi.refresh();
				}
				catch (final Exception exc)
				{}
				SwingUtilities.invokeLater(new Runnable()
				{
					@Override
					public void run()
					{
						jScopeWaveUpdate();
					}
				});
			}
		};
		p.start();
	}

	public void RefreshOnEvent()
	{
		final MdsWaveInterface mwi = (MdsWaveInterface) wi;
		try
		{
			if (!mwi.refreshOnEvent())
				mwi.refresh();
		}
		catch (final Exception exc)
		{
			System.out.println(exc);
		}
		SwingUtilities.invokeLater(new Runnable()
		{
			@Override
			public void run()
			{
				jScopeWaveUpdate();
			}
		});
	}

	public void RemoveEvent() throws IOException
	{
		((MdsWaveInterface) wi).RemoveEvent(this);
	}

	public void RemoveEvent(String event) throws IOException
	{
		((MdsWaveInterface) wi).AddEvent(this, event);
	}

	@Override
	public void removeNotify()
	{
		// System.out.println("Rimuovo jScopeMultiWave");
		try
		{
			RemoveEvent();
		}
		catch (final IOException e)
		{}
		this.wi = null;
		signals = null;
		orig_signals = null;
		final Graphics g = getGraphics();
		g.dispose();
		super.removeNotify();
	}

	@Override
	public void setColorMap(ColorMap colorMap)
	{
		super.setColorMap(colorMap);
		wi.setColorMap(colorMap);
	}

	@Override
	public void SetSignalState(String label, boolean state)
	{
		wi.setSignalState(label, state);
		super.SetSignalState(label, state);
	}

	public void Update(WaveInterface _wi)
	{
		wi = _wi;
		resetMode();
		orig_signals = null;
		super.x_label = wi.xlabel;
		super.y_label = wi.ylabel;
		super.z_label = wi.zlabel;
		super.x_log = wi.x_log;
		super.y_log = wi.y_log;
		// String error = null;
		// if(!wi.isAddSignal())
		wave_error = wi.getErrorTitle(true);
		if (wi.title != null)
			super.title = wi.title;
		else
			super.title = "";
		setColorMap(wi.getColorMap());
		super.show_legend = wi.show_legend;
		super.legend_x = wi.legend_x;
		super.legend_y = wi.legend_y;
		super.is_image = wi.is_image;
		SetFrames(wi.getFrames());
		if (wi.signals != null)
		{
			boolean all_null = true;
			for (int i = 0; i < wi.signals.length; i++)
				if (wi.signals[i] != null)
				{
					all_null = false;
					if (wi.in_label[i] != null && wi.in_label[i].length() != 0)
						wi.signals[i].setName(wi.in_label[i]);
					else
						wi.signals[i].setName(wi.in_y[i]);
					wi.signals[i].setMarker(wi.markers[i]);
					wi.signals[i].setMarkerStep(wi.markers_step[i]);
					wi.signals[i].setInterpolate(wi.interpolates[i]);
					wi.signals[i].setColorIdx(wi.colors_idx[i]);
					wi.signals[i].setMode1D(wi.mode1D[i]);
					wi.signals[i].setMode2D(wi.mode2D[i]);
				}
			if (!all_null)
			{
				Update(wi.signals);
				return;
			}
		}
		if (wi.is_image && wi.getFrames() != null)
		{
			super.frames.setAspectRatio(wi.keep_ratio);
			super.frames.setHorizontalFlip(wi.horizontal_flip);
			super.frames.setVerticalFlip(wi.vertical_flip);
			super.curr_point_sig_idx = 0;
			if (signals.size() != 0)
				signals.removeAllElements();
			if (wi.getModified())
				frame = 0;
			not_drawn = true;
			super.Update();
			return;
		}
		Erase();
	}
}
